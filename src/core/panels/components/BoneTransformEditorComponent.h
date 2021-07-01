#ifndef __BONE_TRANSFORM_EDITOR_COMPONENT_H_
#define __BONE_TRANSFORM_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class BoneTransformEditorComponent  : public EditorComponent
{
public:
	BoneTransformEditorComponent ();
	virtual ~BoneTransformEditorComponent ();

protected:
	virtual void onInspectorUpdate() override;
	void drawComponent();

protected:
	std::shared_ptr<Group> m_compGroup = nullptr;
};

NS_IGE_END

#endif //__SCRIPT_EDITOR_COMPONENT_H_