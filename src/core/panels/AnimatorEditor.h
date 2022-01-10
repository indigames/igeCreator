#pragma once

#include "core/Panel.h"

namespace ige::creator
{  
    class AnimatorEditor: public Panel
    {
    public:
        AnimatorEditor(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AnimatorEditor();

        virtual void update(float dt);


    protected:
        virtual void initialize() override;
        virtual void clear();
        virtual void drawWidgets() override;
    };
}
