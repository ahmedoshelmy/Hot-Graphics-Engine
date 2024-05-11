#include "physics.hpp"
// #include "components/mesh-renderer.hpp"
// #include "components/ground-or-stairs.hpp"

#include "components/camera.hpp"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../physics/physics-utils.hpp"


namespace our {
    
    void PhysicsSystem::initialize(World* world, glm::ivec2 windowSize)  {

        this->windowSize = windowSize;
        this->mouseSelected = "";
        this->cameraSelected = "";
        mapped_entities[0] = "";
        // initalizing bullet physics engine
        broadphase = new btDbvtBroadphase();
        // Set up the collision configuration and dispatcher
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        // The actual physics solver
        solver = new btSequentialImpulseConstraintSolver;
        // The physics world.
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0,-9.81f,0));
        // ghost body (act as player)
        ghost = new btGhostObject();
        ghost->setCollisionShape(new btSphereShape(1.0f));
        ghost->setUserPointer((void *)0);
        mp_ids[0] = "GHOST";

        dynamicsWorld->addCollisionObject(ghost);
        
        // mydebugdrawer = new BulletDebugDrawer_OpenGL();
        // dynamicsWorld->setDebugDrawer(mydebugdrawer);

        // start creating rigid bodies
        for(auto entity : world->getEntities()){
            // get mesh componenent & skip if it is not
            MeshRendererComponent* meshComponent = entity->getComponent<MeshRendererComponent>();
            GroundOrStairsComponent* groundComponent = entity->getComponent<GroundOrStairsComponent>();
            if(meshComponent == nullptr) continue;
            unsigned int mesh_id = meshComponent->mesh->id; // mesh id
            // create rigid body and set it's id
            btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
                0,                                     // mass, in kg. 0 -> Static object, will never move.
                physics_utils::prepareMotionStateEntity(entity),       // get motion state (local to world & set scaling of collision shape)
                meshComponent->mesh->shape,            // collision shape of body
                btVector3(0,0,0)                      // local inertia
            );
            btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
            rigidBody->setUserPointer((void*)(entity->id));
            // store debugging information and maps
            rigidBodies[entity->id] = rigidBody; // store pointer to it (no need for now but to delete it later)
            mp_ids[entity->id] = entity->name;   // set mesh id to entity name for debugging
            isGroundOrStairs[entity->id] = (groundComponent != nullptr);
            // add to the world & set pointer to the mesh id
            dynamicsWorld->addCollisionObject(rigidBody);
        }

        // init gpu accelerated picking buffer
        initCastingBuffer();
    }
    
    void PhysicsSystem::update(World *world, Application* app, float deltaTime) {
        
        updatePicking(world, app, deltaTime);
        avoidCollision(world, app, deltaTime);
        // std::cout << " hit: " << mp_ids[ground_id] << " " << hit_fraction << " " << dist <<  "\n";
        // std::cout << glm::sign(cameraForward.x) << " " << glm::sign(cameraForward.y) << " " << glm::sign(cameraForward.z) << "\n";
    }

    void PhysicsSystem::updatePicking(World *world, Application* app, float deltaTime) {
        pickingPhaseRenderer(world, windowSize);
        // read pixel that camera point on (since camera always point to center)
        PixelInfo pixel = readPixel(windowSize.x / 2, windowSize.y / 2);
        // get picked by camera entity name
        prevCameraSelected = cameraSelected;
        cameraSelected = mapped_entities[pixel.ObjectID];
        // read pixel of mouse info
        glm::vec2 mouse_coord = app->getMouse().getMousePosition();
        mouse_coord.y = windowSize.y - mouse_coord.y;
        // since opengl read y from (0_top) where opposite happen in renderer (0_bottom) we need to reverse directions
        PixelInfo mouse_pixel = readPixel(mouse_coord.x, mouse_coord.y);
        // get selected item with previouse
        prevMouseSelected = mouseSelected;
        mouseSelected = mapped_entities[mouse_pixel.ObjectID];
    }

    void PhysicsSystem::avoidCollision(World *world, Application* app, float deltaTime) {
        Entity * player = world->getEntity("player");
        if (!player) return;
        // controller of FPS camera if not exist (Free Camera) there's no collision or stairs walk
        auto *controller = player->getComponent<FpsCameraControllerComponent>();
        if(!controller) return;

        glm::vec3 current_sensitivity = controller->positionSensitivity;

        // unsigned int mesh_selected = getCameraCollidedMesh(world, deltaTime, 1000.0f);
        unsigned int mesh_hit =  getPersonCollidedMesh(world, deltaTime) ;
        unsigned int ground_id = allowMoveOnGround(world, deltaTime, factorDist);

        Entity *groundEntity = world->getEntity(mp_ids[ground_id]);
        GroundOrStairsComponent *ground = groundEntity != nullptr ? groundEntity->getComponent<GroundOrStairsComponent>() : nullptr;



        if((ground != nullptr && !ground->dy && mesh_hit) || (ground == nullptr && mesh_hit)){
            // std::cout<<mp_ids[mesh_hit]<<"\n";
            if(!isOpenDoor(world->getEntity(mp_ids[mesh_hit])))
                reverseMovement(deltaTime, app, player);
        }
        // if stairs
        if(ground != nullptr && ground->dy) {
            glm::vec3 cameraForward =  player->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0, 0.0);
            bool isUp = cameraForward.x < 0;
            float dist = factorDist * hit_fraction;

            MeshRendererComponent* stairs_mesh = groundEntity->getComponent<MeshRendererComponent>();

            if(((isUp && app->getKeyboard().isPressed(GLFW_KEY_W)) || (!isUp && app->getKeyboard().isPressed(GLFW_KEY_S)))
                && dist <= 5.0f)
            {
                player->localTransform.position.y += ground->dy  * (deltaTime * current_sensitivity.z );
            }
            else if(((!isUp && app->getKeyboard().isPressed(GLFW_KEY_W)) || (isUp && app->getKeyboard().isPressed(GLFW_KEY_S)))
                && dist >= 5.0f)
            {
                player->localTransform.position.y -= ground->dy  * (deltaTime * current_sensitivity.z );
            }
        }
    }

    bool PhysicsSystem::checkCollision(const glm::vec3 &box1_min, const glm::vec3 &box1_max,
                                       const glm::vec3 &box2_min, const glm::vec3 &box2_max) {
        // Check along x-axis
        if (box1_max.x < box2_min.x || box2_max.x < box1_min.x) {
            return false;
        }

        // Check along y-axis
        if (box1_max.y < box2_min.y || box2_max.y < box1_min.y) {
            return false;
        }

        // Check along z-axis
        if (box1_max.z < box2_min.z || box2_max.z < box1_min.z) {
            return false;
        }

        // The two boxes are colliding
        return true;
    }

    std::pair<glm::vec3, glm::vec3> PhysicsSystem::getCollisionBox(Entity *entity) {
        // get local to world matrix
        glm::mat4 M = entity->getLocalToWorldMatrix();

        // get the mesh component to get its min and max positions
        MeshRendererComponent *meshComponent = entity->getComponent<MeshRendererComponent>();

        if (!meshComponent)
            std::cout << "No mesh component " << std::endl;

        // min and max positions of box
        glm::vec3 minPos = meshComponent->mesh->min;
        glm::vec3 maxPos = meshComponent->mesh->max;

        glm::vec3 boxes[8] = {
                glm::vec3(minPos.x, minPos.y, minPos.z),
                glm::vec3(minPos.x, maxPos.y, minPos.z),
                glm::vec3(maxPos.x, maxPos.y, minPos.z),
                glm::vec3(maxPos.x, minPos.y, minPos.z),
                glm::vec3(minPos.x, minPos.y, maxPos.z),
                glm::vec3(minPos.x, maxPos.y, maxPos.z),
                glm::vec3(maxPos.x, maxPos.y, maxPos.z),
                glm::vec3(maxPos.x, minPos.y, maxPos.z)};

        // transform each point to its position in space

        for (int i = 0; i < 8; i++) {
            boxes[i] = M * glm::vec4(boxes[i], 1.0);
        }

        glm::vec3 minPos_afterTransformation = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxPos_afterTransformation = glm::vec3(std::numeric_limits<float>::min());

        // loop over each vertex
        for (auto &vertix: boxes) {
            minPos_afterTransformation.x = std::min(minPos_afterTransformation.x, vertix.x);
            minPos_afterTransformation.y = std::min(minPos_afterTransformation.y, vertix.y);
            minPos_afterTransformation.z = std::min(minPos_afterTransformation.z, vertix.z);
            maxPos_afterTransformation.x = std::max(maxPos_afterTransformation.x, vertix.x);
            maxPos_afterTransformation.y = std::max(maxPos_afterTransformation.y, vertix.y);
            maxPos_afterTransformation.z = std::max(maxPos_afterTransformation.z, vertix.z);
        }
        return std::make_pair(minPos_afterTransformation, maxPos_afterTransformation);
    }

    bool PhysicsSystem::checkCollisionByPosition(const glm::vec3 &position, const glm::vec3 &box_max,
                                                 const glm::vec3 &box_min) {
        // Check if the position is within the boundaries of the box
        return (position.x >= box_min.x && position.x <= box_max.x &&
                position.y >= box_min.y && position.y <= box_max.y &&
                position.z >= box_min.z && position.z <= box_max.z);
    }

    bool PhysicsSystem::checkSpheresCollison(const glm::vec3 &center1, const glm::vec3 &center2, const int &radius1,
                                             const int &radius2) {
        double distance = sqrt(
                (center1.x - center2.x) * (center1.x - center2.x) +
                (center1.y - center2.y) * (center1.y - center2.y) +
                (center1.z - center2.z) * (center1.z - center2.z)
        );
        return distance < radius1 + radius2;
    }

    void PhysicsSystem::reverseMovement(float deltaTime,  Application* app, Entity * player, float deltaReverseSpeed) {
        auto *controller = player->getComponent<FpsCameraControllerComponent>();
        if(controller == nullptr) return;

        glm::vec3 current_sensitivity = controller->positionSensitivity;
        glm::mat4 M = player->localTransform.toMat4();
        glm::vec3 front = glm::vec3(M * glm::vec4(0, 0, -1, 0)),
                up = glm::vec3(M * glm::vec4(0, 1, 0, 0)),
                right = glm::vec3(M * glm::vec4(1, 0, 0, 0));
        float prev_y = player->localTransform.position.y;
        glm::vec3 &player_pos = player->localTransform.position;
        if (app->getKeyboard().isPressed(GLFW_KEY_W))
            player_pos -= front * (deltaTime * current_sensitivity.z );

        if (app->getKeyboard().isPressed(GLFW_KEY_S))
            player_pos += front * (deltaTime * current_sensitivity.z );

        if (app->getKeyboard().isPressed(GLFW_KEY_A))
            player_pos += right * (deltaTime * current_sensitivity.x );

        if (app->getKeyboard().isPressed(GLFW_KEY_D))
            player_pos -= right * (deltaTime * current_sensitivity.x );

        player_pos.y = prev_y;
    }


    // perform ray picking using ray cast from camera within certain distance
    unsigned int PhysicsSystem::getCameraCollidedMesh(World *world, float deltaTime, float distance) {

        Entity * camera = world->getEntity("player"); // camera

        if(!camera) return 0;

        dynamicsWorld->stepSimulation(deltaTime, 7);
        // =========== ray test ======
        glm::vec3 out_origin = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0f, 1.0f);
        glm::vec3 out_direction = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0f);

        glm::vec3 out_end = out_origin + out_direction * distance;

        btCollisionWorld::ClosestRayResultCallback RayCallback(
            btVector3(out_origin.x, out_origin.y, out_origin.z),
            btVector3(out_end.x, out_end.y, out_end.z)
        );

        dynamicsWorld->rayTest(
            btVector3(out_origin.x, out_origin.y, out_origin.z),
            btVector3(out_end.x, out_end.y, out_end.z),
            RayCallback
        );


        if(RayCallback.hasHit()) {
            return (size_t)RayCallback.m_collisionObject->getUserPointer();
        }
        return 0; // return if no objects
    }

    // perform ray picking using ray cast from mouse within certain distance
    unsigned int PhysicsSystem::getMouseCollidedMesh(World *world, Application* app, float deltaTime, float distance) {

        Entity * camera = world->getEntity("player"); // camera

        if(!camera) return 0;

        dynamicsWorld->stepSimulation(deltaTime, 7);
        // =========== ray test ======
        // if you want it by mouse instead of camera -> uncomment

        glm::vec3 out_origin = physics_utils::getMouseWorldSpace(app->getMouse().getMousePosition(), camera->getComponent<CameraComponent>(), windowSize);
        glm::vec3 out_direction = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0f);
        glm::vec3 out_end = out_origin + out_direction * distance;

        btCollisionWorld::ClosestRayResultCallback RayCallback(
            btVector3(out_origin.x, out_origin.y, out_origin.z),
            btVector3(out_end.x, out_end.y, out_end.z)
        );

        dynamicsWorld->rayTest(
            btVector3(out_origin.x, out_origin.y, out_origin.z),
            btVector3(out_end.x, out_end.y, out_end.z),
            RayCallback
        );


        if(RayCallback.hasHit()) {
            return (size_t)RayCallback.m_collisionObject->getUserPointer();
        }
        return 0; // return if no objects
    }

    // perform collision with any mesh
    unsigned int  PhysicsSystem::getPersonCollidedMesh(World *world, float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime, 7);

        Entity * camera      = world->getEntity("player");
        btTransform camerTransform = physics_utils::getEntityWorldTransform(camera);

        ghost->setWorldTransform(camerTransform);
        collisionCallback.collided_id = 0;
        dynamicsWorld->contactTest(ghost, collisionCallback);
        return collisionCallback.collided_id;
    }

    // this function work by cast a ray from camera origin to ground *
    unsigned int PhysicsSystem::allowMoveOnGround(World *world, float deltaTime, float distance ) {
       // get gamera origin & if not camera exist don't
        Entity * camera = world->getEntity("player"); // camera
        if(!camera) return 0;
        dynamicsWorld->stepSimulation(deltaTime, 7);
        // =========== ray test ======
        // glm::vec3 out_origin = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0f, 1.0f); // origin of camera in the world
        glm::vec3 out_direction =  glm::vec4(0.0, -1.0, 0.0f, 0.0f); // in -ve y axis (down)


        glm::vec3 origin = camera->getLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 out_end = origin + out_direction * distance;

        btCollisionWorld::ClosestRayResultCallback RayCallback(
            btVector3(origin.x, origin.y, origin.z),
            btVector3(out_end.x, out_end.y, out_end.z)
        );

        dynamicsWorld->rayTest(
            btVector3(origin.x, origin.y, origin.z),
            btVector3(out_end.x, out_end.y, out_end.z),
            RayCallback
        );

        if(RayCallback.hasHit()) {
            unsigned int id = (size_t)RayCallback.m_collisionObject->getUserPointer();
            hit_fraction = RayCallback.m_closestHitFraction;
            if(isGroundOrStairs[id]) return id;
        }
        // std::cout << (int)(should_move) << "\n";
        return 0;
    }

    void PhysicsSystem::destroy() {
        for(auto& body : rigidBodies) {
            dynamicsWorld->removeRigidBody(body.second);
            delete body.second;
        }
        dynamicsWorld->removeCollisionObject(ghost);
        delete ghost;

        delete broadphase;
        delete collisionConfiguration;
        delete dispatcher;
        delete solver;
        delete dynamicsWorld;

        glDeleteFramebuffers(1, &castingFBO);
        glDeleteVertexArrays(1, &castingVAO);
        delete primitiveCastingTarget;
        delete depthCastingTarget;
        delete castingMaterial->sampler;
        delete castingMaterial->shader;
        delete castingMaterial;
    }

    bool PhysicsSystem::isOpenDoor(Entity * entity) {
        auto * knobComponent = entity->getComponent<KnobComponent>();
        if(!knobComponent) return false;
        return knobComponent->open;

    }
    
    // GPU Accelerated Functions
    void PhysicsSystem::initCastingBuffer() {
        glGenFramebuffers(1, &castingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, castingFBO);

        primitiveCastingTarget = texture_utils::empty(windowSize, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, primitiveCastingTarget->getOpenGLName(), 0);

        depthCastingTarget = our::texture_utils::empty(windowSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCastingTarget->getOpenGLName(), 0);
        // check status
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR:: CASTING FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }

        //Unbind the framebuffer and texture just to be safe
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Create a sampler to use for sampling the scene texture in the post processing shader
        glGenVertexArrays(1, &castingVAO);

        Sampler* castingSampler = new Sampler();
        castingSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        castingSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        castingSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        castingSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create the post processing shader
        ShaderProgram* castingShader = new ShaderProgram();
        castingShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
        castingShader->attach("assets/shaders/picking.frag", GL_FRAGMENT_SHADER);
        castingShader->link();

        // Create a post processing material
        castingMaterial = new TexturedMaterial();
        castingMaterial->shader = castingShader;
        castingMaterial->texture = primitiveCastingTarget;
        castingMaterial->sampler = castingSampler;
        // The default options are fine but we don't need to interact with the depth buffer
        // so it is more performant to disable the depth mask
        // castingMaterial->pipelineState.depthMask = false;
        // castingMaterial->pipelineState.depthTesting.enabled = false;

    }
        
    // this function used to implement GPU accelerated picking by renderer all obaqu meshes
    void PhysicsSystem::pickingPhaseRenderer(our::World *world, glm::ivec2 windowSize) {
        glViewport(0, 0, windowSize[0], windowSize[1]);


        //Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0);
        //Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, castingFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // get camera
        Entity* cameraEntity = world->getEntity("player");
        if(!cameraEntity) return;
        CameraComponent* camera = cameraEntity->getComponent<CameraComponent>();
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // mapped_entities.clear();
        // loop as it is and draw id to pixel
        unsigned int gObjectIndex = 1;
        for(auto entity : world->getEntities()) {
            // TODO: add alpha or support for transparent material
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer && !meshRenderer->material->transparent) {
                castingMaterial->setup();
                mapped_entities[gObjectIndex] = meshRenderer->getOwner()->name;
                castingMaterial->shader->set("gObjectIndex", gObjectIndex++);
                castingMaterial->shader->set("transform", VP * meshRenderer->getOwner()->getLocalToWorldMatrix());
                meshRenderer->mesh->draw();
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *) 0);
            }   
        }
        // for assureness
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

}