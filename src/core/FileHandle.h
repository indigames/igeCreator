#pragma once
#include <string>
#include <vector>

enum class E_FileExts {
    Hidden = 0,
    Sprite,
    Figure,
    Script,
    Font,
    FontBitmap,
    Audio,
    Prefab,
    Scene,
    Particle
};

// Useful when iterate though all supported file types
const auto AllFileExts =  {
    E_FileExts::Sprite,
    E_FileExts::Figure,
    E_FileExts::Script,
    E_FileExts::Font,
    E_FileExts::FontBitmap,
    E_FileExts::Audio,
    E_FileExts::Prefab,
    E_FileExts::Scene,
    E_FileExts::Particle
};

inline std::vector<std::string> GetFileExtensionSuported(E_FileExts fileExt) {
    switch (fileExt) {
    case E_FileExts::Hidden:
        return { ".pyxf", ".pyxi", ".git",".gitignore", ".pyc", ".pyxd", ".meta", ".igeproj", ".ini", ".INI", ".tmp", ".TMP", ".log", ".LOG", "__pycache__", "config", "release"};
        break;
    case E_FileExts::Sprite:
        return {".png",".PNG", ".tga",".TGA", ".jpg",".JPG", ".jpeg",".JPEG", ".bmp", ".BMP"};
        break;
    case E_FileExts::Figure:
        return {".dae",".DAE", ".fbx", ".FBX"};
        break;
    case E_FileExts::Script:
        return {".py", ".PY"};
        break;
    case E_FileExts::Font:
        return {".ttf", ".TTF", ".otf", ".OTF"};
        break;
    case E_FileExts::FontBitmap:
        return { ".pybm", ".PYBM"};
        break;
    case E_FileExts::Audio:
        return {".wav", ".WAV", ".ogg", ".OGG", ".mp3", ".MP3"};
        break;
    case E_FileExts::Prefab:
        return { ".prefab", ".PREFAB" };
        break;
    case E_FileExts::Scene:
        return { ".scene", ".SCENE" };
        break;
    case E_FileExts::Particle:
        return { ".efk", ".EFK" };
        break;
    default:
        break;
    }
    return {};
}

inline bool IsFormat(E_FileExts fileExt, const std::string &ex) {
    if (ex.empty() || ex.size() <= 0 || ex.compare("") == 0) return false;
    const auto &supported = GetFileExtensionSuported(fileExt);
    return std::find(std::begin(supported), std::end(supported), ex) != std::end(supported);
}

inline const std::vector<std::vector<std::string>> &IGEExtensionSuported()
{
    static const std::vector<std::vector<std::string>> types = {
        GetFileExtensionSuported(E_FileExts::Figure),
        GetFileExtensionSuported(E_FileExts::Sprite)
    };
    return types;
}

inline bool IsIGEExtensionSuported(const std::string &ex) {
    std::vector<std::string> types;
    types.insert(types.end(), GetFileExtensionSuported(E_FileExts::Figure).begin(), GetFileExtensionSuported(E_FileExts::Figure).end());
    types.insert(types.end(), GetFileExtensionSuported(E_FileExts::Sprite).begin(), GetFileExtensionSuported(E_FileExts::Sprite).end());
    return std::find(std::begin(types), std::end(types), ex) != std::end(types);
}
