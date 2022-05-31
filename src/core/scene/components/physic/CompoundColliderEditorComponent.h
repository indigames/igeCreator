#ifndef __PHYSIC_COMPOUND_EDITOR_COMPONENT_H_
#define __PHYSIC_COMPOUND_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class CompoundColliderEditorComponent : public EditorComponent
{
public:
	CompoundColliderEditorComponent();
	~CompoundColliderEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawCollider();

protected:
	std::shared_ptr<Group> m_physicGroup = nullptr;
};

NS_IGE_END

#endif //__PHYSIC_COMPOUND_EDITOR_COMPONENT_H_