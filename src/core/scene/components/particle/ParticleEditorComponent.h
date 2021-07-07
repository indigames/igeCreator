#ifndef __PARTICLE_EDITOR_COMPONENT_H_
#define __PARTICLE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class ParticleEditorComponent : public EditorComponent
{
public:
	ParticleEditorComponent();
	virtual ~ParticleEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawParticle();

protected:
	std::shared_ptr<Group> m_particleGroup = nullptr;
};

NS_IGE_END

#endif //__PARTICLE_EDITOR_COMPONENT_H_