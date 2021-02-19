#ifndef __SPOT_LIGHT_EDITOR_COMPONENT_H_
#define __SPOT_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"
#include "core/panels/components/LightEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class SpotLightEditorComponent : public LightEditorComponent
{
public:
	SpotLightEditorComponent();
	~SpotLightEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawSpotLight();
protected:
	std::shared_ptr<Group> m_spotLightGroup = nullptr;

	
};

NS_IGE_END

#endif //__SPOT_LIGHT_EDITOR_COMPONENT_H_