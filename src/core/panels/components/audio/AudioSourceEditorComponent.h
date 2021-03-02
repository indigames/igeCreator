#ifndef __AUDIO_SOURCE_EDITOR_COMPONENT_H_
#define __AUDIO_SOURCE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class AudioSourceEditorComponent : public EditorComponent
{
public:
	AudioSourceEditorComponent();
	~AudioSourceEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawAudioSource();
protected:
	std::shared_ptr<Group> m_audioSourceGroup = nullptr;


};

NS_IGE_END

#endif //__AUDIO_SOURCE_EDITOR_COMPONENT_H_