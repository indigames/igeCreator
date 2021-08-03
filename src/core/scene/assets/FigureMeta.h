#ifndef __FIGURE_META_H_
#define __FIGURE_META_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "core/scene/assets/AssetMeta.h"
#include "components/Component.h"

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
};

NS_IGE_END

#endif //__FIGURE_META_H_