#ifndef __PHYSIC_OBJECT_EDITOR_COMPONENT_H_
#define __PHYSIC_OBJECT_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

#include <components/physic/FixedConstraint.h>
#include <components/physic/HingeConstraint.h>
#include <components/physic/SliderConstraint.h>
#include <components/physic/SpringConstraint.h>
#include <components/physic/Dof6SpringConstraint.h>

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class ComboBox;

class PhysicObjectEditorComponent : public EditorComponent
{
public:
	PhysicObjectEditorComponent();
	~PhysicObjectEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawPhysicObject();
	void drawPhysicConstraints();

	void drawPhysicConstraint(const std::shared_ptr<PhysicConstraint>& constraint, std::shared_ptr<Group> constraintGroup);
	void drawFixedConstraint(const std::shared_ptr<PhysicConstraint>& constraint);
	void drawHingeConstraint(const std::shared_ptr<PhysicConstraint>& constraint);
	void drawSliderConstraint(const std::shared_ptr<PhysicConstraint>& constraint);
	void drawSpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint);
	void drawDof6SpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint);
protected:
	std::shared_ptr<Group> m_physicGroup = nullptr;
	std::shared_ptr<Group> m_constraintGroup = nullptr;
	std::shared_ptr<ComboBox> m_constraintCreateCombo = nullptr;
};

NS_IGE_END

#endif //__PHYSIC_OBJECT_EDITOR_COMPONENT_H_