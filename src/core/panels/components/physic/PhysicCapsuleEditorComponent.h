#ifndef __PHYSIC_CAPSULE_EDITOR_COMPONENT_H_
#define __PHYSIC_CAPSULE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

#include "core/panels/components/physic/PhysicObjectEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PhysicCapsuleEditorComponent : public PhysicObjectEditorComponent
{
public:
	PhysicCapsuleEditorComponent();
	~PhysicCapsuleEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawPhysicCapsule();

};

NS_IGE_END

#endif //__PHYSIC_CAPSULE_EDITOR_COMPONENT_H_