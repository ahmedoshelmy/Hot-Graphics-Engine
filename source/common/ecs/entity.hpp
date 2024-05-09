#pragma once

#include "component.hpp"
#include "transform.hpp"
#include <list>
#include <iterator>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace our {

    class World; // A forward declaration of the World Class

    class Entity {
        World *world; // This defines what world own this entity
        std::list<Component *> components; // A list of components that are owned by this entity

        friend World; // The world is a friend since it is the only class that is allowed to instantiate an entity
        Entity() = default; // The entity constructor is private since only the world is allowed to instantiate an entity
    public:
        unsigned int id;
        std::string name; // The name of the entity. It could be useful to refer to an entity by its name
        bool debug;
        Entity *parent;   // The parent of the entity. The transform of the entity is relative to its parent.
        // If parent is null, the entity is a root entity (has no parent).
        Transform localTransform; // The transform of this entity relative to its parent.

        World *getWorld() const { return world; } // Returns the world to which this entity belongs

        glm::mat4
        getLocalToWorldMatrix() const; // Computes and returns the transformation from the entities local space to the world space
        void deserialize(const nlohmann::json &); // Deserializes the entity data and components from a json object

        // This template method create a component of type T,
        // adds it to the components map and returns a pointer to it 
        template<typename T>
        T *addComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            //TODO: (Req 8) Create an component of type T, set its "owner" to be this entity, then push it into the component's list
            // Don't forget to return a pointer to the new component
            T *comp = new T();
            comp->owner = this;
            components.push_back(comp);
            return comp;
            // return nullptr;
        }

        // This template method searhes for a component of type T and returns a pointer to it
        // If no component of type T was found, it returns a nullptr 
        template<typename T>
        T *getComponent() {
            //TODO: (Req 8) Go through the components list and find the first component that can be dynamically cast to "T*".
            // Return the component you found, or return null of nothing was found.
            for (auto comp: components) {
                T *t = dynamic_cast<T *>(comp);
                if (t != nullptr) { // First component of type T found
                    return t;
                }
            }
            return nullptr; // Not found
        }

        // This template method dynami and returns a pointer to it
        // If no component of type T was found, it returns a nullptr 
        template<typename T>
        T *getComponent(size_t index) {
            auto it = components.begin();
            std::advance(it, index);
            if (it != components.end())
                return dynamic_cast<T *>(*it);
            return nullptr;
        }

        // This template method searhes for a component of type T and deletes it
        template<typename T>
        void deleteComponent() {
            //TODO: (Req 8) Go through the components list and find the first component that can be dynamically cast to "T*".
            // If found, delete the found component and remove it from the components list
            T *del_component = getComponent<T>();
            if (del_component != nullptr) {
                this->deleteComponent<T>(del_component);
            }
        }

        // This template method searhes for a component of type T and deletes it
        void deleteComponent(size_t index) {
            auto it = components.begin();
            std::advance(it, index);
            if (it != components.end()) {
                delete *it;
                components.erase(it);
            }
        }

        // This template method searhes for the given component and deletes it
        template<typename T>
        void deleteComponent(T const *component) {
            //TODO: (Req 8) Go through the components list and find the given component "component".
            // If found, delete the found component and remove it from the components list
            for (auto it = components.begin(); it != components.end(); it++) {
                if (*it == component) { // Component found "component"
                    delete *it;
                    components.erase(it);
                    break;
                }
            }
        }


        void showGUI() {
            if (debug && name != "") {
                if (ImGui::CollapsingHeader(name.c_str())) {
                    ImGui::InputFloat3(("position "+name).c_str(), glm::value_ptr(localTransform.position), 3, 0.0f);
                    ImGui::InputFloat3(("rotation "+name).c_str(), glm::value_ptr(localTransform.rotation), 3, 0.0f);
                    ImGui::InputFloat3(("scaling  "+name).c_str(), glm::value_ptr(localTransform.scale), 3, 0.0f);
                }
            }
        }

        // This adds a parent for the entity like the hand is a child for the person and the object(e.g. key) is a child for the hand
        void addParent(Entity *child) {
            this->parent = child;
        }

        // Since the entity owns its components, they should be deleted alongside the entity
        ~Entity() {
            //TODO: (Req 8) Delete all the components in "components".
            for (auto comp: components) {
                delete comp;
            }
        }

        // Entities should not be copyable
        Entity(const Entity &) = delete;

        Entity &operator=(Entity const &) = delete;
    };

}