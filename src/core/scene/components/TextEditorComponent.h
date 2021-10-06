#ifndef __TEXT_EDITOR_COMPONENT_H_
#define __TEXT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class TextEditorComponent : public EditorComponent
{
public:
	TextEditorComponent();
	virtual ~TextEditorComponent();

protected:
	virtual void onInspectorUpdate() override;
	void drawComponent();

protected:
	std::shared_ptr<Group> m_textCompGroup = nullptr;
};

NS_IGE_END

#endif //__TEXT_EDITOR_COMPONENT_H_