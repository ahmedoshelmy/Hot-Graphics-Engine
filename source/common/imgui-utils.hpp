#include<ImGuizmo.h>
#include<imgui.h>
#include<imconfig.h>
#include "components/camera.hpp"
#include "ecs/entity.hpp"
#include <glm/gtc/quaternion.hpp>       // For glm::quat
#include <glm/gtx/matrix_decompose.hpp>      // For glm::decompose
#include <glm/glm.hpp>


namespace imgui_utils {
    void EditTransform(const our::CameraComponent* camera, our::Entity* entity, glm::vec3 geom_origin = glm::vec3(0.0f))
    {
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
        if (ImGui::IsKeyPressed(84)) // t key
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(82)) // r key
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(59)) // y Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
       

        
        float matrix[16];
        glm::vec3 origin =  entity->localTransform.scale * geom_origin;
        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(entity->localTransform.position + origin), glm::value_ptr(entity->localTransform.rotation), glm::value_ptr(entity->localTransform.scale), matrix);
        ImGui::InputFloat3(("Tr##"+entity->name).c_str(), glm::value_ptr(entity->localTransform.position), 3);
        ImGui::InputFloat3(("Rt##"+entity->name).c_str(), glm::value_ptr(entity->localTransform.rotation), 3);
        ImGui::InputFloat3(("Sc##"+entity->name).c_str(), glm::value_ptr(entity->localTransform.scale), 3);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        // static bool useSnap(false);
        // if (ImGui::IsKeyPressed(83))
        //     useSnap = !useSnap;
        // ImGui::Checkbox("", &useSnap);
        // ImGui::SameLine();
        // vec_t snap;
        // switch (mCurrentGizmoOperation)
        // {
        // case ImGuizmo::TRANSLATE:
        //     snap = config.mSnapTranslation;
        //     ImGui::InputFloat3("Snap", &snap.x);
        //     break;
        // case ImGuizmo::ROTATE:
        //     snap = config.mSnapRotation;
        //     ImGui::InputFloat("Angle Snap", &snap.x);
        //     break;
        // case ImGuizmo::SCALE:
        //     snap = config.mSnapScale;
        //     ImGui::InputFloat("Scale Snap", &snap.x);
        //     break;
        // }
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
        ImGuizmo::Manipulate(glm::value_ptr(camera->getViewMatrix()), glm::value_ptr(camera->getProjectionMatrix(glm::ivec2(1920, 1080))), mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, NULL);

        if(ImGuizmo::IsUsing()) {
            float translation[3], rotation[3], scale[3];
            ImGuizmo::DecomposeMatrixToComponents(matrix, translation, rotation, scale);

            if(mCurrentGizmoOperation == ImGuizmo::SCALE) {
                entity->localTransform.scale = glm::make_vec3(scale);
            }
            if(mCurrentGizmoOperation == ImGuizmo::ROTATE) {
                entity->localTransform.rotation = glm::make_vec3(rotation);
            }
            if(mCurrentGizmoOperation == ImGuizmo::TRANSLATE) {
                entity->localTransform.position = glm::make_vec3(translation) - origin ;
            }
        }

    }

}
