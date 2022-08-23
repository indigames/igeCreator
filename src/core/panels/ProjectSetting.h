#pragma once

#include "core/Panel.h"

#include <utils/PyxieHeaders.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ige::creator
{
    class PlatformSetting {
    public:
        std::string platform;
        std::string romDir;
        std::string configDir;
        std::vector<std::string> archs;

        //! Serialize
        virtual void to_json(json& j) const {
            j["Platform"] = platform;
            j["RomDir"] = romDir;
            j["ConfigDir"] = configDir;
            j["Archs"] = archs;
        }

        //! Deserialize
        virtual void from_json(const json& j) {
            platform = j.value("Platform", "windows");
            romDir = j.value("RomDir", "mobile");
            configDir = j.value("ConfigDir", "");
            archs = j.value("Archs", std::vector<std::string>());
        }

        //! Serialize
        friend void to_json(json& j, const PlatformSetting& obj);

        //! Deserialize
        friend void from_json(const json& j, PlatformSetting& obj);
    };

    class AndroidSetting : public PlatformSetting {
    public:
        int minSdk;
        int targetSdk;
        std::vector<std::string> permissions;
        std::map<std::string, bool> features;

        //! Serialize
        virtual void to_json(json& j) const override {
            PlatformSetting::to_json(j);
            j["MinSdkVersion"] = minSdk;
            j["TargetSdkVersion"] = targetSdk;
            j["Permissions"] = permissions;
            j["Features"] = features;
        }

        //! Deserialize
        virtual void from_json(const json& j) override {
            PlatformSetting::from_json(j);
            minSdk = j.value("MinSdkVersion", 21);
            targetSdk = j.value("TargetSdkVersion", 31);
            permissions = j.value("Permissions", std::vector<std::string>());
            features = j.value("Features", std::map<std::string, bool>());
        }
    };

    class IOSSetting : public PlatformSetting {
    public:
        std::string deployTarget;
        std::string deviceFamily;
        std::string developerTeamId;
        std::string codeSignIdentity;
        std::string provisionProfile;

        //! Serialize
        virtual void to_json(json& j) const override {
            PlatformSetting::to_json(j);
            j["DeploymentTarget"] = deployTarget;
            j["DeviceFamily"] = deviceFamily;
            j["DevelopmentTeamID"] = developerTeamId;
            j["CodeSignIdentity"] = codeSignIdentity;
            j["ProvisioningProfile"] = provisionProfile;
        }

        //! Deserialize
        virtual void from_json(const json& j) override {
            PlatformSetting::from_json(j);
            deployTarget = j.value("DeploymentTarget", "11.0");
            deviceFamily = j.value("DeviceFamily", "1,2");
            developerTeamId = j.value("DevelopmentTeamID", "None");
            codeSignIdentity = j.value("CodeSignIdentity", "iPhone Development");
            provisionProfile = j.value("ProvisioningProfile", "Automatic");
        }
    };

    class ProjectSetting: public Panel
    {
    public:
        ProjectSetting(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~ProjectSetting();

        virtual void clear();
        virtual void initialize() override;

    protected:
        virtual void _drawImpl() override;

        //! Draw settings
        void drawSettings();

        //! Save settings
        void saveSettings();

        //! Redraw
        void redraw() { m_bNeedRedraw = true; }

        //! Serialize
        friend void to_json(json& j, const ProjectSetting& obj);

        //! Deserialize
        friend void from_json(const json& j, ProjectSetting& obj);

    protected:
        std::string appName;
        std::string appLabel;
        std::string versionName;
        int versionCode = 0;
        std::string bundleId;
        std::string orientation;
        std::string startScene;
        std::map<std::string, bool> dependencies;
        std::vector<std::shared_ptr<PlatformSetting>> platforms;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
    };
}
