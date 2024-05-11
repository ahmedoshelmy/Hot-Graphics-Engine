#pragma once

#include "../ecs/world.hpp"
#include "../components/light-component.hpp"


namespace our {

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class LightSystem {
    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
        void showGUI(World *world) {
            // For each entity in the world
            for (auto entity: world->getEntities()) {
                // Get the movement component if it exists
                LightComponent *light = entity->getComponent<LightComponent>();
                if (!light) continue;
                if (light->debug) {
                    ImGui::Begin(light->name.c_str());
                    ImGui::Text("Light Component");
                    ImGui::InputFloat3("position", glm::value_ptr(light->getOwner()->localTransform.position), 3, 0);
                    ImGui::ColorEdit3("color", glm::value_ptr(light->color), 0.0);
                    ImGui::SliderFloat("cutOff", &light->cutOff, 0.0, 180.0, "%.3f", 1.0f);
                    ImGui::SliderFloat("outerCutOff", &light->outerCutOff, 0.0, 180.0, "%.3f", 1.0f);
                    ImGui::InputFloat("attenuationConstant", &light->attenuationConstant, 0.1f, 1.0f, "%.3f");
                    ImGui::InputFloat("attenuationLinear", &light->attenuationLinear, 0.1f, 1.0f, "%.3f");
                    ImGui::InputFloat("attenuationQuadratic", &light->attenuationQuadratic, 0.1f, 1.0f, "%.3f");

                    ImGui::End();
                }
            }
        }

    };

}
