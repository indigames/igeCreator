#ifndef __PHYSIC_BOX_EDITOR_COMPONENT_H_
#define __PHYSIC_BOX_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

#include "core/panels/components/physic/PhysicObjectEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PhysicBoxEditorComponent : public PhysicObjectEditorComponent
{
public:
	PhysicBoxEditorComponent();
	~PhysicBoxEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawPhysicBox();

};

NS_IGE_END

#endif //__PHYSIC_BOX_EDITOR_COMPONENT_H_