#include "picking.hpp"
#include "components/trigger.hpp"
#include <iostream>
#include "../components/pickable.hpp"
#include "../components/mesh-renderer.hpp"

namespace our {


    void PickingSystem::pick(World *world, const std::string &item_name) {
        Entity *hand = world->getEntity("player");
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
        showMessage("You added to the inventory a " + item_name + " Press [I] to open the inventory", 1);

    }

    void PickingSystem::update(our::World *world, our::Application *app, TextRenderer *renderer, double deltaTime) {
        if (!this->renderer) this->renderer = renderer;
        currentTime += deltaTime;

        pickedObject = app->pickedObject;
        mousePickedObject = app->mousePickedObject;

        bool isEntityPickable = isPickable(pickedObject);
        // show text to help user
        if (isEntityPickable) {
            renderer->addTextCommand("Right Click to Pick it", 0.05, 1500, 50, 0.6, glm::vec3(0.96, 1.0, 0.78), -1, 1);
        }
        // change highlight of crosshair
        Entity *crosshair = world->getEntity("CrossHair");
        if (crosshair) {
            auto crossComponent = crosshair->getComponent<MeshRendererComponent>();
            auto crossMaterial = dynamic_cast<TintedMaterial *>(crossComponent->material);
            if (isEntityPickable)
                crossMaterial->tint = glm::vec4(0.45, 0.9, 0.5, 0.5);
            else
                crossMaterial->tint = glm::vec4(0.45, 0.4, 0.5, 0.5);
        }
        if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (isEntityPickable) {
                addToInventory(world, pickedObject);
                lastTimePicked = currentTime;
                app->song.second = 1;
                app->song = {"assets/music/Collect Item Sound Effect.mp3", 1};
            }
        }
        if (app->getKeyboard().isPressed(GLFW_KEY_I)) {
            showInventory(world);
            inventoryState = true;
            app->song = {"assets/music/inventory.mp3", 1};
        }
        if (app->getKeyboard().isPressed(GLFW_KEY_ESCAPE)) {
            hideInventory(world);
            inventoryState = false;
        }
        if (inventoryState) {
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1)) {
                if (!mousePickedObject.empty() && isPickable(mousePickedObject) &&
                    currentTime - lastTimeInInventory > 0.5 && itemInRightHand != mousePickedObject) {
                    // if(itemInRightHand == mousePickedObject)
                    if (!itemInRightHand.empty()) addToInventory(world, itemInRightHand);
                    pick(world, mousePickedObject);
                    lastTimeInInventory = currentTime;
                }
            }
        }
        app->inHandItem = itemInRightHand;
    }

    void PickingSystem::addToInventory(World *world, std::string item_name) {
        Entity *entity = world->getEntity(item_name);
        Entity *inventory = world->getEntity("Inventory");
        auto *pickableComponent = entity->getComponent<PickableComponent>();
        if (!pickableComponent) return;
        // Add the object to the inventory entities
        pickableComponent->pickedObjectState = PickedObjectState::INVENTORY;
        glm::vec3 &position = entity->localTransform.position;
        glm::vec3 &scale = entity->localTransform.scale;
        glm::vec3 &rotation = entity->localTransform.rotation;
        entity->parent = inventory;
        position = pickableComponent->inventoryPosition;
        rotation = pickableComponent->inventoryRotation;
        scale = pickableComponent->inventoryScale;
        showMessage("You picked " + item_name + " Press I to to open the inventory ", 5);
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
            if (!entity)continue;
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

    void PickingSystem::showMessage(std::string text, double time) {
        renderer->addTextCommand(text, time, 1500, 50, 0.6, glm::vec3(0.96, 1.0, 0.78), 0, 1);

    }
}
