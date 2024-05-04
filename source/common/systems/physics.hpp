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

namespace our {

    
    // The Collision system is responsible for detecting collision between objects
    class PhysicsSystem {
    public:
        Application *app;
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
        // BulletDebugDrawer_OpenGL* mydebugdrawer;

        // Initialize the renderer 
        void initialize(World* world);
        
        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world, float deltaTime) {
            std::cout << "Camera hit: " << mp_ids[ getCameraCollidedMesh(world, deltaTime, 1.0f)] << "\n";
            // For each entity in the world
            Entity * player = world->getEntity("player");
            if (!player) return;
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

        void reverseMovement(float deltaTime,Entity * player);

        std::pair<glm::vec3, glm::vec3> getCollisionBox(Entity *entity);

        unsigned int  getCameraCollidedMesh(World *world, float deltaTime, float distance = 5.0f) ; // return mesh id that collided from camera within certain distance 
        unsigned int  getPersonCollidedMesh(World *world, float deltaTime) ; // return mesh id that person collided with
        
    };

}
