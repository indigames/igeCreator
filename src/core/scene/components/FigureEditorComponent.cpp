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

    auto txtPath = m_figureCompGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
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
        auto files = OpenFileDialog("Import Assets", "", { "Figure (*.pyxf)", "*.pyxf" }).result();
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

    figColumn->createWidget<CheckBox>("CullFace", comp->getProperty<bool>("cull", false))->getOnDataChangedEvent().addListener([this](bool val) {
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

    figColumn->createWidget<CheckBox>("AlphaBlend", comp->getProperty<bool>("aBlend", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("aBlend", val);
        setDirty();
    });

    if (comp->getProperty<bool>("aBlend", false))
    {
        auto m_alphaCombo = m_figureCompGroup->createWidget<ComboBox>("Blend OP", comp->getProperty<int>("aBlendOp", 0));
        m_alphaCombo->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("aBlendOp", val);
            setDirty();
        });
        m_alphaCombo->setEndOfLine(false);
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::COL, "COL");
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::ADD, "ADD");
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::SUB, "SUB");
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::MUL, "MUL");
        m_alphaCombo->setEndOfLine(true);
    }

    // Only show details if this is single target
    if (comp->size() == 1)
    {
        auto figure = std::dynamic_pointer_cast<FigureComponent>(comp->getComponents()[0])->getFigure();
        if (figure)
        {
            if (figure->NumMeshes() > 0)
            {
                auto meshGroup = m_figureCompGroup->createWidget<Group>("Mesh", true, false, Group::E_Align::LEFT, false);
                auto meshColumn = meshGroup->createWidget<Columns<3>>();
                for (int i = 0; i < figure->NumMeshes(); i++)
                {
                    auto mesh = figure->GetMesh(i);
                    auto txtName = meshColumn->createWidget<TextField>("", figure->GetMeshName(i), true);
                    txtName->addPlugin<DDSourcePlugin<int>>(EDragDropID::MESH, figure->GetMeshName(i), i);
                    meshColumn->createWidget<TextField>("V", std::to_string(mesh->numVerticies).c_str(), true);
                    meshColumn->createWidget<TextField>("I", std::to_string(mesh->numIndices).c_str(), true);
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
                            if ((currMat->params[j].type == ParamTypeFloat4))
                            {
                                auto color = Vec4(currMat->params[j].fValue[0], currMat->params[j].fValue[1], currMat->params[j].fValue[2], currMat->params[j].fValue[3]);
                                materialGroup->createWidget<Color>(info->name, color);
                            }
                            else if ((currMat->params[j].type == ParamTypeSampler))
                            {
                                auto textPath = currMat->params[j].sampler.tex->ResourceName();
                                materialGroup->createWidget<TextField>(info->name, textPath);
                            }
                            else if ((currMat->params[j].type == ParamTypeFloat))
                            {
                                std::array val = { currMat->params[j].fValue[0] };
                                materialGroup->createWidget<Drag<float>>(info->name, ImGuiDataType_Float, val);
                            }
                        }
                    }
                }
            }
        }
    }    
}
NS_IGE_END