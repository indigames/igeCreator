#ifndef __AMBIENT_LIGHT_EDITOR_COMPONENT_H_
#define __AMBIENT_LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"
#include "core/panels/components/LightEditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class AmbientLightEditorComponent : public LightEditorComponent
{
public:
	AmbientLightEditorComponent();
	~AmbientLightEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawAmbientLight();
protected:
	std::shared_ptr<Group> m_ambientLightGroup = nullptr;

	
};

NS_IGE_END

#endif //__AMBIENT_LIGHT_EDITOR_COMPONENT_H_