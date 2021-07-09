#ifndef __PHYSIC_MESH_EDITOR_COMPONENT_H_
#define __PHYSIC_MESH_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

#include "core/scene/components/physic/PhysicObjectEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PhysicMeshEditorComponent : public PhysicObjectEditorComponent
{
public:
	PhysicMeshEditorComponent();
	~PhysicMeshEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawPhysicMesh();
};

NS_IGE_END

#endif //__PHYSIC_MESH_EDITOR_COMPONENT_H_