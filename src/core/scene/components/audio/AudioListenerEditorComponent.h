#ifndef __AUDIO_LISTENER_EDITOR_COMPONENT_H_
#define __AUDIO_LISTENER_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class AudioListenerEditorComponent : public EditorComponent
{
public:
	AudioListenerEditorComponent();
	virtual ~AudioListenerEditorComponent();

protected:
	//! Internal inspector update
	void onInspectorUpdate() override {}
};

NS_IGE_END

#endif //__AUDIO_LISTENER_EDITOR_COMPONENT_H_