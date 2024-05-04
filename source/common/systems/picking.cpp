#include "picking.hpp"
#include "components/trigger.hpp"

namespace our {
    void PickingSystem::pick(World *world, std::string item_name) {
        Entity *hand = world->getEntity("hand");
        Entity *object = world->getEntity(item_name);

        // Add the object to the inventory entities
        addToInventory(world, item_name);

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
        if (app->getKeyboard().isPressed(GLFW_KEY_P)) pick(world, "key");

        if (app->getKeyboard().isPressed(GLFW_KEY_I)) {            
            showInventory(world);
            inventoryState = true;
        }

        if (app->getKeyboard().isPressed(GLFW_KEY_C)) {            
            hideInventory(world);
            inventoryState = false;
        }


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

        // Add the object to the inventory entities
        inventoryEntities.push_back(object);
    }

    void PickingSystem::showInventory(World *world) {
        // This should show the inventory
        // This should be done by showing the children of the inventory entity
        Entity *inventory = world->getEntity("Inventory");
        // Set the position, scale and rotation of the inventory
        glm::vec3 &position = inventory->localTransform.position;
        glm::vec3 &rotation = inventory->localTransform.rotation;
        glm::vec3 &scale = inventory->localTransform.scale;

        position = glm::vec3(-0.1, 0, -2);
        rotation = glm::vec3(1.5, 0, 0);
        scale = glm::vec3(1.1);

        // Show the children of the inventory entity
        for (Entity *entity: inventoryEntities) {
            entity->parent = inventory;

            // Set the position, scale and rotation of the object to be in the correct position in the inventory
            glm::vec3 &position = entity->localTransform.position;
            glm::vec3 &scale = entity->localTransform.scale;
            glm::vec3 &rotation = entity->localTransform.rotation;

            position = glm::vec3(-1.6, 0.1, -0.5);
            rotation = glm::vec3(1.8, 3.1, -0.05);
            scale = glm::vec3(0.03, 0.03, 0.03);
        }

        
    }
    
    void PickingSystem::hideInventory(World *world) {
        // This should hide the inventory
        // This should be done by hiding the children of the inventory entity
        Entity *inventory = world->getEntity("Inventory");
        // Set the scale of the inventory to 0
        glm::vec3 &scale = inventory->localTransform.scale;
        scale = glm::vec3(0);
    }

}
