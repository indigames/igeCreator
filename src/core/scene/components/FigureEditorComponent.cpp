#include "core/scene/components/FigureEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "components/FigureComponent.h"

#include <core/layout/Group.h>
#include "core/layout/Columns.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

FigureEditorComponent::FigureEditorComponent() {
    m_figureCompGroup = nullptr;
}

FigureEditorComponent::~FigureEditorComponent() {
    m_figureCompGroup = nullptr;
}


void FigureEditorComponent::onInspectorUpdate() {
    drawFigureComponent();
}

void FigureEditorComponent::drawFigureComponent()
{
    if (m_figureCompGroup == nullptr) {
        m_figureCompGroup = m_group->createWidget<Group>("FigureGroup", false);
    }
    m_figureCompGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtPath = m_figureCompGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""), false, true);
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](const auto& txt) {
        getComponent<CompoundComponent>()->setProperty("path", txt);
        setDirty();
    });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& txt) {
            getComponent<CompoundComponent>()->setProperty("path", txt);
            setDirty();
        });
    }

    m_figureCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Figure", "*.dae", "FBX file (*.fbx)", "*.fbx" }).result();
        if (files.size() > 0) {
            getComponent<CompoundComponent>()->setProperty("path", files[0]);
            setDirty();
        }
    });

    auto figColumn = m_figureCompGroup->createWidget<Columns<2>>();
    figColumn->createWidget<CheckBox>("Fog", comp->getProperty<bool>("fog", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("fog", val);
        setDirty();
    });

    figColumn->createWidget<CheckBox>("DoubleSide", comp->getProperty<bool>("doubleSide", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("doubleSide", val);
        setDirty();
    });

    figColumn->createWidget<CheckBox>("FFCulling", comp->getProperty<bool>("cull", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("cull", val);
        setDirty();
    });

    figColumn->createWidget<CheckBox>("Z-Test", comp->getProperty<bool>("zTest", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("zTest", val);
        setDirty();
    });

    figColumn->createWidget<CheckBox>("Z-Write", comp->getProperty<bool>("zWrite", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("zWrite", val);
        setDirty();
    });

    figColumn->createWidget<CheckBox>("ScissorTest", comp->getProperty<bool>("scissor", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("scissor", val);
        setDirty();
    });

    figColumn->createWidget<CheckBox>("AlphaBlend", comp->getProperty<bool>("aBlend", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("aBlend", val);
        setDirty();
    });

    if (comp->getProperty<bool>("aBlend", false))
    {
        auto aBlendOp = comp->getProperty<int>("aBlendOp", -1);
        auto alphaCombo = m_figureCompGroup->createWidget<ComboBox>("Blend OP", aBlendOp);
        alphaCombo->getOnDataChangedEvent().addListener([this](auto val) {
            if (val != -1) {
                getComponent<CompoundComponent>()->setProperty("aBlendOp", val);
                setDirty();
            }
        });
        alphaCombo->setEndOfLine(false);
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::COL, "COL");
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::ADD, "ADD");
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::SUB, "SUB");
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::MUL, "MUL");
        if(aBlendOp == -1) alphaCombo->addChoice((int)-1, "");
        alphaCombo->setEndOfLine(true);
    }

    // Only show details if this is single target
    if (comp->size() == 1)
    {
        auto figureComp = std::dynamic_pointer_cast<FigureComponent>(comp->getComponents()[0]);
        if (figureComp)
        {
            auto figure = figureComp->getFigure();
            if (figure != nullptr) {
                if (figure->NumMeshes() > 0)
                {
                    auto meshGroup = m_figureCompGroup->createWidget<Group>("Mesh", true, false, Group::E_Align::LEFT, false);
                    auto meshColumn = meshGroup->createWidget<Columns<4>>();
                    for (int i = 0; i < figure->NumMeshes(); i++)
                    {
                        auto mesh = figure->GetMesh(i);
                        auto txtName = meshColumn->createWidget<TextField>("", figure->GetMeshName(i), true);
                        txtName->addPlugin<DDSourcePlugin<int>>(EDragDropID::MESH, figure->GetMeshName(i), i);
                        meshColumn->createWidget<TextField>("V", std::to_string(mesh->numVerticies).c_str(), true);
                        meshColumn->createWidget<TextField>("I", std::to_string(mesh->numIndices).c_str(), true);
                        meshColumn->createWidget<CheckBox>("", figureComp->isMeshEnable(i))->getOnDataChangedEvent().addListener([this, i](auto val) {
                            auto figureComp = std::dynamic_pointer_cast<FigureComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
                            if (figureComp) {
                                figureComp->setMeshEnable(i, val);
                            }
                        });
                    }
                }

                if (figure->NumMaterials() > 0)
                {
                    auto materialGroup = m_figureCompGroup->createWidget<Group>("Material", true, false, Group::E_Align::LEFT, false);
                    for (int i = 0; i < figure->NumMaterials(); i++)
                    {
                        const char* matName = figure->GetMaterialName(i);
                        if (matName)
                        {
                            materialGroup->createWidget<Label>(matName);
                            int index = figure->GetMaterialIndex(GenerateNameHash(matName));
                            auto currMat = figure->GetMaterial(i);

                            for (auto j = 0; j < currMat->numParams; j++)
                            {
                                auto info = RenderContext::Instance().GetShaderParameterInfoByHash(currMat->params[j].hash);
                                if (!info)
                                    continue;

                                auto infoName = info->name;
                                if (currMat->params[j].type == ParamTypeSampler)
                                {
                                    materialGroup->createWidget<TextField>(info->name, currMat->params[j].sampler.tex->ResourceName());
                                }
                                else if (currMat->params[j].type == ParamTypeBool)
                                {
                                    std::array val = { currMat->params[j].fValue[0] };
                                    materialGroup->createWidget<Drag<float>>(info->name, ImGuiDataType_S32, val, 1, 0, 1);
                                }
                                else if (currMat->params[j].type == ParamTypeInt)
                                {
                                    std::array val = { currMat->params[j].fValue[0] };
                                    materialGroup->createWidget<Drag<float>>(info->name, ImGuiDataType_S32, val);
                                }
                                else if (currMat->params[j].type == ParamTypeFloat)
                                {
                                    std::array val = { currMat->params[j].fValue[0] };
                                    materialGroup->createWidget<Drag<float>>(info->name, ImGuiDataType_Float, val);
                                }
                                else if (currMat->params[j].type == ParamTypeFloat2)
                                {
                                    std::array val = { currMat->params[j].fValue[0], currMat->params[j].fValue[1] };
                                    materialGroup->createWidget<Drag<float, 2>>(info->name, ImGuiDataType_Float, val);
                                }
                                else if (currMat->params[j].type == ParamTypeFloat3)
                                {
                                    std::array val = { currMat->params[j].fValue[0], currMat->params[j].fValue[1], currMat->params[j].fValue[2] };
                                    materialGroup->createWidget<Drag<float, 3>>(info->name, ImGuiDataType_Float, val);
                                }
                                else if (currMat->params[j].type == ParamTypeFloat4)
                                {
                                    auto color = Vec4(currMat->params[j].fValue[0], currMat->params[j].fValue[1], currMat->params[j].fValue[2], currMat->params[j].fValue[3]);
                                    materialGroup->createWidget<Color>(info->name, color);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
NS_IGE_END