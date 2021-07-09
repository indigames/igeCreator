#ifndef __AMBIENT_LIGHT_EDITOR_COMPONENT_H_
#define __AMBIENT_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class AmbientLightEditorComponent : public EditorComponent
{
public:
	AmbientLightEditorComponent();
	~AmbientLightEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawAmbientLight();

protected:
	std::shared_ptr<Group> m_ambientLightGroup = nullptr;
};

NS_IGE_END

#endif //__AMBIENT_LIGHT_EDITOR_COMPONENT_H_