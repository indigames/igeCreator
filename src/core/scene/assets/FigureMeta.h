#ifndef __FIGURE_META_H_
#define __FIGURE_META_H_

#include "core/scene/assets/AssetMeta.h"

#include <utils/PyxieHeaders.h>

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class FigureMeta : public AssetMeta
{
public:
	FigureMeta(const std::string& path);
	virtual ~FigureMeta();

	//! Save to file
	virtual bool save() override;

	//! Draw
	virtual void draw(std::shared_ptr<Group> group) override;

protected:
	//! Load figure
	bool loadCollada(EditableFigure& efig, const std::string& path);

	//! Load fbx
	bool loadFbx(EditableFigure& efig, const std::string& path);

	//! Replace texture path
	bool replaceTextures(EditableFigure& efig);
};

NS_IGE_END

#endif //__FIGURE_META_H_