#pragma once

#include <string>
#include "core/Widget.h"

#include <scene/SceneObject.h>
using namespace ige::scene;

namespace ige::creator
{    
    class Gizmo: public Widget
    {
    public:
        Gizmo();
        virtual ~Gizmo();
        
        void setTarget(const std::shared_ptr<SceneObject>& target);

    protected:
        virtual void _drawImpl() override;

        std::shared_ptr<SceneObject> m_target;
    };
}
