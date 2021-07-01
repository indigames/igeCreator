#ifndef __PARTICLE_MANAGER_EDITOR_COMPONENT_H_
#define __PARTICLE_MANAGER_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class ParticleManagerEditorComponent : public EditorComponent
{
public:
	ParticleManagerEditorComponent();
	virtual ~ParticleManagerEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawParticleManager();
protected:
	std::shared_ptr<Group> m_particleManagerGroup = nullptr;

};

NS_IGE_END

#endif //__PARTICLE_MANAGER_EDITOR_COMPONENT_H_