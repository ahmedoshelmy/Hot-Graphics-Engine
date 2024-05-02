#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"

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
        // This should be called every frame to update all entities containing a CollisionComponent.
        void update(World *world, float deltaTime) {
            // For each entity in the world
            Entity *player = nullptr;
            for (auto &entity: world->getEntities()) {
                if (entity->name == "player") {
                    player = entity;
                }
            }
            auto player_position = player->localTransform.position;
            for (auto &entity: world->getEntities()) {
                if (entity->name == "player") continue;
                auto pos = entity->localTransform.position;
                auto [minBox,maxBox] = getCollisionBox(entity);
                if (checkCollisionByPosition(player_position, maxBox,minBox)) {
                    std::cout << "Collision with " << entity->name << "\n";
                }
            }
        }

        bool checkCollision(const glm::vec3 &box1_min, const glm::vec3 &box1_max,
                            const glm::vec3 &box2_min, const glm::vec3 &box2_max);

        bool checkCollisionByPosition(const glm::vec3 &position, const glm::vec3 &box_max, const glm::vec3 &box_min);


        std::pair<glm::vec3, glm::vec3> getCollisionBox(Entity *entity);

    };

}
