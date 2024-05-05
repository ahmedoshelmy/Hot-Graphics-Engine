#include "physics.hpp"
#include "components/mesh-renderer.hpp"
#include "components/ground-or-stairs.hpp"
#include "components/free-camera-controller.hpp"
#include "components/camera.hpp"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../physics/physics-utils.hpp"


namespace our {
    

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

    void PhysicsSystem::reverseMovement(float deltaTime,  Application* app, Entity * player) {
        auto *controller = player->getComponent<FreeCameraControllerComponent>();
        glm::vec3 current_sensitivity = controller->positionSensitivity;
        glm::mat4 M = player->localTransform.toMat4();
        glm::vec3 front = glm::vec3(M * glm::vec4(0, 0, -1, 0)),
                up = glm::vec3(M * glm::vec4(0, 1, 0, 0)),
                right = glm::vec3(M * glm::vec4(1, 0, 0, 0));

        glm::vec3 &player_pos = player->localTransform.position;
        if (app->getKeyboard().isPressed(GLFW_KEY_W))
            player_pos -= front * (deltaTime * current_sensitivity.z);

        if (app->getKeyboard().isPressed(GLFW_KEY_S))
            player_pos += front * (deltaTime * current_sensitivity.z);

        if (app->getKeyboard().isPressed(GLFW_KEY_A))
            player_pos += right * (deltaTime * current_sensitivity.x);

        if (app->getKeyboard().isPressed(GLFW_KEY_D))
            player_pos -= right * (deltaTime * current_sensitivity.x);
    }


    bool PhysicsSystem::checkCollisionRayCasting() {
        return false;
    }

    void PhysicsSystem::initialize(World* world)  {
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

        ghost = new btGhostObject();
        ghost->setCollisionShape(new btBoxShape(btVector3(1.0, 2.0, 1.5)));
        ghost->setUserPointer((void *)0);
        mp_ids[0] = "GHOST";
        
        dynamicsWorld->addCollisionObject(ghost);
        // mydebugdrawer = new BulletDebugDrawer_OpenGL();
        // dynamicsWorld->setDebugDrawer(mydebugdrawer);
        // ======================= start creating rigid bodies
        for(auto entity : world->getEntities()){
            // get mesh componenent & skip if it is not
            MeshRendererComponent* meshComponent = entity->getComponent<MeshRendererComponent>();
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
            // add to the world & set pointer to the mesh id
            dynamicsWorld->addCollisionObject(rigidBody);
            rigidBody->setUserPointer((void*)(mesh_id));

            rigidBodies[mesh_id] = rigidBody; // store pointer to it (no need for now but to delete it later)
            mp_ids[mesh_id] = entity->name;   // set mesh id to entity name for debugging

            // ========== if it's ground or stairs set isGroundOrStairs = true
            isGroundOrStairs[mesh_id] = (entity->getComponent<GroundOrStairsComponent>() != nullptr);

        }
    }

    unsigned int PhysicsSystem::getCameraCollidedMesh(World *world, float deltaTime, float distance) {
        
        Entity * camera = world->getEntity("player"); // camera

        if(!camera) return 0;

        dynamicsWorld->stepSimulation(deltaTime, 7);
        // =========== ray test ======
        glm::vec3 out_origin = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0f, 1.0f);
        glm::vec3 out_direction = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0f);

        glm::vec3 out_end = out_origin + out_direction * distance;

        // if you want it by mouse instead of camera -> uncomment

        // glm::vec3 out_origin = getMouseWorldSpace(camera);
        // glm::vec3 out_direction = normalize(out_origin);
        // glm::vec3 out_end = out_origin + out_direction * 100000.0f;

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

    unsigned int  PhysicsSystem::getPersonCollidedMesh(World *world, float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime, 7);

        Entity * camera      = world->getEntity("player"); 
        btTransform camerTransform = physics_utils::getEntityWorldTransform(camera);

        ghost->setWorldTransform(camerTransform);
        collisionCallback.collided_id = 0;
        dynamicsWorld->contactTest(ghost, collisionCallback);

        return collisionCallback.collided_id;
    }

    bool PhysicsSystem::allowMoveOnGround(World *world, float deltaTime, float distance ) {
        Entity * camera = world->getEntity("player"); // camera

        if(!camera) return 0;

        dynamicsWorld->stepSimulation(deltaTime, 7);
        // =========== ray test ======
        glm::vec3 out_origin = camera->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, 0.0f, 1.0f); // origin of camera in the world
        glm::vec3 out_direction =  glm::vec4(0.0, -1.0, 0.0f, 0.0f); // in -ve y axis (down)

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
            unsigned int id = (size_t)RayCallback.m_collisionObject->getUserPointer();
            return isGroundOrStairs[id];
        } 
        
        return 0; // return if no objects
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
    }

}