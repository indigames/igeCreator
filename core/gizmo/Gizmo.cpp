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
        if(!m_bEnabled || m_camera == nullptr) return;

        auto target = Editor::getSelectedObject();
        if(!target) return;

        auto transform = target->getTransform();
        if (!transform) return;
        
        // Detect ortho projection
        gizmo::SetOrthographic(m_camera->IsOrthographicProjection());

        Mat4 temp;
        auto view = m_camera->GetViewInverseMatrix(temp).Inverse().P();
        auto proj = m_camera->GetProjectionMatrix(temp).P();
        auto model = (float*)(transform->getWorldMatrix().P());

        ImGui::PushStyleColor(ImGuiCol_Border, 0);
        gizmo::BeginFrame();
        gizmo::Enable(m_bEnabled);

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        ImGuizmo::SetID(target->getId());
        
        float delta[16] = { 0.f };
        gizmo::Manipulate(&view[0], &proj[0], m_operation, m_mode, &model[0], &delta[0]);

        m_bIsUsing = gizmo::IsUsing();
        if (!m_bIsUsing)
        {
            ImGui::PopStyleColor();
            return;
        }

        float deltaTranslation[3], deltaRotation[3], deltaScale[3];
        gizmo::DecomposeMatrixToComponents(delta, deltaTranslation, deltaRotation, deltaScale);

        if(m_operation == gizmo::TRANSLATE)
        {            
            auto dPos = Vec3(deltaTranslation[0], deltaTranslation[1], deltaTranslation[2]);
            transform->setWorldPosition(transform->getWorldPosition() + dPos);
        }
        else if(m_operation == gizmo::SCALE)
        {
            auto dScale = Vec3(deltaScale[0], deltaScale[1], deltaScale[2]);
            transform->setWorldScale(dScale);
        }
        else if(m_operation == gizmo::ROTATE)
        {            
            auto dRotation = Vec3(DEGREES_TO_RADIANS(deltaRotation[0]), DEGREES_TO_RADIANS(deltaRotation[1]), DEGREES_TO_RADIANS(deltaRotation[2]));

            Vec3 eulerRotation;
            vmath_quatToEuler(transform->getWorldRotation().P(), eulerRotation.P());
            eulerRotation = eulerRotation + dRotation;

            Quat quat;
            vmath_eulerToQuat(eulerRotation.P(), quat.P());

            transform->setWorldRotation(quat);
        }
        ImGui::PopStyleColor();
    }
}
