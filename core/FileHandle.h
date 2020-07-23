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

template <typename T>
const std::vector<std::string>& GetFileExtensionSuported();

template <>
inline const std::vector<std::string>& GetFileExtensionSuported<SpriteComponent>()
{
	static std::vector<std::string> formats = { ".pyxi" };
	return formats;
}

template <>
inline const std::vector<std::string>& GetFileExtensionSuported<FigureComponent>()
{
	static std::vector<std::string> formats = { ".pyxf" };
	return formats;
}

template <>
inline const std::vector<std::string>& GetFileExtensionSuported<ScriptComponent>()
{
	static std::vector<std::string> formats = { ".py" };
	return formats;
}


template<typename T>
inline bool IsFormat(const std::string& ex)
{
	const auto& supported = GetFileExtensionSuported<T>();
	return std::find(std::begin(supported), std::end(supported), ex) != std::end(supported);
}

inline const std::vector<std::vector<std::string>>& IGEExtensionSuported()
{
	static const std::vector<std::vector<std::string>> types = {
		GetFileExtensionSuported<SpriteComponent>(),
		GetFileExtensionSuported<FigureComponent>() };

	return types;
}

inline bool IsIGEExtensionSuported(const std::string& ex)
{
	std::vector<std::string> types;
	types.insert(types.end(), GetFileExtensionSuported<SpriteComponent>().begin(), GetFileExtensionSuported<SpriteComponent>().end());
	types.insert(types.end(), GetFileExtensionSuported<FigureComponent>().begin(), GetFileExtensionSuported<FigureComponent>().end());

	return std::find(std::begin(types), std::end(types), ex) != std::end(types);
}