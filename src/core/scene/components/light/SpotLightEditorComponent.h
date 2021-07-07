#ifndef __SPOT_LIGHT_EDITOR_COMPONENT_H_
#define __SPOT_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class SpotLightEditorComponent : public EditorComponent
{
public:
	SpotLightEditorComponent();
	~SpotLightEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawSpotLight();

protected:
	std::shared_ptr<Group> m_spotLightGroup = nullptr;	
};

NS_IGE_END

#endif //__SPOT_LIGHT_EDITOR_COMPONENT_H_