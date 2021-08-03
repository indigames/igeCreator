#ifndef __TEXTURE_META_H_
#define __TEXTURE_META_H_

#include "core/scene/assets/AssetMeta.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class TextureMeta : public AssetMeta
{
public:
	TextureMeta(const std::string& path);
	virtual ~TextureMeta();

	//! Save to file
	virtual bool save() override;

protected:
	//! Convert texture
	bool convertTexture(const std::string& path);
};

NS_IGE_END

#endif //__TEXTURE_META_H_