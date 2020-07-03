#pragma once

#include <string>
#include "core/Widget.h"

#include <utils/PyxieHeaders.h>
using namespace pyxie;

#include <scene/SceneObject.h>
using namespace ige::scene;

#include <imgui.h>
#include "3rd-party/ImGuizmo/ImGuizmo.h"
namespace gizmo = ImGuizmo;

namespace ige::creator
{    
    class Gizmo: public Widget
    {
    public:
        Gizmo();
        virtual ~Gizmo();
        
        /**
         * Set target
         */
        void setTarget(const std::shared_ptr<SceneObject>& target);

        /**
         * Get target
         */
        std::shared_ptr<SceneObject>& getTarget();

        /**
         * Set camera
         */
        void setCamera(Camera* cam);

        /**
         * Get camera
         */
        Camera* getCamera();

        /**
         * Set gizmo mode
         */
        void setMode(gizmo::MODE value);

        /**
         * Get gizmo mode
         */
        gizmo::MODE getMode() const;


        /**
         * Set gizmo operation
         */
        void setOperation(gizmo::OPERATION value);

        /**
         * Get gizmo operation
         */
        gizmo::OPERATION getOperation() const;

    protected:
        virtual void _drawImpl() override;

        std::shared_ptr<SceneObject> m_target;
        Camera* m_camera;
        
        gizmo::OPERATION m_operation;
        gizmo::MODE m_mode;

        bool m_bIsUsing = false;
    };
}
