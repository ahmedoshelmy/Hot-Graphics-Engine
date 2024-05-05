#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/camera.hpp"
#include "application.hpp"
#include "components/trigger.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <components/rigid-body.hpp>
#include <iostream>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

namespace our {

    struct bulletCollisionCallback : public btCollisionWorld::ContactResultCallback {
        unsigned int collided_id;
		virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
            collided_id = (size_t)colObj1Wrap->getCollisionObject()->getUserPointer(); 
    		return 1.f;
        }
    };
    // The Collision system is responsible for detecting collision between objects
    class PhysicsSystem {
    public:
        // need for bullet integration
        btBroadphaseInterface* broadphase;
        // Set up the collision configuration and dispatcher
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        // The actual physics solver
        btSequentialImpulseConstraintSolver* solver;
        // The world.
        btDiscreteDynamicsWorld* dynamicsWorld;
        // store the rigid bodies of system 
        std::map<unsigned int, btRigidBody*> rigidBodies;
        std::map<unsigned int, std::string> mp_ids; // mesh id => entity name
        std::map<unsigned int, bool> isGroundOrStairs; // mesh id => is ground or not
        // BulletDebugDrawer_OpenGL* mydebugdrawer;
        // btKinematicCharacterController character;
        // Initialize the renderer 
        btCollisionObject *ghost;
        bulletCollisionCallback collisionCallback;


        void initialize(World* world);
        
        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world, Application* app, float deltaTime) {
            Entity * player = world->getEntity("player");
            if (!player) return;
            unsigned int mesh_selected = getCameraCollidedMesh(world, deltaTime, 1000.0f);
            unsigned int mesh_hit = getPersonCollidedMesh(world, deltaTime) ;
            bool isOnGround = allowMoveOnGround(world, deltaTime, 5.0f);
            if(mesh_hit || !isOnGround) reverseMovement(deltaTime, app, player);
            // For each entity in the world
            auto *rigidBody = player->getComponent<RigidBodyComponent>();
            auto player_pos = player->localTransform.position;
            if (!rigidBody) return;
            for (auto &entity: world->getEntities()) {
                if (entity->name == "player") continue;
                if(checkCollisionRayCasting()){
                    entity->addComponent<TriggerComponent>();
                }
            }
        }

        void destroy();

        bool checkCollision(const glm::vec3 &box1_min, const glm::vec3 &box1_max,
                            const glm::vec3 &box2_min, const glm::vec3 &box2_max);

        bool checkCollisionRayCasting();

        bool checkCollisionByPosition(const glm::vec3 &position, const glm::vec3 &box_max, const glm::vec3 &box_min);

        static bool checkSpheresCollison(const glm::vec3 &center1, const glm::vec3 &center2, const int &radius1,
                                         const int &radius2);

        void reverseMovement(float deltaTime, Application* app, Entity * player);

        std::pair<glm::vec3, glm::vec3> getCollisionBox(Entity *entity);

        unsigned int  getCameraCollidedMesh(World *world, float deltaTime, float distance = 5.0f) ; // return mesh id that collided from camera within certain distance 
        unsigned int  getPersonCollidedMesh(World *world, float deltaTime) ; // return mesh id that person collided with
        bool allowMoveOnGround(World *world, float deltaTime, float distance = 5.0f);
    };
    
}   
