#include "core/gizmo/Gizmo.h"
#include "core/Editor.h"

#include <components/TransformComponent.h>
#include <components/gui/RectTransform.h>

namespace ige::creator
{
    Gizmo::Gizmo()
        : Widget(true, false), m_camera(nullptr)
    {
        m_operation = gizmo::OPERATION::TRANSLATE;
        m_mode = gizmo::MODE::LOCAL;
        m_visible = true;
    }

    Gizmo::~Gizmo()
    {
        m_camera = nullptr;
    }

    void Gizmo::setCamera(Camera* cam)
    {
        m_camera = cam;
    }

    Camera* Gizmo::getCamera()
    {
        return m_camera;
    }

    void Gizmo::setMode(gizmo::MODE value)
    {
        m_mode = value;
    }

    gizmo::MODE Gizmo::getMode() const
    {
        return m_mode;
    }

    void Gizmo::setOperation(gizmo::OPERATION value)
    {
        m_operation = value;
    }

    gizmo::OPERATION Gizmo::getOperation() const
    {
        return m_operation;
    }

    void Gizmo::_drawImpl()
    {
        if(!m_bEnabled || m_camera == nullptr || !m_visible) return;

        auto target = Editor::getInstance()->getSelectedObject();
        if(!target) return;

        auto transform = target->getTransform();
        if (!transform) return;
        
        // Detect ortho projection
        gizmo::SetOrthographic(m_camera->IsOrthographicProjection());

        Mat4 viewInvMat;
        auto view = m_camera->GetViewInverseMatrix(viewInvMat).Inverse().P();

        Mat4 projMat;
        auto proj = m_camera->GetProjectionMatrix(projMat).P();

        Mat4 modelMat = transform->getWorldMatrix();
        auto model = modelMat.P();

        ImGui::PushStyleColor(ImGuiCol_Border, 0);
        gizmo::BeginFrame();
        gizmo::Enable(m_bEnabled);

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 25.f, ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 25.f);
        ImGuizmo::SetID(target->getId());

        float delta[16] = { 0.f };
        gizmo::Manipulate(&view[0], &proj[0], m_operation, m_mode, &model[0], &delta[0]);
        
        // Show view manipulation in 3D mode only
        if (!m_camera->IsOrthographicProjection())
        {
            ImVec4 newEye, newTarget;
            if (gizmo::ViewManipulate(&view[0], 8.f, ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x10101010, newEye, newTarget))
            {
                m_camera->SetCameraPosition({ newEye.x, newEye.y, newEye.z });
                m_camera->SetTarget({ newTarget.x, newTarget.y, newTarget.z });
            }
        }

        m_bIsUsing = gizmo::IsUsing();
        if (!m_bIsUsing)
        {
            ImGui::PopStyleColor();
            return;
        }

        if(m_operation == gizmo::TRANSLATE)
        {
            float deltaTranslation[3], deltaRotation[3], deltaScale[3];
            gizmo::DecomposeMatrixToComponents(delta, deltaTranslation, deltaRotation, deltaScale);
            if (isnan(deltaTranslation[0]) || isnan(deltaTranslation[1]) || isnan(deltaTranslation[2])) return;
            transform->worldTranslate(Vec3(deltaTranslation[0], deltaTranslation[1], deltaTranslation[2]));
        }
        else if(m_operation == gizmo::ROTATE)
        {            
            Vec3 col0(delta[0], delta[4], delta[8]);
            Vec3 col1(delta[1], delta[5], delta[9]);
            Vec3 col2(delta[2], delta[6], delta[10]);
            Vec3 scale(col0.Length(), col1.Length(), col2.Length());
            Mat3 rotmat(col0 / scale.X(), col1 / scale.Y(), col2 / scale.Z());
            transform->worldRotate(Quat(rotmat));
        }
        else if (m_operation == gizmo::SCALE)
        {
            float deltaTranslation[3], deltaRotation[3], deltaScale[3];
            gizmo::DecomposeMatrixToComponents(delta, deltaTranslation, deltaRotation, deltaScale);
            if (isnan(deltaScale[0]) || isnan(deltaScale[1]) || isnan(deltaScale[2])) return;
            transform->worldScale(Vec3(deltaScale[0], deltaScale[1], deltaScale[2]));
        }
        ImGui::PopStyleColor();
    }
}
