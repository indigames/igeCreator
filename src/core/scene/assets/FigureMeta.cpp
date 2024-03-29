#include "core/scene/assets/FigureMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"
#include "core/Editor.h"

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
        {"OPTIMIZE_MESH", true},
        {"OPTIMIZE_VERTEX", true},
        {"OPTIMIZE_ANIMATION", true},
        {"SHADER_MAKE_SHADOW", false},
        {"SHADER_RECEIVE_SHADOW", false},
        {"SHADER_DEPTH_SHADOW", true},
        {"SHADER_VERTEX_COLOR", false},
        {"SHADER_NUM_DIR_LAMP", 3},
        {"SHADER_NUM_POINT_LAMP", 7},
        {"SHADER_NUM_SPOT_LAMP", 7},
        {"EMBEDDED_ANIMATION", false},
    };

    auto ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext.compare(".fbx") == 0)
        m_options["BASE_SCALE"] = 100.f;

    loadOptions();
}

FigureMeta::~FigureMeta() {
}

void FigureMeta::draw(std::shared_ptr<Group> group) {
    AssetMeta::draw(group);
}

bool FigureMeta::isFolderRule() {
    auto fsPath = fs::path(m_path);
    auto parentPath = fsPath.parent_path();

    auto ext = fsPath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    bool isFbx = ext.compare(".fbx") == 0;

    // Load model data and binded animation
    auto parentName = parentPath.filename();
    auto baseModelPath = parentPath;
    if (isFbx)
        baseModelPath.append(parentName.string() + ".fbx");
    else
        baseModelPath.append(parentName.string() + ".dae");
    return fs::exists(baseModelPath);
}

bool FigureMeta::isBaseModel() {
    if(!isFolderRule()) return true;
    auto fsPath = fs::path(m_path);

    auto filename = fsPath.filename().stem().string();
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

    auto basename = fsPath.parent_path().filename().string();
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);

    return (filename.compare(basename) == 0);
}

std::string FigureMeta::baseModelPath()
{
    if(!isFolderRule()) return m_path;
    auto fsPath = fs::path(m_path);
    auto parentPath = fsPath.parent_path();

    auto ext = fsPath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    bool isFbx = ext.compare(".fbx") == 0;

    // Load model data and binded animation
    auto parentName = parentPath.filename();
    auto baseModelPath = parentPath;
    if (isFbx)
        baseModelPath.append(parentName.string() + ".fbx");
    else
        baseModelPath.append(parentName.string() + ".dae");
    return fs::exists(baseModelPath) ? baseModelPath.string() : m_path;
}

bool FigureMeta::isAnim() 
{
    bool embedded = std::make_unique<FigureMeta>(baseModelPath())->getOption("EMBEDDED_ANIMATION").get<bool>();
    if (!embedded && (!isFolderRule() || (isFolderRule() && !isBaseModel())))
        return true;
    return false;
}


//! Replace texture path
bool FigureMeta::replaceTextures(EditableFigure& efig)
{
    auto fsPath = fs::path(efig.ResourceName());
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()) : fsPath;
    auto texSrcs = efig.GetTextureSources();
    pyxie::ImageConv imgConv;

    for (const auto& texSrc : texSrcs) {
        auto texName = fs::path(texSrc.path).filename();
        for (const auto& entry : fs::recursive_directory_iterator(relPath.parent_path())) {
            if (entry.is_regular_file()) {
                if (entry.path().filename().compare(texName) == 0) {
                    // Check for alpha
                    auto path = entry.path().string();
                    std::replace(path.begin(), path.end(), '\\', '/');
                    imgConv.SetInputFile(path.c_str());
                    auto alpha = imgConv.DoConvert(true);

                    // Replace texture
                    auto newTexSrc = TextureSource(texSrc);
                    auto newPath = entry.path().relative_path().replace_extension(".pyxi").string();
                    std::replace(newPath.begin(), newPath.end(), '\\', '/');
                    memset(newTexSrc.path, 0, MAX_PATH);
                    memcpy(newTexSrc.path, newPath.c_str(), newPath.length());

                    efig.ReplaceTextureSource(texSrc, newTexSrc);
                    if (alpha) efig.EnableAlphaModeByTexture(newTexSrc.path);
                    break;
                }
            }
        }
    }
    return true;
}

bool FigureMeta::loadModel() {
    // Setting figure loader options
    pyxieFigureExportConfigManager::Instance().ClearOption();
    for (auto& [key, val] : m_options) {
        if (val.type() == nlohmann::json::value_t::boolean
            || val.type() == nlohmann::json::value_t::number_integer
            || val.type() == nlohmann::json::value_t::number_unsigned) {
            pyxieFigureExportConfigManager::Instance().SetOptionInt(key.c_str(), val);
        }
        else if (val.type() == nlohmann::json::value_t::number_float) {
            pyxieFigureExportConfigManager::Instance().SetOptionFloat(key.c_str(), val);
        }
        else if (val.type() == nlohmann::json::value_t::string) {
            pyxieFigureExportConfigManager::Instance().SetOptionString(key.c_str(), val.get<std::string>().c_str());
        }
        else {
            // TODO: Just ignore for now
        }
    }

    auto fsPath = fs::path(m_path);
    auto curPath = fs::current_path();
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, curPath) : fsPath;
    auto modelPath = relPath.string();
    std::replace(modelPath.begin(), modelPath.end(), '\\', '/');

    auto ext = fsPath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    bool isFbx = ext.compare(".fbx") == 0;

    // Load model data and binded animation
    auto parentName = fsPath.parent_path().filename();
    auto baseModelPath = relPath.parent_path();
    if(isFbx)
        baseModelPath.append(parentName.string() + ".fbx");
    else
        baseModelPath.append(parentName.string() + ".dae");
    bool folderRule = fs::exists(baseModelPath);

    auto filename = fsPath.filename().string();
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

    auto basename = baseModelPath.filename().string();
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);

    auto embeddedAnim = false;
    if (folderRule) {
        if (filename.compare(basename) == 0) {
            pyxieFbxLoader fbxLoader;
            pyxieColladaLoader colladaloader;
            auto embeddedAnim = m_options["EMBEDDED_ANIMATION"].get<bool>();

            // Clean-up
            auto baseFigurePath = fsPath.parent_path().append(fsPath.stem().string() + ".pyxf");
            safeDelete(baseFigurePath.string());

            auto baseAnimPath = fsPath.parent_path().append(fsPath.stem().string() + ".pyxa");
            safeDelete(baseAnimPath.string());

            auto efig = ResourceCreator::Instance().NewEditableFigure(modelPath.c_str(), true);

            if (isFbx && fbxLoader.LoadModel(modelPath.c_str(), efig)
                || !isFbx && colladaloader.LoadCollada(modelPath.c_str(), efig)) {
                for (const auto& entry : fs::directory_iterator(relPath.parent_path())) {
                    if (entry.is_regular_file()) {
                        auto ext = entry.path().extension().string();
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                        if (entry.path().filename().compare(baseModelPath.filename()) == 0 || (isFbx && ext.compare(".fbx") != 0) || (!isFbx && ext.compare(".dae") != 0))
                            continue;

                        // Clean-up
                        auto animPath = entry.path().parent_path().append(entry.path().stem().string() + ".pyxa").string();
                        safeDelete(animPath);

                        auto figurePath = entry.path().parent_path().append(entry.path().stem().string() + ".pyxf").string();
                        safeDelete(figurePath);

                        auto entryRelPath = entry.path().is_absolute() ? fs::relative(entry.path(), fs::current_path()).string() : entry.path().string();
                        std::replace(entryRelPath.begin(), entryRelPath.end(), '\\', '/');
                        if (embeddedAnim) {
                            if (isFbx)
                                fbxLoader.LoadAnimation(entryRelPath.c_str(), efig);
                            else
                                colladaloader.LoadColladaAnimation(entryRelPath.c_str(), efig);
                        }
                        else {
                            pyxieFbxLoader fbxAnimLoader;
                            pyxieColladaLoader colladaAnimLoader;
                            auto animFig = ResourceCreator::Instance().NewEditableFigure(entryRelPath.c_str(), true);
                            if (isFbx && fbxLoader.LoadModel(entryRelPath.c_str(), animFig)
                                || !isFbx && colladaloader.LoadCollada(entryRelPath.c_str(), animFig)) {
                                std::replace(animPath.begin(), animPath.end(), '\\', '/');
                                animFig->SaveAnimation(animPath.c_str());
                            }
                        }
                    }
                }
            }
            if (m_options["OPTIMIZE_VERTEX"]) {
                efig->RemoveUnreferencedVertices();
            }
            if (m_options["OPTIMIZE_MESH"]) {
                efig->MergeSameMaterialMesh();
            }
            replaceTextures(*efig);
            auto rv = efig->SaveFigure(baseFigurePath.string().c_str());
            return rv;
        }
    }
    else {
        pyxieFbxLoader fbxLoader;
        pyxieColladaLoader colladaloader;
        auto efig = ResourceCreator::Instance().NewEditableFigure(modelPath.c_str(), true);
        auto embeddedAnim = m_options["EMBEDDED_ANIMATION"].get<bool>();
        bool result = false;

        // Clean
        auto animPath = fsPath.parent_path().append(fsPath.stem().string() + ".pyxa").string();
        safeDelete(animPath);

        auto figurePath = fsPath.parent_path().append(fsPath.stem().string() + ".pyxf").string();
        safeDelete(figurePath);

        if (isFbx && fbxLoader.LoadModel(modelPath.c_str(), efig)
            || !isFbx && colladaloader.LoadCollada(modelPath.c_str(), efig)) {
            if (m_options["OPTIMIZE_VERTEX"]) {
                efig->RemoveUnreferencedVertices();
            }
            if (m_options["OPTIMIZE_MESH"]) {
                efig->MergeSameMaterialMesh();
            }
            replaceTextures(*efig);
            result = efig->SaveFigure(figurePath.c_str());
        }
        if (!embeddedAnim && result) {
            if (isFbx && fbxLoader.LoadAnimation(modelPath.c_str(), efig)
                || !isFbx && colladaloader.LoadColladaAnimation(modelPath.c_str(), efig)) {
                result =  efig->SaveAnimation(animPath.c_str());
            }
        }
        return result;
    }
    return false;
}

bool FigureMeta::save() {
    auto loaded = loadModel();
    if (loaded) {
        Editor::getInstance()->reloadResource(m_path);
    }
    else {
        pyxie_printf("[WARN] Model load failed: %s", m_path.c_str());
    }
    return AssetMeta::save();
}

NS_IGE_END