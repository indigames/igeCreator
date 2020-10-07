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

        const std::string& getStartScene() const { return m_startScene;  }
        void setStartScene(const std::string& scene);

        const Vec3& getGravity() const { return m_gravity; }
        void setGravity(const Vec3& gravity) { m_gravity = gravity; };

        float getGlobalVolume() const { return m_globalVolume; }
        void setGlobalVolume(float vol) { m_globalVolume = vol; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        void saveSettings();

    protected:
        //! Scene settings
        std::string m_startScene;

        //! Physic settings
        Vec3 m_gravity;

        //! Audio settings
        float m_globalVolume;
    };
}
