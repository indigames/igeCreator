#ifndef __SPRITE_EDITOR_COMPONENT_H_
#define __SPRITE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class SpriteEditorComponent : public EditorComponent
{
public:
	SpriteEditorComponent();
	virtual ~SpriteEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawSpriteComponent();
protected:
	std::shared_ptr<Group> m_spriteCompGroup = nullptr;


};

NS_IGE_END

#endif //__SCRIPT_EDITOR_COMPONENT_H_