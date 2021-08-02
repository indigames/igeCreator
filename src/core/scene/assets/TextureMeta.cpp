#include "core/scene/assets/TextureMeta.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include <chrono>
#include <ctime>
#include <iomanip>

#include "pyxieImageConv.h"
using namespace pyxie;

NS_IGE_BEGIN

TextureMeta::TextureMeta(const std::string& path)
	: AssetMeta(path) 
{
    m_options = {
        {"IsMobile", true},
        {"IsNormalMap", false},
        {"WrapRepeat", false},
        {"UseMipmap", false},
        {"Quality", 2},
    };
    loadOptions();
}

TextureMeta::~TextureMeta() {
}

//! Convert texture
bool TextureMeta::convertTexture(const std::string& path) {
	auto fsPath = fs::path(path);
	auto parentPath = fsPath.parent_path();
	auto relPath = fsPath.is_absolute() ? fs::relative(fsPath, fs::current_path()) : fsPath;
	auto relPathStr = relPath.string();
	std::replace(relPathStr.begin(), relPathStr.end(), '\\', '/');

	ImageConv imgConv;
	imgConv.SetInputFile(relPathStr.c_str());
	imgConv.SetOutputFile((relPath.parent_path().append(relPath.stem().string() + ".pyxi")).c_str());
	
	auto platform = m_options["IsMobile"].get<bool>() ? (int)TargetPlatform::MOBILE_Platform : (int)TargetPlatform::PC_Platform;
	imgConv.SetTargetPlatform(platform);

	auto isNormalMap = m_options["IsNormalMap"].get<bool>();
	imgConv.SetIsNormalmap(isNormalMap);

	auto isWrapRepeat = m_options["WrapRepeat"].get<bool>();
	imgConv.SetWrapRepeat(isWrapRepeat);

	auto useMipmap = m_options["UseMipmap"].get<bool>();
	imgConv.SetNoMipmaps(!useMipmap);
	
	auto quality = m_options["Quality"].get<int>();
	if (quality < 0) quality = 0;
	if (quality > 3) quality = 3;
	imgConv.SetQuality(quality);

	imgConv.SetAutoDetectAlpha();
	imgConv.DoConvert();	
    return true;
}

bool TextureMeta::save() {
	convertTexture(m_path);
    return AssetMeta::save();
}

NS_IGE_END