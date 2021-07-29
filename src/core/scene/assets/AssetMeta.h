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

	//! Save to meta file
	virtual bool save();

	//! Draw
	virtual void draw(std::shared_ptr<Group> group);

protected:
	std::string m_path;
};

NS_IGE_END

#endif