#ifndef __SCRIPT_EDITOR_COMPONENT_H_
#define __SCRIPT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class ScriptEditorComponent : public EditorComponent
{
public:
	ScriptEditorComponent();
	~ScriptEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawScriptComponent();
protected:
	std::shared_ptr<Group> m_scriptCompGroup = nullptr;

	
};

NS_IGE_END

#endif //__SCRIPT_EDITOR_COMPONENT_H_