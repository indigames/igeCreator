#include "core/gizmo/Gizmo.h"
#include "core/Editor.h"
#include "core/scene/TargetObject.h"

#include <components/TransformComponent.h>
#include <components/gui/RectTransform.h>
#include <scene/Scene.h>
#include <scene/SceneObject.h>

namespace ige::creator
{
    Gizmo::Gizmo()
        : Widget(true, false), m_camera(nullptr)
    {
        m_operation = gizmo::OPERATION::TRANSLATE;
        m_mode = gizmo::MODE::LOCAL;
        m_visible = true;

        m_targetAddedEventId = Editor::getTargetAddedEvent().addListener(std::bind(&Gizmo::onTargetAdded, this, std::placeholders::_1));
        m_targetRemovedEventId = Editor::getTargetRemovedEvent().addListener(std::bind(&Gizmo::onTargetRemoved, this, std::placeholders::_1));
        m_targetClearedEventId = Editor::getTargetClearedEvent().addListener(std::bind(&Gizmo::onTargetCleared, this));
    }

    Gizmo::~Gizmo()
    {
        m_targets.clear();
        m_camera = nullptr;
        onTargetCleared();

        Editor::getTargetAddedEvent().removeListener(m_targetAddedEventId);
        Editor::getTargetRemovedEvent().removeListener(m_targetRemovedEventId);
        Editor::getTargetClearedEvent().removeListener(m_targetClearedEventId);
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
        if(!m_bEnabled || m_camera == nullptr || !m_visible || Editor::getCurrentScene() == nullptr)
            return;

        auto target = Editor::getInstance()->getTarget();
        if (target == nullptr || target->empty())
            return;
        
        // Detect ortho projection
        gizmo::SetOrthographic(m_camera->IsOrthographicProjection());

        Mat4 viewInvMat;
        auto view = m_camera->GetViewInverseMatrix(viewInvMat).Inverse().P();

        Mat4 projMat;
        auto proj = m_camera->GetProjectionMatrix(projMat).P();

        auto modelMat = Mat4::IdentityMat();
        vmath_mat4_from_rottrans(m_rotation.P(), m_position.P(), modelMat.P());
        vmath_mat_appendScale(modelMat.P(), m_scale.P(), 4, 4, modelMat.P());
        auto model = modelMat.P();

        ImGui::PushStyleColor(ImGuiCol_Border, 0);
        gizmo::BeginFrame();
        gizmo::Enable(m_bEnabled);

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 25.f, ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 25.f);

        float delta[16] = { 0.f };
        gizmo::Manipulate(&view[0], &proj[0], m_operation, m_mode, &model[0], &delta[0]);
        
        bool isManipulating = false;

        // Show view manipulation in 3D mode only
        if (!m_camera->IsOrthographicProjection())
        {
            ImVec4 newEye, newTarget;
            isManipulating = gizmo::ViewManipulate(&view[0], 8.f, ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x10101010, newEye, newTarget);
            if (isManipulating)
            {
                m_camera->SetCameraPosition({ newEye.x, newEye.y, newEye.z });
                m_camera->SetTarget({ newTarget.x, newTarget.y, newTarget.z });
            }
        }

        if(m_bIsUsing && !gizmo::IsUsing())
            updateTargetNode();

        m_bIsUsing = gizmo::IsUsing() || isManipulating;
        if (!gizmo::IsUsing())
        {
            ImGui::PopStyleColor();
            return;
        }

        if(m_operation == gizmo::TRANSLATE)
        {
            float deltaTranslation[3], deltaRotation[3], deltaScale[3];
            gizmo::DecomposeMatrixToComponents(delta, deltaTranslation, deltaRotation, deltaScale);
            if (isnan(deltaTranslation[0]) || isnan(deltaTranslation[1]) || isnan(deltaTranslation[2])) return;
            translate(Vec3(deltaTranslation[0], deltaTranslation[1], deltaTranslation[2]));
        }
        else if(m_operation == gizmo::ROTATE)
        {
            Vec3 col0(delta[0], delta[4], delta[8]);
            Vec3 col1(delta[1], delta[5], delta[9]);
            Vec3 col2(delta[2], delta[6], delta[10]);
            Vec3 scale(col0.Length(), col1.Length(), col2.Length());
            Mat3 rotmat(col0 / scale.X(), col1 / scale.Y(), col2 / scale.Z());
            m_rotation = Quat(rotmat);
            rotate(m_rotation);
        }
        else if (m_operation == gizmo::SCALE)
        {
            float deltaTranslation[3], deltaRotation[3], deltaScale[3];
            gizmo::DecomposeMatrixToComponents(delta, deltaTranslation, deltaRotation, deltaScale);
            if (isnan(deltaScale[0]) || isnan(deltaScale[1]) || isnan(deltaScale[2])) return;
            scale(Vec3(deltaScale[0], deltaScale[1], deltaScale[2]));
        }
        ImGui::PopStyleColor();
    }

    //! Target events
    void Gizmo::onTargetAdded(const std::shared_ptr<SceneObject>& object)
    {
        updateTargetNode();
    }

    void Gizmo::onTargetRemoved(const std::shared_ptr<SceneObject>& object)
    {
        updateTargetNode();
    }

    void Gizmo::onTargetCleared()
    {
        m_position = { 0.f, 0.f, 0.f };
        m_scale = { 1.f, 1.f, 1.f };
        m_rotation = {};
        m_initScales.clear();
        m_initPositions.clear();
    }

    // Update targets
    void Gizmo::updateTargetNode(bool reset)
    {
        if (Editor::getCurrentScene() == nullptr) return;

        if (reset)
        {
            m_position = { 0.f, 0.f, 0.f };
            m_scale = { 1.f, 1.f, 1.f };
            m_rotation = {};
        }

        m_initScales.clear();
        m_initPositions.clear();

        updateTargets();

        for (const auto& target : m_targets)
        {
            if (!target.expired())
            {
                auto targetObj = target.lock();
                auto transform = targetObj->getTransform();
                m_position += transform->getWorldPosition();
                m_initScales[targetObj->getId()] = transform->getWorldScale();
                m_initPositions[targetObj->getId()] = transform->getWorldPosition();
            }
        }
        m_position /= m_targets.size();
        
        if(m_targets.size() == 1)
        {
            auto target = m_targets.at(0);
            if (!target.expired())
            {
                auto transform = target.lock()->getTransform();
                m_rotation = transform->getWorldRotation();
            }
        }
    }

    void Gizmo::updateTargets()
    {
        m_targets.clear();
        m_targets = Editor::getInstance()->getTarget()->getAllTargets();

        auto it = m_targets.begin();
        while (it != m_targets.end())
        {
            if (!(*it).expired())
            {
                auto parent = (*it).lock()->getParent();
                if (parent != nullptr)
                {
                    auto itr = std::find_if(m_targets.begin(), m_targets.end(), [parent](auto& elem) {
                        return !elem.expired() && elem.lock()->getId() == parent->getId();
                    });
                    if (itr != m_targets.end())
                    {
                        removeAllChildren((*itr).lock());
                    }
                }
                else
                {
                    removeAllChildren((*it).lock());
                }
            }
            if (it != m_targets.end()) it++;
        }
    }

    void Gizmo::removeAllChildren(std::shared_ptr<SceneObject> obj)
    {
        if (obj != nullptr)
        {
            for (const auto& child : obj->getChildren())
            {
                auto itr = std::find_if(m_targets.begin(), m_targets.end(), [child](auto& elem) {
                    return !elem.expired() && elem.lock()->getId() == child.lock()->getId();
                });
                if (itr != m_targets.end())
                {
                    m_targets.erase(itr);
                    removeAllChildren(itr->lock());
                }
            }
        }
    }

    //! Get position
    const Vec3& Gizmo::getPosition() const
    {
        return m_position;
    }


    //! Get scale
    const Vec3& Gizmo::getScale() const
    {
        return m_scale;
    }

    //! Get rotation
    const Quat& Gizmo::getRotation() const
    {
        return m_rotation;
    }

    //! Translate
    void Gizmo::translate(const Vec3& trans)
    {
        for (auto& target : m_targets)
            if (!target.expired()) target.lock()->getTransform()->worldTranslate(trans);
        updateTargetNode();
    }

    //! Rotate
    void Gizmo::rotate(const Quat& rot)
    {
        for (auto& target : m_targets)
            if (!target.expired()) target.lock()->getTransform()->worldRotate(rot);
    }

    float Quat_Norm(const Quat& rotation)
    {
        return std::sqrt(rotation.X() * rotation.X() +
            rotation.Y() * rotation.Y() +
            rotation.Z() * rotation.Z() +
            rotation.W() * rotation.W());
    }

    Quat Quat_Conjugate(const Quat& rotation)
    {
        return Quat(-rotation.X(), -rotation.Y(), -rotation.Z(), rotation.W());
    }

    Quat Quat_Inverse(const Quat& rotation)
    {
        float n = Quat_Norm(rotation);
        auto invQuat = Quat_Conjugate(rotation);
        invQuat.X(invQuat.X() / (n * n));
        invQuat.Y(invQuat.Y() / (n * n));
        invQuat.Z(invQuat.Z() / (n * n));
        invQuat.W(invQuat.W() / (n * n));
        return invQuat;
    }

    Vec3 Vec3_Mul(const Vec3& v1, const Vec3& v2)
    {
        return Vec3(v1.X() * v2.X(), v1.Y() * v2.Y(), v1.Z() * v2.Z());
    }

    //! Scale
    void Gizmo::scale(const Vec3& scale)
    {
        for (auto& targetObj : m_targets)
        {
            if (!targetObj.expired())
            {
                auto target = targetObj.lock();
                auto transform = target->getTransform();
                transform->setWorldScale(Vec3_Mul(scale, m_initScales[target->getId()]));

                if (m_targets.size() > 1)
                {
                    auto deltaPos = m_initPositions[target->getId()] - m_position;
                    deltaPos = Quat_Inverse(m_rotation) * Vec3_Mul(scale - Vec3(1.f, 1.f, 1.f), deltaPos);
                    auto newPosition = m_initPositions[target->getId()] + deltaPos;
                    transform->setWorldPosition(newPosition);
                }
            }
        }
    }
}
