#ifndef __ANIMATOR_EDITOR_COMPONENT_H_
#define __ANIMATOR_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class AnimatorEditorComponent  : public EditorComponent
{
public:
	AnimatorEditorComponent ();
	virtual ~AnimatorEditorComponent ();

protected:
	virtual void onInspectorUpdate() override;
	void drawComponent();

protected:
	std::shared_ptr<Group> m_compGroup = nullptr;
};

NS_IGE_END

#endif //__ANIMATOR_EDITOR_COMPONENT_H_