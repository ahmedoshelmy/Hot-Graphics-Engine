#include "physics.hpp"
#include "components/mesh-renderer.hpp"

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

}