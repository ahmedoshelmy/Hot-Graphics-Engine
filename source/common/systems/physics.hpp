#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "application.hpp"
#include "components/trigger.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <components/rigid-body.hpp>
#include <iostream>

namespace our {

    // The Collision system is responsible for detecting collision between objects
    class PhysicsSystem {
    public:
        Application *app;

        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world, float deltaTime) {
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

        bool checkCollision(const glm::vec3 &box1_min, const glm::vec3 &box1_max,
                            const glm::vec3 &box2_min, const glm::vec3 &box2_max);

        bool checkCollisionRayCasting();

        bool checkCollisionByPosition(const glm::vec3 &position, const glm::vec3 &box_max, const glm::vec3 &box_min);

        static bool checkSpheresCollison(const glm::vec3 &center1, const glm::vec3 &center2, const int &radius1,
                                         const int &radius2);

        void reverseMovement(float deltaTime,Entity * player);

        std::pair<glm::vec3, glm::vec3> getCollisionBox(Entity *entity);

    };

}
