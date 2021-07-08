#include "core/scene/components/physic/PhysicObjectEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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

PhysicObjectEditorComponent::~PhysicObjectEditorComponent() {
    m_constraintGroup = nullptr;
    m_physicGroup = nullptr;
}

void PhysicObjectEditorComponent::onInspectorUpdate() {
    drawPhysicObject();
}

void PhysicObjectEditorComponent::drawPhysicObject() {
    if (m_physicGroup == nullptr)
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);;
    m_physicGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto columns = m_physicGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("Continous", comp->getProperty<bool>("ccd", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("ccd", val);
    });

    columns->createWidget<CheckBox>("Kinematic", comp->getProperty<bool>("isKinematic", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("isKinematic", val);
        setDirty();
    });

    columns->createWidget<CheckBox>("Trigger", comp->getProperty<bool>("isTrigger", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("isTrigger", val);
    });

    std::array filterGroup = { comp->getProperty<int>("group", 0) };
    m_physicGroup->createWidget<Drag<int>>("Collision Group", ImGuiDataType_S32, filterGroup, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("group", val[0]);
    });

    std::array filterMask = { comp->getProperty<int>("mask", 0) };
    m_physicGroup->createWidget<Drag<int>>("Collision Mask", ImGuiDataType_S32, filterMask, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("mask", val[0]);
    });

    std::array mass = { comp->getProperty<float>("mass", 1.f) };
    m_physicGroup->createWidget<Drag<float>>("Mass", ImGuiDataType_Float, mass, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("mass", val[0]);
    });

    std::array friction = { comp->getProperty<float>("friction", 0.5f) };
    m_physicGroup->createWidget<Drag<float>>("Friction", ImGuiDataType_Float, friction, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("friction", val[0]);
    });

    std::array restitution = { comp->getProperty<float>("restitution", 1.f) };
    m_physicGroup->createWidget<Drag<float>>("Restitution", ImGuiDataType_Float, restitution, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("restitution", val[0]);
    });

    auto lVel = comp->getProperty<Vec3>("linearVelocity", {});
    std::array linearVelocity = { lVel.X(), lVel.Y(), lVel.Z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Linear Velocity", ImGuiDataType_Float, linearVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("linearVelocity", { val[0], val[1], val[2] });
    });

    auto aVel = comp->getProperty<Vec3>("angularVelocity", {});
    std::array angularVelocity = { aVel.X(), aVel.Y(), aVel.Z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Angular Velocity", ImGuiDataType_Float, angularVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("angularVelocity", { val[0], val[1], val[2] });
    });

    auto lFactor = comp->getProperty<Vec3>("linearFactor", {});
    std::array linearFactor = { lFactor.X(), lFactor.Y(), lFactor.Z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Linear Factor", ImGuiDataType_Float, linearFactor)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("linearFactor", { val[0], val[1], val[2] });
    });

    auto aFactor = comp->getProperty<Vec3>("angularFactor", {});
    std::array angularFactor = { aFactor.X(), aFactor.Y(), aFactor.Z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Angular Factor", ImGuiDataType_Float, angularFactor)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("angularFactor", { val[0], val[1], val[2] });
    });

    std::array margin = { comp->getProperty<float>("margin", 0.025f) };
    m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("margin", val[0]);
    });
}

void PhysicObjectEditorComponent::drawPhysicConstraints() {
    // Only inspect single target
    if (getComponent<CompoundComponent>()->empty()
        || getComponent<CompoundComponent>()->size() > 1) {
        return;
    }

    if (m_constraintGroup != nullptr) {
        m_constraintGroup->removeAllWidgets();
        m_constraintGroup = nullptr;
    }
    m_constraintGroup = m_physicGroup->createWidget<Group>("ConstraintGroup");

    // Create constraint selection
    m_constraintCreateCombo = m_constraintGroup->createWidget<ComboBox>("Constraint", 0, true, false);
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Fixed, "Fixed Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Hinge, "Hinge Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Slider, "Slider Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Spring, "Spring Constraint");
    m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Dof6Spring, "Dof6-Spring Constraint");

    // Add button
    auto createCompButton = m_constraintGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
    createCompButton->getOnClickEvent().addListener([this](auto widget) {
        auto comp = std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0]);
        switch (m_constraintCreateCombo->getSelectedIndex())
        {
        case (int)PhysicConstraint::ConstraintType::Fixed:
            comp->addConstraint<FixedConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Hinge:
            comp->addConstraint<HingeConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Slider:
            comp->addConstraint<SliderConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Spring:
            comp->addConstraint<SpringConstraint>();
            break;
        case (int)PhysicConstraint::ConstraintType::Dof6Spring:
            comp->addConstraint<Dof6SpringConstraint>();
            break;
        }
        setDirty();
    });
    m_constraintGroup->createWidget<Separator>();

    auto physicComp = std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0]);    
    if (physicComp)
    {
        for (auto constraint : physicComp->getContraints())
        {
            if (constraint->getType() == PhysicConstraint::ConstraintType::Fixed)
                drawFixedConstraint(constraint);
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Hinge)
                drawHingeConstraint(constraint);
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Slider)
                drawSliderConstraint(constraint);
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Spring)
                drawSpringConstraint(constraint);
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Dof6Spring)
                drawDof6SpringConstraint(constraint);
        }
    }    
}

//! Draw Physic Constraint
void PhysicObjectEditorComponent::drawPhysicConstraint(std::shared_ptr<PhysicConstraint> constraint, std::shared_ptr<Group> constraintGroup)
{
    auto columns = constraintGroup->createWidget<Columns<2>>();

    // Enabled
    columns->createWidget<CheckBox>("Enable", constraint->isEnabled())->getOnDataChangedEvent().addListener([constraint](bool val) {
        constraint->setEnabled(val);
    });

    // Bodies Collision
    columns->createWidget<CheckBox>("Bodies Collision", constraint->isEnableCollisionBetweenBodies())->getOnDataChangedEvent().addListener([constraint](bool val) {
        constraint->setEnableCollisionBetweenBodies(val);
    });

    // Other body
    auto otherBodyTxt = constraintGroup->createWidget<TextField>("Other body", constraint->getOther() ? constraint->getOther()->getOwner()->getName() : std::string());
    otherBodyTxt->getOnDataChangedEvent().addListener([&](const auto& val) {
        auto obj = Editor::getCurrentScene()->findObjectByName(val);
        constraint->setOtherUUID(obj->getUUID());
    });
    otherBodyTxt->addPlugin<DDTargetPlugin<int>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([constraint, this](auto val) {
        auto obj = Editor::getCurrentScene()->findObjectById(val);
        constraint->setOtherUUID(obj->getUUID());
        setDirty();
    });

    // Breaking impulse
    std::array breakImpulse = { constraint->getBreakingImpulseThreshold() };
    constraintGroup->createWidget<Drag<float>>("Break Impulse", ImGuiDataType_Float, breakImpulse)->getOnDataChangedEvent().addListener([constraint](const auto& val) {
        constraint->setBreakingImpulseThreshold(val[0]);
    });
}

//! Draw Fixed Constraint
void PhysicObjectEditorComponent::drawFixedConstraint(std::shared_ptr<PhysicConstraint> constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("FixedConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([constraint, this]() {
        std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0])->removeConstraint(constraint);
        setDirty();
    });

    // Draw Physic Constraint base
    drawPhysicConstraint(constraint, constraintGroup);
}

//! draw Hinge Constraint
void PhysicObjectEditorComponent::drawHingeConstraint(std::shared_ptr<PhysicConstraint> constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("HingeConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([constraint, this]() {
        std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0])->removeConstraint(constraint);
        setDirty();
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
void PhysicObjectEditorComponent::drawSliderConstraint(std::shared_ptr<PhysicConstraint> constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("SliderConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([constraint, this]() {
        std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0])->removeConstraint(constraint);
        setDirty();
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
void PhysicObjectEditorComponent::drawSpringConstraint(std::shared_ptr<PhysicConstraint> constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("SpringConstraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([constraint, this]() {
        std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0])->removeConstraint(constraint);
        setDirty();
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
void PhysicObjectEditorComponent::drawDof6SpringConstraint(std::shared_ptr<PhysicConstraint> constraint)
{
    auto constraintGroup = m_constraintGroup->createWidget<Group>("Dof6Constraint", true, true);
    constraintGroup->getOnClosedEvent().addListener([constraint, this]() {
        std::dynamic_pointer_cast<PhysicObject>(getComponent<CompoundComponent>()->getComponents()[0])->removeConstraint(constraint);
        setDirty();
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