#include "core/scene/assets/AssetMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include "core/widgets/Label.h"
#include "core/widgets/CheckBox.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Drag.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <chrono>
#include <ctime>
#include <iomanip>

NS_IGE_BEGIN

AssetMeta::AssetMeta(const std::string& path)
    : m_path(path) {
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
                if (val.type() == nlohmann::json::value_t::array)
                {

                }
                else if (val.type() == nlohmann::json::value_t::boolean)
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
        }
    }
}

AssetMeta::~AssetMeta() {
}

bool AssetMeta::save() {
    auto fsPath = fs::path(m_path);
    if (fs::exists(fsPath)) {
        const auto metaPath = fsPath.parent_path().append(fsPath.filename().string() + ".meta"); 
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(fsPath).time_since_epoch()).count();

        json metaJson = json {
            {"timestamp", (long long)ms },
        };
        std::ofstream file(metaPath);
        file << std::setw(2) << metaJson << std::endl;
        file.close();
        return true;
    }
    return false;
}

NS_IGE_END
