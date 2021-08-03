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

inline const std::vector<std::string> &GetFileExtensionSuported(E_FileExts fileExt) {
    static std::vector<std::string> formats = {};
    switch (fileExt) {
    case E_FileExts::Hidden:
        formats = { ".pyxf", ".pyxi", ".git",".gitignore", ".pyc", ".pyxd", ".meta", ".igeproj", ".ini", ".tmp", "__pycache__", "config", "release"};
        break;
    case E_FileExts::Sprite:
        formats = {".png", ".tga", ".jpg", ".jpeg", ".bmp"};
        break;
    case E_FileExts::Figure:
        formats = {".dae", ".fbx"};
        break;
    case E_FileExts::Script:
        formats = {".py"};
        break;
    case E_FileExts::Font:
        formats = {".ttf", ".otf"};
        break;
    case E_FileExts::FontBitmap:
        formats = { ".pybm"};
        break;
    case E_FileExts::Audio:
        formats = {".wav", ".ogg", ".mp3"};
        break;
    case E_FileExts::Prefab:
        formats = { ".prefab" };
        break;
    case E_FileExts::Scene:
        formats = { ".scene" };
        break;
    case E_FileExts::Particle:
        formats = { ".efk" };
        break;
    default:
        formats = {};
        break;
    }
    return formats;
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
