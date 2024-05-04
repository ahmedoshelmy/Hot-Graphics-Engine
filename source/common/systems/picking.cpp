#include "picking.hpp"
#include "components/trigger.hpp"
#include <iostream>
#include "../components/pickable.hpp"

namespace our {


    void PickingSystem::pick(World *world, const std::string &item_name) {
        Entity *hand = world->getEntity("hand");
        Entity *object = world->getEntity(item_name);
        // insert child to hand
        object->addParent(hand);

        glm::vec3 &position = object->localTransform.position;
        glm::vec3 &scale = object->localTransform.scale;
        glm::vec3 &rotation = object->localTransform.rotation;
        auto *pickableComponent = object->getComponent<PickableComponent>();
        if (!pickableComponent)return;

        pickableComponent->pickedObjectState = PickedObjectState::PICKED;
        position = pickableComponent->handPosition;
        rotation = pickableComponent->handRotation;
        scale = pickableComponent->handScale;
        itemInRightHand = item_name;
    }

    void PickingSystem::update(our::World *world, our::Application *app, std::string pickedObject) {
        // Check that the user clicked on P
        if (app->getKeyboard().isPressed(GLFW_KEY_P)) {
            if (isPickable(pickedObject)) {
                addToInventory(world, pickedObject);
            }
        }

        if (app->getKeyboard().isPressed(GLFW_KEY_I)) {
            showInventory(world);
            inventoryState = true;
        }
        if (app->getKeyboard().isPressed(GLFW_KEY_C)) {
            hideInventory(world);
            inventoryState = false;
        }
        if (inventoryState) {
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1)) {
                glm::vec2 mousePosition = app->getMouse().getMousePosition();
                std::string clickedItem = getClickedInventoryItem(world, mousePosition.x, mousePosition.y);
                if (!clickedItem.empty()) {
                    if (!itemInRightHand.empty())addToInventory(world, itemInRightHand);
                    pick(world, clickedItem);
                }
            }
        }

//        std::cout << mousePosition.x << " " << mousePosition.y << "\n";




        // This should handle the picking of different things including keys and boxes
//        Entity *Key1 = world->getEntity("key1");
//        auto triggerComponent = Key1->getComponent<TriggerComponent>();
//        if (triggerComponent) {
//            Key1->deleteComponent<TriggerComponent>();
//            // Handle Logic of either picking it or adding it to the inventory
//        }
    }

    void PickingSystem::addToInventory(World *world, std::string item_name) {
        Entity *entity = world->getEntity(item_name);
        Entity *inventory = world->getEntity("Inventory");
        auto *pickableComponent = entity->getComponent<PickableComponent>();
        if (!pickableComponent)return;
        // Add the object to the inventory entities
        pickableComponent->pickedObjectState = PickedObjectState::INVENTORY;
        glm::vec3 &position = entity->localTransform.position;
        glm::vec3 &scale = entity->localTransform.scale;
        glm::vec3 &rotation = entity->localTransform.rotation;
        entity->parent = inventory;
        position = pickableComponent->inventoryPosition;
        rotation = pickableComponent->inventoryRotation;
        scale = pickableComponent->inventoryScale;
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
    }

    void PickingSystem::hideInventory(World *world) {
        // This should hide the inventory
        // This should be done by hiding the children of the inventory entity
        Entity *inventory = world->getEntity("Inventory");
        // Set the scale of the inventory to 0
        glm::vec3 &scale = inventory->localTransform.scale;
        scale = glm::vec3(0);
    }

    bool PickingSystem::isPickable(std::string name) {
        if (name.size() >= 3 && name.substr(0, 3) == "key") return true;
        if (name.size() >= 10 && name.substr(0, 10) == "flashlight") return true;
        return false;
    }

    std::string PickingSystem::getClickedInventoryItem(World *world, const int &mouseX, const int &mouseY) {
        int xStride = 100, yStride = 25;
        for (auto &entityName: inventoryEntities) {
            auto entity = world->getEntity(entityName);
            auto *pickableComponent = entity->getComponent<PickableComponent>();
            if (!pickableComponent)continue;
            if (mouseX >= pickableComponent->inventoryMousePosition.x - xStride &&
                mouseX <= pickableComponent->inventoryMousePosition.x + xStride &&
                mouseY >= pickableComponent->inventoryMousePosition.y - yStride &&
                mouseY <= pickableComponent->inventoryMousePosition.y + yStride
                    ) {
                return entity->name;
            }
        }
        return "";
    }
}
