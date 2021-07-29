#include "core/scene/assets/FigureMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include <pyxieFigureExportConfigManager.h>
#include <pyxieColladaLoader.h>

#include <chrono>
#include <ctime>
#include <iomanip>

NS_IGE_BEGIN

FigureMeta::FigureMeta(const std::string& path)
	: AssetMeta(path) {
}

FigureMeta::~FigureMeta() {
}


void FigureMeta::draw(std::shared_ptr<Group> group) {
    AssetMeta::draw(group);
}

bool FigureMeta::loadCollada(EditableFigure& efig, const std::string& path) {
    // Setting figure loader options
    pyxieFigureExportConfigManager::Instance().ClearOption();

    auto fsPath = fs::path(path);
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()).string() : fsPath.string();
    std::replace(relPath.begin(), relPath.end(), '\\', '/');

    auto parentPath = fsPath.parent_path();
    auto metaPath = parentPath.append(fsPath.filename().string() + ".meta");
    auto file = std::ifstream(metaPath);
    if (file.is_open()) {
        json metaJs;
        file >> metaJs;
        file.close();
        json options = metaJs.value("options", json{});
        for (auto& [key, val] : options.items()) {
            if (val.type() == nlohmann::json::value_t::boolean
                || val.type() == nlohmann::json::value_t::number_integer
                || val.type() == nlohmann::json::value_t::number_unsigned) {
                pyxieFigureExportConfigManager::Instance().SetOptionInt(key.c_str(), val);
            } else if (val.type() == nlohmann::json::value_t::number_float) {
                pyxieFigureExportConfigManager::Instance().SetOptionFloat(key.c_str(), val);
            } else if (val.type() == nlohmann::json::value_t::string) {
                pyxieFigureExportConfigManager::Instance().SetOptionString(key.c_str(), val.get<std::string>().c_str());
            } else {
                // Just ignore
            }
        }
    }

    // Load model data and binded animation
    pyxieColladaLoader loader;
    auto rv = loader.LoadCollada(relPath.c_str(), &efig);
    if (!rv) return false;

    // Load additional animations
    bool folderRule = fsPath.filename().compare(parentPath.filename()) == 0;
    if (folderRule) {
        for (const auto& entry : fs::directory_iterator(parentPath)) {
            if (entry.is_regular_file()) {
                if (entry.path().filename().compare(parentPath.filename()))
                    continue;
                auto relPath = entry.path().is_absolute() ? fs::relative(entry.path(), fs::current_path()).string() : entry.path().string();
                std::replace(relPath.begin(), relPath.end(), '\\', '/');
                rv = loader.LoadColladaAnimation(relPath.c_str(), &efig);
                if (!rv) return false;
            }
        }
    }
    return rv;
}

bool FigureMeta::save() {
    auto fsPath = fs::path(m_path);
    auto efig = ResourceCreator::Instance().NewEditableFigure(m_path.c_str(), true);
    if (loadCollada(*efig, m_path))
        efig->SaveFigure((fsPath.parent_path().append(fsPath.filename().string() + ".pyxf")).c_str());
    efig->DecReference();
    efig = nullptr;

    if (fs::exists(fsPath)) {
        const auto metaPath = fsPath.parent_path().append(fsPath.filename().string() + ".meta");
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(fsPath).time_since_epoch()).count();

        json metaJson = json{
            {"timestamp", ms },
            {"options", {}}
        };
        std::ofstream file(metaPath);
        file << std::setw(2) << metaJson << std::endl;
        file.close();
        return true;
    }
    return false;
}


NS_IGE_END