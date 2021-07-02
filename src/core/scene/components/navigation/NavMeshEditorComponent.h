#ifndef __NAV_MESH_EDITOR_COMPONENT_H_
#define __NAV_MESH_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class NavMeshEditorComponent : public EditorComponent
{
public:
	NavMeshEditorComponent();
	virtual ~NavMeshEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawNavMesh();
protected:
	std::shared_ptr<Group> m_navMeshGroup = nullptr;

	
};

NS_IGE_END

#endif //__NAV_MESH_EDITOR_COMPONENT_H_