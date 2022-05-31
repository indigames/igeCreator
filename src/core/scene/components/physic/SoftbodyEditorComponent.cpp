#include "core/scene/components/physic/SoftbodyEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/Softbody.h"
#include "components/FigureComponent.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>
NS_IGE_BEGIN

SoftbodyEditorComponent::SoftbodyEditorComponent() {
    m_physicGroup = nullptr;
}

SoftbodyEditorComponent::~SoftbodyEditorComponent() {
    m_physicGroup = nullptr;
}

void SoftbodyEditorComponent::onInspectorUpdate() {    
    // Draw inspector
    drawSoftbody();

    // Draw constraints
    drawPhysicConstraints();
}

void SoftbodyEditorComponent::drawSoftbody()
{
    drawRigidbody();

    // Draw physic box properties
    m_physicGroup->createWidget<Separator>();
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto columns = m_physicGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("SelfCollision", comp->getProperty<bool>("isSelfCol", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("isSelfCol", val);
    });

    columns->createWidget<CheckBox>("SoftCollision", comp->getProperty<bool>("isSoftCol", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("isSoftCol", val);
    });

    std::array dampCoeff = { comp->getProperty<float>("dampCoeff", NAN) };
    auto d1 = m_physicGroup->createWidget<Drag<float>>("DampingCoeff", ImGuiDataType_Float, dampCoeff, 0.001f, 0.f, 1.f);
    d1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("dampCoeff", val[0]);
    });

    std::array repStiff = { comp->getProperty<float>("repStiff", NAN) };
    auto d2 = m_physicGroup->createWidget<Drag<float>>("LinearStiffness", ImGuiDataType_Float, repStiff, 0.001f, 0.f);
    d2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d2->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("repStiff", val[0]);
    });

    std::array presCoeff = { comp->getProperty<float>("presCoeff", NAN) };
    auto d3 = m_physicGroup->createWidget<Drag<float>>("PressureCoeff", ImGuiDataType_Float, presCoeff, 0.001f);
    d3->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d3->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("presCoeff", val[0]);
    });

    std::array volCoeff = { comp->getProperty<float>("volCoeff", NAN) };
    auto d4 = m_physicGroup->createWidget<Drag<float>>("VolumeConvCoeff", ImGuiDataType_Float, volCoeff, 0.001f, 0.f);
    d4->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d4->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("volCoeff", val[0]);
    });

    std::array friCoeff = { comp->getProperty<float>("friCoeff", NAN) };
    auto d5 = m_physicGroup->createWidget<Drag<float>>("FrictionCoeff", ImGuiDataType_Float, friCoeff, 0.001f, 0.f, 1.f);
    d5->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d5->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("friCoeff", val[0]);
    });

    std::array poseCoeff = { comp->getProperty<float>("poseCoeff", NAN) };
    auto d6 = m_physicGroup->createWidget<Drag<float>>("PoseMatchCoeff", ImGuiDataType_Float, poseCoeff, 0.001f, 0.f, 1.f);
    d6->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d6->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("poseCoeff", val[0]);
    });

    std::array graF = { comp->getProperty<float>("graF", NAN) };
    auto d7 = m_physicGroup->createWidget<Drag<float>>("GravityFactor", ImGuiDataType_Float, graF, 0.001f, 0.f);
    d7->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d7->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("graF", val[0]);
    });

    std::array velF = { comp->getProperty<float>("velF", NAN) };
    auto d8 = m_physicGroup->createWidget<Drag<float>>("VelocityCorFactor", ImGuiDataType_Float, velF, 0.001f, 0.f);
    d8->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d8->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("velF", val[0]);
    });

    std::array rch = { comp->getProperty<float>("rch", NAN) };
    auto d9 = m_physicGroup->createWidget<Drag<float>>("RigidHardness", ImGuiDataType_Float, rch, 0.001f, 0.f, 1.f);
    d9->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d9->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("rch", val[0]);
    });

    std::array kch = { comp->getProperty<float>("kch", NAN) };
    auto d10 = m_physicGroup->createWidget<Drag<float>>("KineticHardness", ImGuiDataType_Float, kch, 0.001f, 0.f, 1.f);
    d10->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    }); 
    d10->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("kch", val[0]);
    });

    std::array sch = { comp->getProperty<float>("sch", NAN) };
    auto d11 = m_physicGroup->createWidget<Drag<float>>("SoftHardness", ImGuiDataType_Float, sch, 0.001f, 0.f, 1.f);
    d11->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d11->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("sch", val[0]);
    });

    std::array ahr = { comp->getProperty<float>("ahr", NAN) };
    auto d12 = m_physicGroup->createWidget<Drag<float>>("AnchorHardness", ImGuiDataType_Float, ahr, 0.001f, 0.f, 1.f);
    d12->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    }); 
    d12->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("ahr", val[0]);
    });

    std::array pItrNum = { comp->getProperty<float>("pItrNum", NAN) };
    auto d13 = m_physicGroup->createWidget<Drag<float>>("PosIterations", ImGuiDataType_S32, pItrNum, 1, 1);
    d13->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d13->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("pItrNum", (int)val[0]);
    });

    std::array sleepThr = { comp->getProperty<float>("sleepThr", NAN) };
    auto d14 = m_physicGroup->createWidget<Drag<float>>("SleepThreshold", ImGuiDataType_Float, sleepThr, 0.001f, 0.0f);
    d14->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d14->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("sleepThr", val[0]);
    });

    std::array restLS = { comp->getProperty<float>("restLS", NAN) };
    auto d15 = m_physicGroup->createWidget<Drag<float>>("RestLengthScale", ImGuiDataType_Float, restLS, 0.001f, 0.0f);
    d15->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    }); 
    d15->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("restLS", val[0]);
    });

    std::array aero = { comp->getProperty<float>("aero", NAN) };
    auto d16 = m_physicGroup->createWidget<Drag<float>>("AeroModel", ImGuiDataType_S32, aero, 1, 0, 6);
    d16->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d16->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("aero", (int)val[0]);
    });

    auto windVel = comp->getProperty<Vec3>("windVel", { NAN, NAN, NAN });
    std::array windVelocity = { windVel.X(), windVel.Y(), windVel.Z() };
    auto d17 = m_physicGroup->createWidget<Drag<float, 3>>("WindVelocity", ImGuiDataType_Float, windVelocity);
    d17->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d17->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("windVel", val[0]);
    });

    // Only show if single target
    if (comp->size() == 1)
    {
        int maxMeshIdx = 0;
        auto physicComp = std::dynamic_pointer_cast<Softbody>(comp->getComponents()[0]);
        auto figureComp = physicComp->getOwner()->getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure())
            maxMeshIdx = figureComp->getFigure()->NumMeshes() - 1;

        std::array meshIdx = { comp->getProperty<float>("meshIdx", NAN) };
        auto meshIdxWg = m_physicGroup->createWidget<Drag<float>>("Mesh Index", ImGuiDataType_S32, meshIdx, 1, 0, maxMeshIdx);
        meshIdxWg->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
        });
        meshIdxWg->getOnDataChangedEvent().addListener([this](auto& val) {
            getComponent<CompoundComponent>()->setProperty("meshIdx", (int)val[0]);
        });
        meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("meshIdx", val);
            redraw();
        });
    }
}
NS_IGE_END