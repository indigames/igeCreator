#include "core/scene/assets/FigureMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include <pyxieFigureExportConfigManager.h>
#include <pyxieColladaLoader.h>
#include <pyxieFbxLoader.h>
#include <pyxieImageConv.h>


NS_IGE_BEGIN

FigureMeta::FigureMeta(const std::string& path)
	: AssetMeta(path) 
{
    m_options = {
        {"BASE_SCALE", 1.f},
        {"EXPORT_NAMES", true},
        {"TRIANGLE_STRIP", true},
        {"GEN_MIPMAP", true},
        {"OPTIMIZE_MESH", false},
        {"OPTIMIZE_VERTEX", true},
        {"OPTIMIZE_ANIMATION", true},
        {"SHADER_MAKE_SHADOW", true},
        {"SHADER_RECEIVE_SHADOW", false},
        {"SHADER_DEPTH_SHADOW", true},
        {"MATERIAL_PARAM_SPARE", 8},
        {"SHADER_NUM_DIR_LAMP", 3},
        {"SHADER_NUM_POINT_LAMP", 7},
        {"SHADER_NUM_SPOT_LAMP", 7},
    };
    loadOptions();
}

FigureMeta::~FigureMeta() {
}


void FigureMeta::draw(std::shared_ptr<Group> group) {
    AssetMeta::draw(group);
}


//! Replace texture path
bool FigureMeta::replaceTextures(EditableFigure& efig)
{
    auto fsPath = fs::path(efig.ResourceName());
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()) : fsPath;
    auto texSrcs = efig.GetTextureSources();
    pyxie::ImageConv imgConv;
    bool alpha = false;

    for (const auto& texSrc : texSrcs) {
        auto texName = fs::path(texSrc.path).filename();
        auto newTexSrc = TextureSource(texSrc);
        for (const auto& entry : fs::recursive_directory_iterator(relPath.parent_path())) {
            if (entry.is_regular_file()) {
                if (entry.path().filename().compare(texName) == 0) {
                    imgConv.SetInputFile(entry.path().string().c_str());
                    alpha = imgConv.DoConvert(true);
                    auto newPath = entry.path().relative_path().replace_extension(".pyxi").string();
                    std::replace(newPath.begin(), newPath.end(), '\\', '/');
                    memset(newTexSrc.path, 0, MAX_PATH);
                    memcpy(newTexSrc.path, newPath.c_str(), newPath.length());
                    break;
                }
            }
        }
        efig.ReplaceTextureSource(texSrc, newTexSrc);
        if(alpha) efig.EnableAlphaModeByTexture(newTexSrc.path);
    }
    return true;
}

bool FigureMeta::loadFbx(EditableFigure& efig, const std::string& path) {
    auto fsPath = fs::path(path);
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()) : fsPath;
    auto parentPath = relPath.parent_path();

    // Load model data and binded animation
    auto parentName = parentPath.filename();
    auto baseModelPath = parentPath;
    baseModelPath.append(parentName.string() + ".fbx");
    bool folderRule = fs::exists(baseModelPath);

    auto filename = fsPath.filename().string();
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

    auto basename = baseModelPath.filename().string();
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);

    if (folderRule && filename.compare(basename) != 0)
        return false;

    pyxieFbxLoader loader;
    auto rv = loader.LoadModel(relPath.c_str(), &efig);
    if (!rv) return false;

    if (folderRule) {
        for (const auto& entry : fs::directory_iterator(parentPath)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (entry.path().filename().compare(baseModelPath.filename()) == 0 || ext.compare(".fbx") != 0)
                    continue;
                auto relPath = entry.path().is_absolute() ? fs::relative(entry.path(), fs::current_path()).string() : entry.path().string();
                std::replace(relPath.begin(), relPath.end(), '\\', '/');
                loader.LoadAnimation(relPath.c_str(), &efig);
            }
        }
    }
    return rv;
}

bool FigureMeta::loadCollada(EditableFigure& efig, const std::string& path) {
    // Setting figure loader options
    pyxieFigureExportConfigManager::Instance().ClearOption();

    auto fsPath = fs::path(path);
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()) : fsPath;
    auto parentPath = relPath.parent_path();

    //! Load options
    for (auto& [key, val] :m_options) {
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

    auto ext = fsPath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    bool isFbx = ext.compare(".fbx") == 0;
    if (isFbx) {
        return loadFbx(efig, path);
    }
    
    // Load model data and binded animation
    auto parentName = parentPath.filename();
    auto baseModelPath = parentPath;
    baseModelPath.append(parentName.string() + ".dae");
    bool folderRule = fs::exists(baseModelPath);

    auto filename = fsPath.filename().string();
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

    auto basename = baseModelPath.filename().string();
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);

    if (folderRule && filename.compare(basename) != 0)
        return false;

    pyxieColladaLoader loader;
    auto rv = loader.LoadCollada(relPath.c_str(), &efig);
    if (!rv) return false;

    if (folderRule) {
        for (const auto& entry : fs::directory_iterator(parentPath)) {
            if (entry.is_regular_file()) {
                auto ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (entry.path().filename().compare(baseModelPath.filename()) == 0 || ext.compare(".dae") != 0)
                    continue;
                auto relPath = entry.path().is_absolute() ? fs::relative(entry.path(), fs::current_path()).string() : entry.path().string();
                std::replace(relPath.begin(), relPath.end(), '\\', '/');
                loader.LoadColladaAnimation(relPath.c_str(), &efig);
            }
        }
    }
    return rv;
}

bool FigureMeta::save() {
    auto fsPath = fs::path(m_path);
    auto efig = ResourceCreator::Instance().NewEditableFigure(m_path.c_str(), true);
    if (loadCollada(*efig, m_path)) {
        replaceTextures(*efig);
        if (m_options["OPTIMIZE_VERTEX"]) {
            efig->RemoveUnreferencedVertices();
        }
        if (m_options["OPTIMIZE_MESH"]) {
            efig->MergeSameMaterialMesh();
        }
        efig->SaveFigure((fsPath.parent_path().append(fsPath.stem().string() + ".pyxf")).c_str());
    }
    efig->DecReference();
    efig = nullptr;
    return AssetMeta::save();
}


NS_IGE_END