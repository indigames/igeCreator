#pragma once

#ifndef __ASSET_META_H_
#define __ASSET_META_H_

#include <string>
#include "core/Macros.h"
#include "core/layout/Group.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

NS_IGE_BEGIN

class AssetMeta {
public:
	AssetMeta(const std::string& path);
	virtual ~AssetMeta();

	//! Load options
	virtual bool loadOptions();

	//! Save to meta file
	virtual bool save();

	//! Draw
	virtual void draw(std::shared_ptr<Group> group);

	//! Get/Set option
	virtual json getOption(const std::string& key);
	virtual void setOption(const std::string& key, json val);

	//! Util to delete file + meta
	static bool safeDelete(const std::string& path);

protected:
	//! Path to the asset file
	std::string m_path;

	//! Options
	std::unordered_map<std::string, json> m_options;
};

NS_IGE_END

#endif