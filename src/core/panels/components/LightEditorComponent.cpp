#include "core/panels/components/LightEditorComponent.h"

#include <core/layout/Group.h>

#include "components/LightComponent.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

LightEditorComponent::LightEditorComponent() {
}

LightEditorComponent::~LightEditorComponent()
{
}

bool LightEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<LightComponent*>(comp);
}

NS_IGE_END