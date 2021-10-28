#ifndef __EDITABLE_FIGURE_EDITOR_COMPONENT_H_
#define __EDITABLE_FIGURE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class EditableFigureEditorComponent : public EditorComponent
{
public:
	EditableFigureEditorComponent();
	virtual ~EditableFigureEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawFigureComponent();

protected:
	std::shared_ptr<Group> m_figureCompGroup = nullptr;
};

NS_IGE_END

#endif //__EDITABLE_FIGURE_EDITOR_COMPONENT_H_