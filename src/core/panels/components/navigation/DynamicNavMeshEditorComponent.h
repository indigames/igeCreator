#ifndef __DYNAMIC_NAV_MESH_EDITOR_COMPONENT_H_
#define __DYNAMIC_NAV_MESH_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"
#include "core/panels/components/navigation/NavMeshEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class DynamicNavMeshEditorComponent : public NavMeshEditorComponent
{
public:
	DynamicNavMeshEditorComponent();
	~DynamicNavMeshEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawDynamicNavMesh();
	
};

NS_IGE_END

#endif //__DYNAMIC_NAV_MESH_EDITOR_COMPONENT_H_