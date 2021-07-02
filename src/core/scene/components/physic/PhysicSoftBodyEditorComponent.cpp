#include "core/scene/components/physic/PhysicSoftBodyEditorComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicSoftBody.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>
NS_IGE_BEGIN

PhysicSoftBodyEditorComponent::PhysicSoftBodyEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicSoftBodyEditorComponent::~PhysicSoftBodyEditorComponent()
{
    m_physicGroup = nullptr;
}

void PhysicSoftBodyEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicGroup == nullptr) {
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);
    }
    drawPhysicSoftBody();

    EditorComponent::redraw();
}

void PhysicSoftBodyEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);

    drawPhysicSoftBody();
}

void PhysicSoftBodyEditorComponent::drawPhysicSoftBody()
{
    if (m_physicGroup == nullptr)
        return;
    m_physicGroup->removeAllWidgets();

    auto physicComp = getComponent<PhysicSoftBody>();
    if (physicComp == nullptr)
        return;

    int maxMeshIdx = 0;
    auto figureComp = physicComp->getOwner()->getComponent<FigureComponent>();
    if (figureComp && figureComp->getFigure())
        maxMeshIdx = figureComp->getFigure()->NumMeshes() - 1;
    std::array meshIdx = { physicComp->getMeshIndex() };
    auto meshIdxWg = m_physicGroup->createWidget<Drag<int>>("Mesh Index", ImGuiDataType_S32, meshIdx, 1, 0, maxMeshIdx);
    meshIdxWg->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setMeshIndex(val[0]);
        });
    meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setMeshIndex(val);
        redraw();
        });

    m_physicGroup->createWidget<Separator>();
    auto columns = m_physicGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("Enable", physicComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setEnabled(val);
        });

    columns->createWidget<CheckBox>("Continous", physicComp->isCCD())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setCCD(val);
        });

    columns->createWidget<CheckBox>("Trigger", physicComp->isTrigger())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setIsTrigger(val);
        });

    columns->createWidget<CheckBox>("Self Collision", physicComp->isSelfCollision())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setSelfCollision(val);
        });

    columns->createWidget<CheckBox>("Soft Collision", physicComp->isSoftSoftCollision())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setSoftSoftCollision(val);
        });

    std::array filterGroup = { physicComp->getCollisionFilterGroup() };
    m_physicGroup->createWidget<Drag<int>>("Collision Group", ImGuiDataType_S32, filterGroup, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setCollisionFilterGroup(val[0]);
        });

    std::array filterMask = { physicComp->getCollisionFilterMask() };
    m_physicGroup->createWidget<Drag<int>>("Collision Mask", ImGuiDataType_S32, filterMask, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setCollisionFilterMask(val[0]);
        });

    std::array mass = { physicComp->getMass() };
    m_physicGroup->createWidget<Drag<float>>("Mass", ImGuiDataType_Float, mass, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setMass(val[0]);
        });

    std::array friction = { physicComp->getFriction() };
    m_physicGroup->createWidget<Drag<float>>("Friction", ImGuiDataType_Float, friction, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setFriction(val[0]);
        });

    std::array restitution = { physicComp->getRestitution() };
    m_physicGroup->createWidget<Drag<float>>("Restitution", ImGuiDataType_Float, restitution, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setRestitution(val[0]);
        });

    std::array linearVelocity = { physicComp->getLinearVelocity().x(), physicComp->getLinearVelocity().y(), physicComp->getLinearVelocity().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Linear Velocity", ImGuiDataType_Float, linearVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setLinearVelocity({ val[0], val[1], val[2] });
        });

    std::array angularVelocity = { physicComp->getAngularVelocity().x(), physicComp->getAngularVelocity().y(), physicComp->getAngularVelocity().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Angular Velocity", ImGuiDataType_Float, angularVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setAngularVelocity({ val[0], val[1], val[2] });
        });

    std::array margin = { physicComp->getCollisionMargin() };
    m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setCollisionMargin(val[0]);
        });

    m_physicGroup->createWidget<Separator>();
    std::array damping = { physicComp->getDampingCoeff() };
    m_physicGroup->createWidget<Drag<float>>("DampingCoeff", ImGuiDataType_Float, damping, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setDampingCoeff(val[0]);
        });

    std::array spring = { physicComp->getRepulsionStiffness() };
    m_physicGroup->createWidget<Drag<float>>("LinearStiffness", ImGuiDataType_Float, spring, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setRepulsionStiffness(val[0]);
        });

    std::array pressure = { physicComp->getPressureCoeff() };
    m_physicGroup->createWidget<Drag<float>>("PressureCoeff", ImGuiDataType_Float, pressure, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setPressureCoeff(val[0]);
        });

    std::array volCoeff = { physicComp->getVolumeConvCoeff() };
    m_physicGroup->createWidget<Drag<float>>("VolumeConvCoeff", ImGuiDataType_Float, volCoeff, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setVolumeConvCoeff(val[0]);
        });

    std::array fricCoeff = { physicComp->getDynamicFrictionCoeff() };
    m_physicGroup->createWidget<Drag<float>>("FrictionCoeff", ImGuiDataType_Float, fricCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setDynamicFrictionCoeff(val[0]);
        });

    std::array poseCoeff = { physicComp->getPoseMatchCoeff() };
    m_physicGroup->createWidget<Drag<float>>("PoseMatchCoeff", ImGuiDataType_Float, poseCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setPoseMatchCoeff(val[0]);
        });

    std::array graF = { physicComp->getGravityFactor() };
    m_physicGroup->createWidget<Drag<float>>("GravityFactor", ImGuiDataType_Float, graF, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setGravityFactor(val[0]);
        });

    std::array velF = { physicComp->getVelocityFactor() };
    m_physicGroup->createWidget<Drag<float>>("VelocityCorFactor", ImGuiDataType_Float, velF, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setVelocityFactor(val[0]);
        });

    std::array rigidH = { physicComp->getRigidContactHardness() };
    m_physicGroup->createWidget<Drag<float>>("RigidHardness", ImGuiDataType_Float, rigidH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setRigidContactHardness(val[0]);
        });

    std::array kinH = { physicComp->getKineticContactHardness() };
    m_physicGroup->createWidget<Drag<float>>("KineticHardness", ImGuiDataType_Float, kinH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setKineticContactHardness(val[0]);
        });

    std::array softH = { physicComp->getSoftContactHardness() };
    m_physicGroup->createWidget<Drag<float>>("SoftHardness", ImGuiDataType_Float, softH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setSoftContactHardness(val[0]);
        });

    std::array anchorH = { physicComp->getAnchorHardness() };
    m_physicGroup->createWidget<Drag<float>>("AnchorHardness", ImGuiDataType_Float, anchorH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setAnchorHardness(val[0]);
        });

    std::array posItrNum = { physicComp->getPosIterationNumber() };
    m_physicGroup->createWidget<Drag<int>>("PosIterations", ImGuiDataType_S32, posItrNum, 1, 1)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setPosIterationNumber(val[0]);
        });

    std::array sleepThreshold = { physicComp->getSleepingThreshold() };
    m_physicGroup->createWidget<Drag<float>>("SleepThreshold", ImGuiDataType_Float, sleepThreshold, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setSleepingThreshold(val[0]);
        });

    std::array restLS = { physicComp->getRestLengthScale() };
    m_physicGroup->createWidget<Drag<float>>("RestLengthScale", ImGuiDataType_Float, restLS, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setRestLengthScale(val[0]);
        });

    std::array aeroModel = { physicComp->getAeroModel() };
    m_physicGroup->createWidget<Drag<int>>("AeroModel", ImGuiDataType_S32, aeroModel, 1, 0, 6)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setAeroModel(val[0]);
        });

    std::array windVelocity = { physicComp->getWindVelocity().x(), physicComp->getWindVelocity().y(), physicComp->getWindVelocity().z() };
    m_physicGroup->createWidget<Drag<float, 3>>("WindVelocity", ImGuiDataType_Float, windVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicSoftBody>();
        physicComp->setWindVelocity({ val[0], val[1], val[2] });
        });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END