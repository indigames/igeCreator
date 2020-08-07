#pragma once
#include <string>
#include <vector>

#include <Components/CameraComponent.h>
#include <Components/TransformComponent.h>
#include <Components/EnvironmentComponent.h>
#include <Components/FigureComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/ScriptComponent.h>

using namespace ige::scene;

enum class E_FileExts {
	Sprite = 0,
	Figure,
	Script,
	Font
};

inline const std::vector<std::string>& GetFileExtensionSuported(E_FileExts fileExt)
{
	static std::vector<std::string> formats = {};
	switch (fileExt)
	{
	case E_FileExts::Sprite:
		formats = { ".pyxi" };
		break;
	case E_FileExts::Figure:
		formats = { ".pyxf" };
		break;
	case E_FileExts::Script:
		formats = { ".py" };
		break;
	case E_FileExts::Font:
		formats = { ".ttf", ".otf" };
		break;
	default:
		formats = {};
		break;
	}
	return formats;
}

inline bool IsFormat(E_FileExts fileExt, const std::string& ex)
{
	const auto& supported = GetFileExtensionSuported(fileExt);
	return std::find(std::begin(supported), std::end(supported), ex) != std::end(supported);
}

inline const std::vector<std::vector<std::string>>& IGEExtensionSuported()
{
	static const std::vector<std::vector<std::string>> types = {
		GetFileExtensionSuported(E_FileExts::Figure),
		GetFileExtensionSuported(E_FileExts::Sprite) };

	return types;
}

inline bool IsIGEExtensionSuported(const std::string& ex)
{
	std::vector<std::string> types;
	types.insert(types.end(), GetFileExtensionSuported(E_FileExts::Figure).begin(), GetFileExtensionSuported(E_FileExts::Figure).end());
	types.insert(types.end(), GetFileExtensionSuported(E_FileExts::Sprite).begin(), GetFileExtensionSuported(E_FileExts::Sprite).end());

	return std::find(std::begin(types), std::end(types), ex) != std::end(types);
}