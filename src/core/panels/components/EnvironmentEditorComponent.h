#ifndef __ENVIRONMENT_EDITOR_COMPONENT_H_
#define __ENVIRONMENT_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class EnvironmentEditorComponent : public EditorComponent
{
public:
	EnvironmentEditorComponent();
	~EnvironmentEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawEnvironmentComponent();
protected:
	std::shared_ptr<Group> m_environmentCompGroup = nullptr;


};

NS_IGE_END

#endif //__ENVIRONMENT_EDITOR_COMPONENT_H_