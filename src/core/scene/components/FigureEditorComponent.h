#ifndef __FIGURE_EDITOR_COMPONENT_H_
#define __FIGURE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class FigureEditorComponent : public EditorComponent
{
public:
	FigureEditorComponent();
	virtual ~FigureEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawFigureComponent();

protected:
	std::shared_ptr<Group> m_figureCompGroup = nullptr;
};

NS_IGE_END

#endif //__FIGURE_EDITOR_COMPONENT_H_