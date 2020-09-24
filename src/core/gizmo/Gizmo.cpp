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

        Mat4 modelMat = (m_mode == gizmo::MODE::LOCAL ? transform->getLocalMatrix() : transform->getWorldMatrix());
        auto model = modelMat.P();

        ImGui::PushStyleColor(ImGuiCol_Border, 0);
        gizmo::BeginFrame();
        gizmo::Enable(m_bEnabled);

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 25.f, ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 25.f);
        ImGuizmo::SetID(target->getId());

        float delta[16] = { 0.f };
        gizmo::Manipulate(&view[0], &proj[0], m_operation, m_mode, &model[0], &delta[0]);

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

            auto dPos = Vec3(deltaTranslation[0], deltaTranslation[1], deltaTranslation[2]);
            if(m_mode == gizmo::MODE::LOCAL)
                transform->setPosition(transform->getPosition() + dPos);
            else
                transform->setWorldPosition(transform->getWorldPosition() + dPos);
        }
        else if(m_operation == gizmo::SCALE)
        {
            float deltaTranslation[3], deltaRotation[3], deltaScale[3];
            gizmo::DecomposeMatrixToComponents(delta, deltaTranslation, deltaRotation, deltaScale);
            auto scale = Vec3(deltaScale[0], deltaScale[1], deltaScale[2]);
            if (m_mode == gizmo::MODE::LOCAL)
                transform->scale(scale);
            else
                transform->scale(scale);
        }
        else if(m_operation == gizmo::ROTATE)
        {            
            Vec3 col0(delta[0], delta[4], delta[8]);
            Vec3 col1(delta[1], delta[5], delta[9]);
            Vec3 col2(delta[2], delta[6], delta[10]);
            Vec3 scale(col0.Length(), col1.Length(), col2.Length());

            Mat3 rotmat(col0 / scale.X(), col1 / scale.Y(), col2 / scale.Z());
            Quat dQuatRotation = Quat(rotmat);

            if (m_mode == gizmo::MODE::LOCAL)
                transform->setRotation(transform->getRotation() * dQuatRotation);
            else
                transform->setWorldRotation(transform->getWorldRotation() * dQuatRotation);
        }
        ImGui::PopStyleColor();
    }
}
