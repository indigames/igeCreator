#ifndef __OFFMESHLINK_EDITOR_COMPONENT_H_
#define __OFFMESHLINK_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class OffMeshLinkEditorComponent : public EditorComponent
{
public:
	OffMeshLinkEditorComponent();
	~OffMeshLinkEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawOffMeshLink();
protected:
	std::shared_ptr<Group> m_offMeshLinkGroup = nullptr;

	
};

NS_IGE_END

#endif //__OFFMESHLINK_EDITOR_COMPONENT_H_