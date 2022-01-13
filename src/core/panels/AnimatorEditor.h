#pragma once

#include "core/Panel.h"

#include <components/animation/AnimatorController.h>
using namespace ige::scene;

namespace ige::creator
{
    class AnimatorEditor: public Panel
    {
    public:
        AnimatorEditor(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AnimatorEditor();

        void openAnimator(const std::string& path);

    protected:
        virtual void initialize() override;
        virtual void clear();
        virtual void drawWidgets() override;

    protected:
        std::string m_path;
        std::shared_ptr<AnimatorController> m_controller = nullptr;
        std::shared_ptr<IPlugin> m_dragDropPlugin = nullptr;
        int m_uniqueId;
    };
}
