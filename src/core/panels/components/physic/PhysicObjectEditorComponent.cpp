#include "core/panels/components/physic/PhysicObjectEditorComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicObject.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include "core/Editor.h"

NS_IGE_BEGIN

PhysicObjectEditorComponent::PhysicObjectEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicObjectEditorComponent::~PhysicObjectEditorComponent()
{
    if (m_constraintGroup)
    {
        m_constraintGroup->removeAllWidgets();
        m_constraintGroup->removeAllPlugins();
        m_constraintGroup = nullptr;
    }

    if (m_physicGroup) {
        m_physicGroup->removeAllWidgets();
        m_physicGroup->removeAllPlugins();
    }
    m_physicGroup = nullptr;
}

bool PhysicObjectEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<PhysicObject*>(comp);
}

void PhysicObjectEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicGroup == nullptr) {
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);
    }
    drawPhysicObject();

    EditorComponent::redraw();
}

void PhysicObjectEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);

    drawPhysicObject();
}

void PhysicObjectEditorComponent::drawPhysicObject()
{
    if (m_physicGroup == nullptr)
        return;
    m_physicGroup->removeAllWidgets();

    auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
    if (physicComp == nullptr)
        return;

    auto columns = m_physicGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("Enable", physicComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setEnabled(val);
        });

    columns->createWidget<CheckBox>("Continous", physicComp->isCCD())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setCCD(val);
        });

    columns->createWidget<CheckBox>("Kinematic", physicComp->isKinematic())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setIsKinematic(val);
        dirty();
        });

    columns->createWidget<CheckBox>("Trigger", physicComp->isTrigger())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setIsTrigger(val);
        });

    std::array filterGroup = { physicComp->getCollisionFilterGroup() };
    m_physicGroup->createWidget<Drag<int>>("Collision Group", ImGuiDataType_S32, filterGroup, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setCollisionFilterGroup(val[0]);
        });

    std::array filterMask = { physicComp->getCollisionFilterMask() };
    m_physicGroup->createWidget<Drag<int>>("Collision Mask", ImGuiDataType_S32, filterMask, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setCollisionFilterMask(val[0]);
        });

    std::array mass = { physicComp->getMass() };
    m_physicGroup->createWidget<Drag<float>>("Mass", ImGuiDataType_Float, mass, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setMass(val[0]);
        });

    std::array friction = { physicComp->getFriction() };
    m_physicGroup->createWidget<Drag<float>>("Friction", ImGuiDataType_Float, friction, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setFriction(val[0]);
        });

    std::array restitution = { physicComp->getRestitution() };
    m_physicGroup->createWidget<Drag<float>>("Restitution", ImGuiDataType_Float, restitution, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setRestitution(val[0]);
        });

    std::array linearVelocity = { physicComp->getLinearVelocity().x(), physicComp->getLinearVelocity().y(), physicComp->getLinearVelocity().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Linear Velocity", ImGuiDataType_Float, linearVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setLinearVelocity({ val[0], val[1], val[2] });
        });

    std::array angularVelocity = { physicComp->getAngularVelocity().x(), physicComp->getAngularVelocity().y(), physicComp->getAngularVelocity().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Angular Velocity", ImGuiDataType_Float, angularVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setAngularVelocity({ val[0], val[1], val[2] });
        });

    std::array linearFactor = { physicComp->getLinearFactor().x(), physicComp->getLinearFactor().y(), physicComp->getLinearFactor().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Linear Factor", ImGuiDataType_Float, linearFactor)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setLinearFactor({ val[0], val[1], val[2] });
        });

    std::array angularFactor = { physicComp->getAngularFactor().x(), physicComp->getAngularFactor().y(), physicComp->getAngularFactor().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Angular Factor", ImGuiDataType_Float, angularFactor)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setAngularFactor({ val[0], val[1], val[2] });
        });

    std::array margin = { physicComp->getCollisionMargin() };
    m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = dynamic_cast<PhysicObject*>(getComponent());
        physicComp->setCollisionMargin(val[0]);
        });
}

void PhysicObjectEditorComponent::drawPhysicConstraints()
{
    if (m_constraintGroup)
    {
        m_constraintGroup->removeAllWidgets();
        m_constraintGroup->removeAllPlugins();
        m_constraintGroup = nullptr;
    }

    m_constraintGroup = m_physicGroup->createWidget<Group>("ConstraintGroup");

    // Create constraint selection
    m_constraintCreateCombo = m_constraintGroup->createWidget<ComboBox>(0, true, false);
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Fixed, "Fixed Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Hinge, "Hinge Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Slider, "Slider Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Spring, "Spring Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Dof6Spring, "Dof6-Spring Constraint");

    // Add button
    auto createCompButton = m_constraintGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
    createCompButton->getOnClickEvent().addListener([this](auto widget) {
        switch (m_constraintCreateCombo->getSelectedIndex())
        {
        case (int)PhysicConstraint::ConstraintType::Fixed:
            dynamic_cast<PhysicObject*>(getComponent())->addConstraint<FixedConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Hinge:
            dynamic_cast<PhysicObject*>(getComponent())->addConstraint<HingeConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Slider:
            dynamic_cast<PhysicObject*>(getComponent())->addConstraint<SliderConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Spring:
            dynamic_cast<PhysicObject*>(getComponent())->addConstraint<SpringConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Dof6Spring:
            dynamic_cast<PhysicObject*>(getComponent())->addConstraint<Dof6SpringConstraint>();
            break;
        }
        dirty();
        });
    m_constraintGroup->createWidget<Separator>();

    const auto& constraints = dynamic_cast<PhysicObject*>(getComponent())->getContraints();
    for (const auto& constraint : constraints)
    {
        if (constraint->getType() == PhysicConstraint::ConstraintType::Fixed)
        {
            drawFixedConstraint(constraint);
        }
        else if (constraint->getType() == PhysicConstraint::ConstraintType::Hinge)
        {
            drawHingeConstraint(constraint);
        }
        else if (constraint->getType() == PhysicConstraint::ConstraintType::Slider)
        {
            drawSliderConstraint(constraint);
        }
        else if (constraint->getType() == PhysicConstraint::ConstraintType::Spring)
        {
            drawSpringConstraint(constraint);
        }
        else if (constraint->getType() == PhysicConstraint::ConstraintType::Dof6Spring)
        {
            drawDof6SpringConstraint(constraint);
        }
    }
}

//! Draw Physic Constraint
void PhysicObjectEditorComponent::drawPhysicConstraint(const std::shared_ptr<PhysicConstraint>& constraint, std::shared_ptr<Group> constraintGroup)
{
    auto columns = constraintGroup->createWidget<Columns<2>>();

    // Enabled
    columns->createWidget<CheckBox>("Enable", constraint->isEnabled())->getOnDataChangedEvent().addListener([&](bool val) {
        constraint->setEnabled(val);
        });

    // Bodies Collision
    columns->createWidget<CheckBox>("Bodies Collision", constraint->isEnableCollisionBetweenBodies())->getOnDataChangedEvent().addListener([&](bool val) {
        constraint->setEnableCollisionBetweenBodies(val);
        });

    // Other body
    auto otherBodyTxt = constraintGroup->createWidget<TextField>("Other body", constraint->getOther() ? constraint->getOther()->getOwner()->getName() : std::string());
    otherBodyTxt->getOnDataChangedEvent().addListener([&](const auto& val) {
        auto obj = Editor::getCurrentScene()->findObjectByName(val);
        constraint->setOtherUUID(obj->getUUID());
        });
    otherBodyTxt->addPlugin<DDTargetPlugin<int>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([&](auto val) {
        auto obj = Editor::getCurrentScene()->findObjectById(val);
        constraint->setOtherUUID(obj->getUUID());
        dirty();
        });

    // Breaking impulse
    std::array breakImpulse = { constraint->getBreakingImpulseThreshold() };
    constraintGroup->createWidget<Drag<float>>("Break Impulse", ImGuiDataType_Float, breakImpulse)->getOnDataChangedEvent().addListener([&](const auto& val) {
        constraint->setBreakingImpulseThreshold(val[0]);
        });
}

//! Draw Fixed Constraint
void PhysicObjectEditorComponent::drawFixedConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("FixedConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([&, this]() {
        dynamic_cast<PhysicObject*>(getComponent())->removeConstraint(constraint);
        dirty();
        });

    // Draw Physic Constraint base
    drawPhysicConstraint(constraint, constraintGroup);
}

//! draw Hinge Constraint
void PhysicObjectEditorComponent::drawHingeConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("HingeConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([&, this]() {
        dynamic_cast<PhysicObject*>(getComponent())->removeConstraint(constraint);
        dirty();
        });

    // Draw Physic Constraint base
    drawPhysicConstraint(constraint, constraintGroup);

    auto hingeConstraint = std::dynamic_pointer_cast<HingeConstraint>(constraint);
    if (hingeConstraint == nullptr)
        return;

    // Draw Hinge Constraint properties
    constraintGroup->createWidget<Separator>();

    // Anchor
    std::array anchor = { hingeConstraint->getAnchor().x(), hingeConstraint->getAnchor().y(), hingeConstraint->getAnchor().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Anchor", ImGuiDataType_Float, anchor)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
        hingeConstraint->setAnchor({ val[0], val[1], val[2] });
        });

    // Axis1
    std::array axis1 = { hingeConstraint->getAxis1().x(), hingeConstraint->getAxis1().y(), hingeConstraint->getAxis1().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Axis1", ImGuiDataType_Float, axis1)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
        hingeConstraint->setAxis1({ val[0], val[1], val[2] });
        });

    // Axis 2
    std::array axis2 = { hingeConstraint->getAxis2().x(), hingeConstraint->getAxis2().y(), hingeConstraint->getAxis2().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Axis2", ImGuiDataType_Float, axis2)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
        hingeConstraint->setAxis2({ val[0], val[1], val[2] });
        });

    // Lower limit
    std::array lowerLimit = { hingeConstraint->getLowerLimit() };
    constraintGroup->createWidget<Drag<float>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
        hingeConstraint->setLowerLimit(val[0]);
        });

    // Upper limit
    std::array upperLimit = { hingeConstraint->getUpperLimit() };
    constraintGroup->createWidget<Drag<float>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
        hingeConstraint->setUpperLimit(val[0]);
        });
}

//! draw Slider Constraint
void PhysicObjectEditorComponent::drawSliderConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("SliderConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([&, this]() {
        dynamic_cast<PhysicObject*>(getComponent())->removeConstraint(constraint);
        dirty();
        });

    // Draw Physic Constraint base
    drawPhysicConstraint(constraint, constraintGroup);

    auto sliderConstraint = std::dynamic_pointer_cast<SliderConstraint>(constraint);
    if (sliderConstraint == nullptr)
        return;

    constraintGroup->createWidget<Separator>();

    // Lower limit
    std::array lowerLimit = { sliderConstraint->getLowerLimit().x(), sliderConstraint->getLowerLimit().y(), sliderConstraint->getLowerLimit().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f)->getOnDataChangedEvent().addListener([sliderConstraint](const auto& val) {
        sliderConstraint->setLowerLimit({ val[0], val[1], val[2] });
        });

    // Upper limit
    std::array upperLimit = { sliderConstraint->getUpperLimit().x(), sliderConstraint->getUpperLimit().y(), sliderConstraint->getUpperLimit().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f)->getOnDataChangedEvent().addListener([sliderConstraint](const auto& val) {
        sliderConstraint->setUpperLimit({ val[0], val[1], val[2] });
        });
}

//! draw Spring Constraint
void PhysicObjectEditorComponent::drawSpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("SpringConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([&, this]() {
        dynamic_cast<PhysicObject*>(getComponent())->removeConstraint(constraint);
        dirty();
        });

    // Draw Physic Constraint base
    drawPhysicConstraint(constraint, constraintGroup);

    auto springConstraint = std::dynamic_pointer_cast<SpringConstraint>(constraint);
    if (springConstraint == nullptr)
        return;

    constraintGroup->createWidget<Separator>();

    // Enable
    std::array enable = { springConstraint->getEnable().x(), springConstraint->getEnable().y(), springConstraint->getEnable().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Enable", ImGuiDataType_Float, enable, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
        springConstraint->setEnable({ val[0], val[1], val[2] });
        });

    // Stiffness
    std::array stiffness = { springConstraint->getStiffness().x(), springConstraint->getStiffness().y(), springConstraint->getStiffness().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Stiffness", ImGuiDataType_Float, stiffness, 0.001f, 0.f)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
        springConstraint->setStiffness({ val[0], val[1], val[2] });
        });

    // Damping
    std::array damping = { springConstraint->getDamping().x(), springConstraint->getDamping().y(), springConstraint->getDamping().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Damping", ImGuiDataType_Float, damping, 0.001f, 0.f)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
        springConstraint->setDamping({ val[0], val[1], val[2] });
        });

    // Lower limit
    std::array lowerLimit = { springConstraint->getLowerLimit().x(), springConstraint->getLowerLimit().y(), springConstraint->getLowerLimit().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
        springConstraint->setLowerLimit({ val[0], val[1], val[2] });
        });

    // Upper limit
    std::array upperLimit = { springConstraint->getUpperLimit().x(), springConstraint->getUpperLimit().y(), springConstraint->getUpperLimit().z() };
    constraintGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
        springConstraint->setUpperLimit({ val[0], val[1], val[2] });
        });
}

//! draw Dof6 Spring Constraint
void PhysicObjectEditorComponent::drawDof6SpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("Dof6Constraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([&, this]() {
        dynamic_cast<PhysicObject*>(getComponent())->removeConstraint(constraint);
        dirty();
        });

    // Draw Physic Constraint base
    drawPhysicConstraint(constraint, constraintGroup);

    auto dof6Constraint = std::dynamic_pointer_cast<Dof6SpringConstraint>(constraint);
    if (dof6Constraint == nullptr)
        return;

    // Linear constraints
    auto linearGroup = constraintGroup->createWidget<Group>("Linear Constraints", true);
    {
        // Lower limit
        std::array lowerLimit = { dof6Constraint->getLinearLowerLimit().x(), dof6Constraint->getLinearLowerLimit().y(), dof6Constraint->getLinearLowerLimit().z() };
        linearGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearLowerLimit({ val[0], val[1], val[2] });
            });

        // Upper limit
        std::array upperLimit = { dof6Constraint->getLinearUpperLimit().x(), dof6Constraint->getLinearUpperLimit().y(), dof6Constraint->getLinearUpperLimit().z() };
        linearGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearUpperLimit({ val[0], val[1], val[2] });
            });

        // Velocity target
        std::array targetVelocity = { dof6Constraint->getLinearTargetVelocity().x(), dof6Constraint->getLinearTargetVelocity().y(), dof6Constraint->getLinearTargetVelocity().z() };
        linearGroup->createWidget<Drag<float, 3>>("Target Velocity", ImGuiDataType_Float, targetVelocity, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearTargetVelocity({ val[0], val[1], val[2] });
            });

        // Bounce
        std::array bounce = { dof6Constraint->getLinearBounce().x(), dof6Constraint->getLinearBounce().y(), dof6Constraint->getLinearBounce().z() };
        linearGroup->createWidget<Drag<float, 3>>("Bounce", ImGuiDataType_Float, bounce, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearBounce({ val[0], val[1], val[2] });
            });

        linearGroup->createWidget<Separator>();
        // Spring
        std::array spring = { dof6Constraint->getLinearSpringEnabled().x(), dof6Constraint->getLinearSpringEnabled().y(), dof6Constraint->getLinearSpringEnabled().z() };
        linearGroup->createWidget<Drag<float, 3>>("Enable Spring", ImGuiDataType_Float, spring, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearSpringEnabled({ val[0], val[1], val[2] });
            });

        // Stiffness
        std::array stiffness = { dof6Constraint->getLinearStiffness().x(), dof6Constraint->getLinearStiffness().y(), dof6Constraint->getLinearStiffness().z() };
        linearGroup->createWidget<Drag<float, 3>>("Stiffness", ImGuiDataType_Float, stiffness, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearStiffness({ val[0], val[1], val[2] });
            });

        // Damping
        std::array damping = { dof6Constraint->getLinearDamping().x(), dof6Constraint->getLinearDamping().y(), dof6Constraint->getLinearDamping().z() };
        linearGroup->createWidget<Drag<float, 3>>("Damping", ImGuiDataType_Float, damping, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearDamping({ val[0], val[1], val[2] });
            });

        linearGroup->createWidget<Separator>();
        // Motor
        std::array motor = { dof6Constraint->getLinearMotorEnabled().x(), dof6Constraint->getLinearMotorEnabled().y(), dof6Constraint->getLinearMotorEnabled().z() };
        linearGroup->createWidget<Drag<float, 3>>("Enable Motor", ImGuiDataType_Float, motor, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearMotorEnabled({ val[0], val[1], val[2] });
            });

        // Max motor force
        std::array motorMaxForce = { dof6Constraint->getLinearMaxMotorForce().x(), dof6Constraint->getLinearMaxMotorForce().y(), dof6Constraint->getLinearMaxMotorForce().z() };
        linearGroup->createWidget<Drag<float, 3>>("Max Motor Force", ImGuiDataType_Float, motorMaxForce, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearMaxMotorForce({ val[0], val[1], val[2] });
            });

        // Servo
        std::array servo = { dof6Constraint->getLinearServoEnabled().x(), dof6Constraint->getLinearServoEnabled().y(), dof6Constraint->getLinearServoEnabled().z() };
        linearGroup->createWidget<Drag<float, 3>>("Enable Servo", ImGuiDataType_Float, servo, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearServoEnabled({ val[0], val[1], val[2] });
            });

        // Servo target
        std::array servoTarget = { dof6Constraint->getLinearServoTarget().x(), dof6Constraint->getLinearServoTarget().y(), dof6Constraint->getLinearServoTarget().z() };
        linearGroup->createWidget<Drag<float, 3>>("Servo Target", ImGuiDataType_Float, servoTarget, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setLinearServoTarget({ val[0], val[1], val[2] });
            });
    }


    // Angular constraints
    auto angularGroup = constraintGroup->createWidget<Group>("Angular Constraints", true);
    {
        // Lower limit
        std::array lowerLimit = { dof6Constraint->getAngularLowerLimit().x(), dof6Constraint->getAngularLowerLimit().y(), dof6Constraint->getAngularLowerLimit().z() };
        angularGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularLowerLimit({ val[0], val[1], val[2] });
            });

        // Upper limit
        std::array upperLimit = { dof6Constraint->getAngularUpperLimit().x(), dof6Constraint->getAngularUpperLimit().y(), dof6Constraint->getAngularUpperLimit().z() };
        angularGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularUpperLimit({ val[0], val[1], val[2] });
            });

        // Velocity target
        std::array targetVelocity = { dof6Constraint->getAngularTargetVelocity().x(), dof6Constraint->getAngularTargetVelocity().y(), dof6Constraint->getAngularTargetVelocity().z() };
        angularGroup->createWidget<Drag<float, 3>>("Target Velocity", ImGuiDataType_Float, targetVelocity, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularTargetVelocity({ val[0], val[1], val[2] });
            });

        // Bounce
        std::array bounce = { dof6Constraint->getAngularBounce().x(), dof6Constraint->getAngularBounce().y(), dof6Constraint->getAngularBounce().z() };
        angularGroup->createWidget<Drag<float, 3>>("Bounce", ImGuiDataType_Float, bounce, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularBounce({ val[0], val[1], val[2] });
            });

        angularGroup->createWidget<Separator>();
        // Spring
        std::array spring = { dof6Constraint->getAngularSpringEnabled().x(), dof6Constraint->getAngularSpringEnabled().y(), dof6Constraint->getAngularSpringEnabled().z() };
        angularGroup->createWidget<Drag<float, 3>>("Enable Spring", ImGuiDataType_Float, spring, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularSpringEnabled({ val[0], val[1], val[2] });
            });

        // Stiffness
        std::array stiffness = { dof6Constraint->getAngularStiffness().x(), dof6Constraint->getAngularStiffness().y(), dof6Constraint->getAngularStiffness().z() };
        angularGroup->createWidget<Drag<float, 3>>("Stiffness", ImGuiDataType_Float, stiffness, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularStiffness({ val[0], val[1], val[2] });
            });

        // Damping
        std::array damping = { dof6Constraint->getAngularDamping().x(), dof6Constraint->getAngularDamping().y(), dof6Constraint->getAngularDamping().z() };
        angularGroup->createWidget<Drag<float, 3>>("Damping", ImGuiDataType_Float, damping, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularDamping({ val[0], val[1], val[2] });
            });

        angularGroup->createWidget<Separator>();
        // Motor
        std::array motor = { dof6Constraint->getAngularMotorEnabled().x(), dof6Constraint->getAngularMotorEnabled().y(), dof6Constraint->getAngularMotorEnabled().z() };
        angularGroup->createWidget<Drag<float, 3>>("Enable Motor", ImGuiDataType_Float, motor, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularMotorEnabled({ val[0], val[1], val[2] });
            });

        // Max motor force
        std::array motorMaxForce = { dof6Constraint->getAngularMaxMotorForce().x(), dof6Constraint->getAngularMaxMotorForce().y(), dof6Constraint->getAngularMaxMotorForce().z() };
        angularGroup->createWidget<Drag<float, 3>>("Max Motor Force", ImGuiDataType_Float, motorMaxForce, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularMaxMotorForce({ val[0], val[1], val[2] });
            });

        // Servo
        std::array servo = { dof6Constraint->getAngularServoEnabled().x(), dof6Constraint->getAngularServoEnabled().y(), dof6Constraint->getAngularServoEnabled().z() };
        angularGroup->createWidget<Drag<float, 3>>("Enable Servo", ImGuiDataType_Float, servo, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularServoEnabled({ val[0], val[1], val[2] });
            });

        // Servo target
        std::array servoTarget = { dof6Constraint->getAngularServoTarget().x(), dof6Constraint->getAngularServoTarget().y(), dof6Constraint->getAngularServoTarget().z() };
        angularGroup->createWidget<Drag<float, 3>>("Servo Target", ImGuiDataType_Float, servoTarget, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
            dof6Constraint->setAngularServoTarget({ val[0], val[1], val[2] });
            });
    }
}
NS_IGE_END