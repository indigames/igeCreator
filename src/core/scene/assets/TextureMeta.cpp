#include "core/scene/assets/TextureMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include <chrono>
#include <ctime>
#include <iomanip>

#include "pyxieImageConv.h"
using namespace pyxie;

NS_IGE_BEGIN

//! Convert texture
bool convertTexture(const std::string& path, std::unordered_map<std::string, json>& options) {
    auto fsPath = fs::path(path);
    auto parentPath = fsPath.parent_path();
    auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()) : fsPath;
    auto relPathStr = relPath.string();
    std::replace(relPathStr.begin(), relPathStr.end(), '\\', '/');

    ImageConv imgConv;
    imgConv.SetInputFile(relPathStr.c_str());
    imgConv.SetOutputFile((relPath.parent_path().append(relPath.stem().string() + ".pyxi")).c_str());

    auto platform = options["IsMobile"].get<bool>() ? (int)TargetPlatform::MOBILE_Platform : (int)TargetPlatform::PC_Platform;
    imgConv.SetTargetPlatform(platform);

    auto isNormalMap = options["IsNormalMap"].get<bool>();
    imgConv.SetIsNormalmap(isNormalMap);

    auto isWrapRepeat = options["WrapRepeat"].get<bool>();
    imgConv.SetWrapRepeat(isWrapRepeat);

    auto useMipmap = options["UseMipmap"].get<bool>();
    imgConv.SetNoMipmaps(!useMipmap);

    auto quality = options["Quality"].get<int>();
    if (quality < 0) quality = 0;
    if (quality > 3) quality = 3;
    imgConv.SetQuality(quality);

    imgConv.SetAutoDetectAlpha();
    imgConv.DoConvert();
    return true;
}

TextureMeta::TextureMeta(const std::string& path)
	: AssetMeta(path) 
{
    m_options = {
        {"IsMobile", true},
        {"IsNormalMap", false},
        {"WrapRepeat", false},
        {"UseMipmap", false},
        {"Quality", 1},
    };
    loadOptions();
}

TextureMeta::~TextureMeta() {
}

bool TextureMeta::save() {
    // Do not convert, it take lots of time so better do it when build rom
    //std::thread convertThread(convertTexture, m_path, m_options);
    //convertThread.detach();
    return AssetMeta::save();
}

NS_IGE_END