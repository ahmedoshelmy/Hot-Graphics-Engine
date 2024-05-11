#include "entity.hpp"
#include "../deserialize-utils.hpp"
#include "../components/component-deserializer.hpp"
#include "../imgui-utils.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function returns the transformation matrix from the entity's local space to the world space
    // Remember that you can get the transformation matrix from this entity to its parent from "localTransform"
    // To get the local to world matrix, you need to combine this entities matrix with its parent's matrix and
    // its parent's parent's matrix and so on till you reach the root.
    glm::mat4 Entity::getLocalToWorldMatrix() const {
        //TODO: (Req 8) Write this function
        Entity* p = this->parent;
        glm::mat4 transformationMatrix = localTransform.toMat4();
        while(p != nullptr){
            // Combining the transformation matrix of the entity with its parent's transformation matrix
            transformationMatrix = p->localTransform.toMat4() * transformationMatrix;
            p = p->parent;
        }
        // return glm::mat4(1.0f);
        return transformationMatrix;
    }

    // Deserializes the entity data and components from a json object
    void Entity::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        name = data.value("name", name);
        debug = data.value("debug", true);
        localTransform.deserialize(data);
        if(data.contains("components")){
            if(const auto& components = data["components"]; components.is_array()){
                for(auto& component: components){
                    deserializeComponent(component, this);
                }
            }
        }
    }


    void Entity::showGUI(CameraComponent* camera) {
        if (debug && name != "") {
            //TODO: fix it so one only can change
            MeshRendererComponent* meshComponent = this->getComponent<MeshRendererComponent>();
            if (meshComponent && camera ) {
                ImGui::PushID(meshComponent->mesh->id);
                if(ImGui::TreeNode(name.c_str())) {
                    imgui_utils::EditTransform(camera, this, meshComponent->mesh->origin);
                    ImGui::TreePop();
                }
                ImGui::PopID();

                if(auto meshMaterial = dynamic_cast<LitMaterial *>(meshComponent->material); meshMaterial) {
                    // ImGui
                }
            }
        }
    }

}