#include "core/scene/components/FigureEditorComponent.h"
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

FigureEditorComponent::~FigureEditorComponent()
{
    m_figureCompGroup = nullptr;
}

void FigureEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_figureCompGroup == nullptr) {
        m_figureCompGroup = m_group->createWidget<Group>("FigureGroup", false);
    }
    drawFigureComponent();

    EditorComponent::redraw();
}

void FigureEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_figureCompGroup = m_group->createWidget<Group>("FigureGroup", false);

    drawFigureComponent();
}

void FigureEditorComponent::drawFigureComponent()
{
    if (m_figureCompGroup == nullptr)
        return;
    m_figureCompGroup->removeAllWidgets();

    auto figureComp = getComponent<FigureComponent>();
    if (figureComp == nullptr)
        return;

    auto txtPath = m_figureCompGroup->createWidget<TextField>("Path", figureComp->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto figureComp = getComponent<FigureComponent>();
        figureComp->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto figureComp = getComponent<FigureComponent>();
            figureComp->setPath(txt);
            dirty();
            });
    }

    m_figureCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Figure (*.pyxf)", "*.pyxf" }).result();
        if (files.size() > 0)
        {
            auto figureComp = getComponent<FigureComponent>();
            figureComp->setPath(files[0]);
            dirty();
        }
        });

    auto figColumn = m_figureCompGroup->createWidget<Columns<2>>();

    figColumn->createWidget<CheckBox>("Fog", figureComp->isFogEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto figureComp = getComponent<FigureComponent>();
        figureComp->setFogEnabled(val);
        });

    figColumn->createWidget<CheckBox>("CullFace", figureComp->isCullFaceEnable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto figureComp = getComponent<FigureComponent>();
        figureComp->setCullFaceEnable(val);
        });

    figColumn->createWidget<CheckBox>("Z-Test", figureComp->isDepthTestEnable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto figureComp = getComponent<FigureComponent>();
        figureComp->setDepthTestEnable(val);
        });

    figColumn->createWidget<CheckBox>("Z-Write", figureComp->isDepthWriteEnable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto figureComp = getComponent<FigureComponent>();
        figureComp->setDepthWriteEnable(val);
        });

    figColumn->createWidget<CheckBox>("AlphaBlend", figureComp->isAlphaBlendingEnable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto figureComp = getComponent<FigureComponent>();
        figureComp->setAlphaBlendingEnable(val);
        dirty();
        });

    if (figureComp->isAlphaBlendingEnable())
    {
        std::array val = { figureComp->getAlphaBlendingOp() };
        m_figureCompGroup->createWidget<Drag<int>>("AlphaBlendOp", ImGuiDataType_S32, val, 1, 0, 3)->getOnDataChangedEvent().addListener([this](auto val) {
            auto figureComp = getComponent<FigureComponent>();
            figureComp->setAlphaBlendingOp(val[0]);
            });
    }

    auto figure = figureComp->getFigure();
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
NS_IGE_END