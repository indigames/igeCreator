#ifndef __LIGHT_EDITOR_COMPONENT_H_
#define __LIGHT_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class LightEditorComponent : public EditorComponent
{
public:
	LightEditorComponent();
	~LightEditorComponent();

protected:
	virtual bool isSafe(Component* comp) override;

};

NS_IGE_END

#endif //__LIGHT_EDITOR_COMPONENT_H_