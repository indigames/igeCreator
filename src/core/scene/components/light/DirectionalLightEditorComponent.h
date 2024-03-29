#ifndef __DIRECTIONAL_LIGHT_EDITOR_COMPONENT_H_
#define __DIRECTIONAL_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class DirectionalLightEditorComponent : public EditorComponent
{
public:
	DirectionalLightEditorComponent();
	~DirectionalLightEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawDirectionalLight();

protected:
	std::shared_ptr<Group> m_directionalLightGroup = nullptr;
};

NS_IGE_END

#endif //__DIRECTIONAL_LIGHT_EDITOR_COMPONENT_H_