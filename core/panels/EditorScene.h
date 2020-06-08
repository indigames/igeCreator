#pragma once

#include "core/Panel.h"

namespace ige::creator
{
    class EditorScene: public Panel
    {
    public:
        EditorScene(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~EditorScene();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
    };
}
