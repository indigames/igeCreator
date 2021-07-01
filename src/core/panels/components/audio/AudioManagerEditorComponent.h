#ifndef __AUDIO_MANAGER_EDITOR_COMPONENT_H_
#define __AUDIO_MANAGER_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class AudioManagerEditorComponent : public EditorComponent
{
public:
	AudioManagerEditorComponent();
	virtual ~AudioManagerEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawAudioManager();
protected:
	std::shared_ptr<Group> m_audioManagerGroup = nullptr;


};

NS_IGE_END

#endif //__AUDIO_MANAGER_EDITOR_COMPONENT_H_