#pragma once

#include "core/Panel.h"

#include <utils/PyxieHeaders.h>

namespace ige::creator
{
    class ProjectSetting: public Panel
    {
    public:
        ProjectSetting(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~ProjectSetting();

        virtual void clear();
        virtual void initialize() override;

        const std::string& getStartScene() const { return m_startScene;  }
        void setStartScene(const std::string& scene);

    protected:
        virtual void _drawImpl() override;

        //! Save settings
        void saveSettings();

        //! Redraw
        void redraw() { m_bNeedRedraw = true; }

    protected:
        //! Scene settings
        std::string m_startScene;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
    };
}
