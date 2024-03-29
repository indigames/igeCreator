#ifndef __PHYSIC_OBJECT_EDITOR_COMPONENT_H_
#define __PHYSIC_OBJECT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

#include <components/physic/constraint/FixedConstraint.h>
#include <components/physic/constraint/HingeConstraint.h>
#include <components/physic/constraint/SliderConstraint.h>
#include <components/physic/constraint/SpringConstraint.h>
#include <components/physic/constraint/Dof6SpringConstraint.h>

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class ComboBox;

class RigidbodyEditorComponent : public EditorComponent
{
public:
	RigidbodyEditorComponent();
	virtual ~RigidbodyEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;

	void drawRigidbody();
	void drawPhysicConstraints();

	void drawPhysicConstraint(std::shared_ptr<PhysicConstraint> constraint, std::shared_ptr<Group> constraintGroup);
	void drawFixedConstraint(std::shared_ptr<PhysicConstraint> constraint);
	void drawHingeConstraint(std::shared_ptr<PhysicConstraint> constraint);
	void drawSliderConstraint(std::shared_ptr<PhysicConstraint> constraint);
	void drawSpringConstraint(std::shared_ptr<PhysicConstraint> constraint);
	void drawDof6SpringConstraint(std::shared_ptr<PhysicConstraint> constraint);

protected:
	std::shared_ptr<Group> m_physicGroup = nullptr;
	std::shared_ptr<Group> m_constraintGroup = nullptr;
	std::shared_ptr<ComboBox> m_constraintCreateCombo = nullptr;
};

NS_IGE_END

#endif //__PHYSIC_OBJECT_EDITOR_COMPONENT_H_