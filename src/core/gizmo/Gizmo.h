#pragma once

#include <string>
#include <map>

#include "core/Widget.h"

#include <utils/PyxieHeaders.h>
using namespace pyxie;

#include <scene/SceneObject.h>
using namespace ige::scene;

#include <imgui.h>
#include <ImGuizmo.h>
namespace gizmo = ImGuizmo;

namespace ige::creator
{    
    class Gizmo: public Widget
    {
    public:
        Gizmo();
        virtual ~Gizmo();
        
        //! Set camera
        void setCamera(Camera* cam);

        //! Get camera
        Camera* getCamera();

        //! Set gizmo mode
        void setMode(gizmo::MODE value);

        //! Get gizmo mode
        gizmo::MODE getMode() const;

        //! Visibility
        void setVisible(bool value) { m_visible = value; }
        bool isVisible() { return m_visible; }

        //! Set gizmo operation
        void setOperation(gizmo::OPERATION value);

        //! Get gizmo operation
        gizmo::OPERATION getOperation() const;

        //! Update target node
        void updateTargetNode(bool reset = true);

        //! Get position
        const Vec3& getPosition() const;

        //! Get scale
        const Vec3& getScale() const;

        //! Get rotation
        const Quat& getRotation() const;

        //! Target object
        void onTargetAdded(SceneObject* object);
        void onTargetRemoved(SceneObject* object);
        void onTargetCleared();

    protected:
        virtual void _drawImpl() override;
        
        //! Translate
        void translate(const Vec3& trans);

        //! Rotate
        void rotate(const Quat& rot);

        //! Scale
        void scale(const Vec3& scale);

        Camera* m_camera;
        
        gizmo::OPERATION m_operation;
        gizmo::MODE m_mode;

        bool m_visible = false;
        bool m_bIsUsing = false;

        Vec3 m_position = {};
        Vec3 m_scale = {1.f, 1.f, 1.f};
        Quat m_rotation = {};

        std::map<uint64_t, Vec3> m_initScales;
        std::map<uint64_t, Vec3> m_initPositions;

        uint64_t m_targetAddedEventId;
        uint64_t m_targetRemovedEventId;
        uint64_t m_targetClearedEventId;
    };
}
