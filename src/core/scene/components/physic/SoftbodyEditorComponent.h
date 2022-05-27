#ifndef __PHYSIC_SOFT_BODY_EDITOR_COMPONENT_H_
#define __PHYSIC_SOFT_BODY_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

#include "core/scene/components/physic/RigidbodyEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class SoftbodyEditorComponent : public RigidbodyEditorComponent
{
public:
	SoftbodyEditorComponent();
	~SoftbodyEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawSoftbody();
};

NS_IGE_END

#endif //__PHYSIC_SOFT_BODY_EDITOR_COMPONENT_H_