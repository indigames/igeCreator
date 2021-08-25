#include "core/scene/assets/AssetMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include "core/widgets/Label.h"
#include "core/widgets/CheckBox.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Button.h"
#include "core/widgets/Drag.h"

#include <chrono>
#include <ctime>
#include <iomanip>

NS_IGE_BEGIN

AssetMeta::AssetMeta(const std::string& path)
    : m_path(path) {
    m_options = {};
    loadOptions();
}

bool AssetMeta::loadOptions() {
    auto fsPath = fs::path(m_path);
    auto parentPath = fsPath.parent_path();
    auto metaPath = parentPath.append(fsPath.filename().string() + ".meta");
    auto file = std::ifstream(metaPath);
    if (file.is_open()) {
        try {
            json metaJs;
            file >> metaJs;
            file.close();
            json options = metaJs.value("Option", json{});
            for (auto& [key, val] : options.items()) {
                if (m_options.count(key) != 0) {
                    m_options[key] = val;
                }
            }
        }
        catch (std::exception e) {
            return false;
        }
    }
    return true;
}

void AssetMeta::draw(std::shared_ptr<Group> group) {
    if (group != nullptr) {
        auto fsPath = fs::path(m_path);
        auto metaPath = fsPath.parent_path().append(fsPath.filename().string() + ".meta");
        auto file = std::ifstream(metaPath);
        if (file.is_open()) {
            json metaJs;
            file >> metaJs;
            file.close();

            for (auto& [key, val] : metaJs.items())
            {
                if (val.type() == nlohmann::json::value_t::boolean)
                {
                    group->createWidget<CheckBox>(key, val);
                }
                else if (val.type() == nlohmann::json::value_t::number_float)
                {
                    group->createWidget<Drag<float>>(key, ImGuiDataType_Float, std::array{ (float)val });
                }                    
                else if (val.type() == nlohmann::json::value_t::number_integer 
                    || val.type() == nlohmann::json::value_t::number_unsigned)
                {
                    group->createWidget<TextField>(key, std::to_string(val.get<int64_t>()), false);
                }
                else if (val.type() == nlohmann::json::value_t::string)
                    group->createWidget<TextField>(key, val);
                else {
                    // Todo: parse Vec<N>, Quat,
                }
            }

            if (m_options.size() > 0) {
                auto optionGroup = group->createWidget<Group>("Options");
                for (auto& [key, val] : m_options) {
                    std::string k = key;
                    if (val.type() == nlohmann::json::value_t::boolean) {
                        optionGroup->createWidget<CheckBox>(key, val)->getOnDataChangedEvent().addListener([this, k](auto val) {
                            setOption(k, val);
                        });
                    }
                    else if (val.type() == nlohmann::json::value_t::number_float) {
                        optionGroup->createWidget<Drag<float>>(key, ImGuiDataType_Float, std::array{ (float)val })->getOnDataChangedEvent().addListener([this, k](auto val) {
                            setOption(k, val[0]);
                        });
                    }
                    else if (val.type() == nlohmann::json::value_t::number_integer
                        || val.type() == nlohmann::json::value_t::number_unsigned) {
                        std::array value = { (float)val.get<int>()};
                        optionGroup->createWidget<Drag<float>>(key, ImGuiDataType_S32, value, 1, -1)->getOnDataChangedEvent().addListener([this, k](auto val) {
                            setOption(k, (int)val[0]);
                        });
                    }
                    else if (val.type() == nlohmann::json::value_t::string) {
                        optionGroup->createWidget<TextField>(key, val)->getOnDataChangedEvent().addListener([this, k](auto val) {
                            setOption(k, val);
                        });
                    }
                    else {
                        // Todo: parse Vec<N>, Quat,
                    }
                }

                auto btnSave = group->createWidget<Button>("Save", ImVec2(64.f, 0.f));
                btnSave->getOnClickEvent().addListener([this](auto widget) {
                    save();
                });
            }
        }
    }
}

void AssetMeta::setOption(const std::string& key, json val) {
    m_options[key] = val;
}

AssetMeta::~AssetMeta() {
}

bool AssetMeta::save() {
    auto fsPath = fs::path(m_path);
    if (fs::exists(fsPath)) {
        const auto metaPath = fsPath.parent_path().append(fsPath.filename().string() + ".meta"); 
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(fsPath).time_since_epoch()).count();
        json metaJson = json {
            {"Name", fsPath.filename().string() },
            {"Timestamp", (long long)ms },
            {"Option", m_options}
        };
        std::ofstream file(metaPath);
        file << std::setw(2) << metaJson << std::endl;
        file.close();
        return true;
    }
    return false;
}

NS_IGE_END
