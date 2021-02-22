#ifndef __POINT_LIGHT_EDITOR_COMPONENT_H_
#define __POINT_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"
#include "core/panels/components/LightEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PointLightEditorComponent : public LightEditorComponent
{
public:
	PointLightEditorComponent();
	~PointLightEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawPointLight();
protected:
	std::shared_ptr<Group> m_pointLightGroup = nullptr;

	
};

NS_IGE_END

#endif //__POINT_LIGHT_EDITOR_COMPONENT_H_