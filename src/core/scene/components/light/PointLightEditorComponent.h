#ifndef __POINT_LIGHT_EDITOR_COMPONENT_H_
#define __POINT_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PointLightEditorComponent : public EditorComponent
{
public:
	PointLightEditorComponent();
	~PointLightEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawPointLight();

protected:
	std::shared_ptr<Group> m_pointLightGroup = nullptr;	
};

NS_IGE_END

#endif //__POINT_LIGHT_EDITOR_COMPONENT_H_