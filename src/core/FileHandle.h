#pragma once
#include <string>
#include <vector>
#include <algorithm>

enum class E_FileExts {
    Hidden = 0,
    Sprite,
    Figure,
    Script,
    Font,
    Audio,
    Prefab,
    Scene,
    Particle,
    AnimationClip,
    Animator
};

// Useful when iterate though all supported file types
const auto AllFileExts =  {
    E_FileExts::Sprite,
    E_FileExts::Figure,
    E_FileExts::Script,
    E_FileExts::Font,
    E_FileExts::Audio,
    E_FileExts::Prefab,
    E_FileExts::Scene,
    E_FileExts::Particle,
    E_FileExts::AnimationClip,
    E_FileExts::Animator
};

inline std::vector<std::string> GetFileExtensionSuported(E_FileExts fileExt) {
    switch (fileExt) {
    case E_FileExts::Hidden:
        return { ".pyxf", ".pyxi", ".git",".gitignore", ".pyc", ".pyxd", ".meta", ".igeproj", ".ini", ".tmp", ".log", "__pycache__", "config", "release"};
        break;
    case E_FileExts::Sprite:
        return {".png", ".tga", ".jpg", ".jpeg", ".bmp"};
        break;
    case E_FileExts::Figure:
        return {".dae", ".fbx"};
        break;
    case E_FileExts::Script:
        return {".py", ".PY"};
        break;
    case E_FileExts::Font:
        return {".ttf", ".otf", ".pybm" };
        break;
    case E_FileExts::Audio:
        return {".wav", ".ogg", ".mp3"};
        break;
    case E_FileExts::Prefab:
        return { ".prefab" };
        break;
    case E_FileExts::Scene:
        return { ".scene" };
        break;
    case E_FileExts::Particle:
        return { ".efk" };
        break;
    case E_FileExts::AnimationClip:
        return { ".pyxa" };
        break;
    case E_FileExts::Animator:
        return { ".anim" };
        break;
    default:
        break;
    }
    return {};
}

inline bool IsFormat(E_FileExts fileExt, const std::string &ex) {
    if (ex.empty() || ex.size() <= 0 || ex.compare("") == 0) return false;
    auto ext = ex; std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    const auto &supportExts = GetFileExtensionSuported(fileExt);
    return std::find(std::begin(supportExts), std::end(supportExts), ext) != std::end(supportExts);
}
