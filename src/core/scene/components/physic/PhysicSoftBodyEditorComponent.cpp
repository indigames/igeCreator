#include "core/scene/components/physic/PhysicSoftBodyEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicSoftBody.h"
#include "components/FigureComponent.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>
NS_IGE_BEGIN

PhysicSoftBodyEditorComponent::PhysicSoftBodyEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicSoftBodyEditorComponent::~PhysicSoftBodyEditorComponent() {
    m_physicGroup = nullptr;
}

void PhysicSoftBodyEditorComponent::onInspectorUpdate() {
    drawPhysicSoftBody();
}

void PhysicSoftBodyEditorComponent::drawPhysicSoftBody()
{
    drawPhysicObject();

    // Draw physic box properties
    m_physicGroup->createWidget<Separator>();
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto columns = m_physicGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("SelfCollision", comp->getProperty<bool>("isSelfCol", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("isSelfCol", val);
    });

    columns->createWidget<CheckBox>("SoftCollision", comp->getProperty<bool>("isSoftCol", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("isSoftCol", val);
    });

    std::array dampCoeff = { comp->getProperty<float>("dampCoeff", NAN) };
    m_physicGroup->createWidget<Drag<float>>("DampingCoeff", ImGuiDataType_Float, dampCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("dampCoeff", val[0]);
    });

    std::array repStiff = { comp->getProperty<float>("repStiff", NAN) };
    m_physicGroup->createWidget<Drag<float>>("LinearStiffness", ImGuiDataType_Float, repStiff, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("repStiff", val[0]);
    });

    std::array presCoeff = { comp->getProperty<float>("presCoeff", NAN) };
    m_physicGroup->createWidget<Drag<float>>("PressureCoeff", ImGuiDataType_Float, presCoeff, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("presCoeff", val[0]);
    });

    std::array volCoeff = { comp->getProperty<float>("volCoeff", NAN) };
    m_physicGroup->createWidget<Drag<float>>("VolumeConvCoeff", ImGuiDataType_Float, volCoeff, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("volCoeff", val[0]);
    });

    std::array friCoeff = { comp->getProperty<float>("friCoeff", NAN) };
    m_physicGroup->createWidget<Drag<float>>("FrictionCoeff", ImGuiDataType_Float, friCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("friCoeff", val[0]);
    });

    std::array poseCoeff = { comp->getProperty<float>("poseCoeff", NAN) };
    m_physicGroup->createWidget<Drag<float>>("PoseMatchCoeff", ImGuiDataType_Float, poseCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("poseCoeff", val[0]);
    });

    std::array graF = { comp->getProperty<float>("graF", NAN) };
    m_physicGroup->createWidget<Drag<float>>("GravityFactor", ImGuiDataType_Float, graF, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("graF", val[0]);
    });

    std::array velF = { comp->getProperty<float>("velF", NAN) };
    m_physicGroup->createWidget<Drag<float>>("VelocityCorFactor", ImGuiDataType_Float, velF, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("velF", val[0]);
    });

    std::array rch = { comp->getProperty<float>("rch", NAN) };
    m_physicGroup->createWidget<Drag<float>>("RigidHardness", ImGuiDataType_Float, rch, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("rch", val[0]);
    });

    std::array kch = { comp->getProperty<float>("kch", NAN) };
    m_physicGroup->createWidget<Drag<float>>("KineticHardness", ImGuiDataType_Float, kch, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("kch", val[0]);
    });

    std::array sch = { comp->getProperty<float>("sch", NAN) };
    m_physicGroup->createWidget<Drag<float>>("SoftHardness", ImGuiDataType_Float, sch, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("sch", val[0]);
    });

    std::array ahr = { comp->getProperty<float>("ahr", NAN) };
    m_physicGroup->createWidget<Drag<float>>("AnchorHardness", ImGuiDataType_Float, ahr, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("ahr", val[0]);
    });

    std::array pItrNum = { comp->getProperty<float>("pItrNum", NAN) };
    m_physicGroup->createWidget<Drag<float>>("PosIterations", ImGuiDataType_S32, pItrNum, 1, 1)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("pItrNum", (int)val[0]);
    });

    std::array sleepThr = { comp->getProperty<float>("sleepThr", NAN) };
    m_physicGroup->createWidget<Drag<float>>("SleepThreshold", ImGuiDataType_Float, sleepThr, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("sleepThr", val[0]);
    });

    std::array restLS = { comp->getProperty<float>("restLS", NAN) };
    m_physicGroup->createWidget<Drag<float>>("RestLengthScale", ImGuiDataType_Float, restLS, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("restLS", val[0]);
    });

    std::array aero = { comp->getProperty<float>("aero", NAN) };
    m_physicGroup->createWidget<Drag<float>>("AeroModel", ImGuiDataType_S32, aero, 1, 0, 6)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("aero", (int)val[0]);
    });

    auto windVel = comp->getProperty<Vec3>("windVel", { NAN, NAN, NAN });
    std::array windVelocity = { windVel.X(), windVel.Y(), windVel.Z() };
    m_physicGroup->createWidget<Drag<float, 3>>("WindVelocity", ImGuiDataType_Float, windVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("windVel", val[0]);
    });

    // Only show if single target
    if (comp->size() == 1)
    {
        int maxMeshIdx = 0;
        auto physicComp = std::dynamic_pointer_cast<PhysicSoftBody>(comp->getComponents()[0]);
        auto figureComp = physicComp->getOwner()->getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure())
            maxMeshIdx = figureComp->getFigure()->NumMeshes() - 1;

        std::array meshIdx = { comp->getProperty<float>("meshIdx", NAN) };
        auto meshIdxWg = m_physicGroup->createWidget<Drag<float>>("Mesh Index", ImGuiDataType_S32, meshIdx, 1, 0, maxMeshIdx);
        meshIdxWg->getOnDataChangedEvent().addListener([this](auto& val) {
            getComponent<CompoundComponent>()->setProperty("meshIdx", (int)val[0]);
        });
        meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("meshIdx", val);
            redraw();
        });
    }
    
    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END