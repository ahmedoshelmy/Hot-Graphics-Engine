#include "picking.hpp"
#include "components/trigger.hpp"

namespace our {
    void PickingSystem::pick(World *world, std::string item_name) {
        Entity *hand = world->getEntity("hand");
        Entity *object = world->getEntity(item_name);

        // insert child to hand
        object->addParent(hand);

        glm::vec3 &position = object->localTransform.position;
        glm::vec3 &scale = object->localTransform.scale;
        glm::vec3 &rotation = object->localTransform.rotation;

        if (item_name == "flashlight") {
            position = glm::vec3(-1, -1.5, -1); // caught by hand
            scale = glm::vec3(30, 30,30);
            rotation = glm::vec3(3, 1.57, 2);
        } else if (item_name == "key") {
            position = glm::vec3(-1, -1.5, -1); // caught by hand
            scale = glm::vec3(0.009 / (hand->localTransform.scale.x / 10), 0.009 / (hand->localTransform.scale.y / 10),
                              -0.09);
        }


    }

    void PickingSystem::update(our::World *world, our::Application *app) {
        // Check that the user clicked on P
        if (!app->getKeyboard().isPressed(GLFW_KEY_P)) return;
        pick(world, "key");
        // This should handle the picking of different things including keys and boxes
//        Entity *Key1 = world->getEntity("key1");
//        auto triggerComponent = Key1->getComponent<TriggerComponent>();
//        if (triggerComponent) {
//            Key1->deleteComponent<TriggerComponent>();
//            // Handle Logic of either picking it or adding it to the inventory
//        }
    }

    void PickingSystem::addToInventory(World *world, std::string item_name) {
        Entity *object = world->getEntity(item_name);


        glm::vec3 &position = object->localTransform.position;
        glm::vec3 &scale = object->localTransform.scale;
        glm::vec3 &rotation = object->localTransform.rotation;

        // If scale is 0, it will disappear xD
        // TODO, This should be changed to insert it as a child to the inventory
        scale = glm::vec3(0);
    }

}
