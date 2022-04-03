#include "core/panels/AnimatorEditor.h"

#include <components/animation/AnimatorStateMachine.h>
#include <components/animation/AnimatorState.h>
#include <components/animation/AnimatorTransition.h>

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/dialog/MsgBox.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/task/TaskManager.h"
#include "core/panels/Inspector.h"

#include "core/widgets/Label.h"
#include "core/widgets/TextField.h"
#include "core/widgets/CheckBox.h"
#include "core/widgets/ComboBox.h"
#include "core/widgets/Button.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Drag.h"
#include "core/layout/Columns.h"
#include "core/FileHandle.h"
#include "core/Canvas.h"
#include "core/panels/AnimatorPreview.h"

#include "core/CommandManager.h"

#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

#define PIN_START_COLOR     {51, 150, 215}
#define PIN_END_COLOR       {220,  48,  48}
#define LINK_NORMAL_COLOR   {147, 226, 74}
#define LINK_MUTE_COLOR     {128, 128, 128}

namespace ige::creator
{
    static bool ImGui_Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
    {
        using namespace ImGui;
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
        bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
        return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }

#pragma region NodeBuilder
    NodeBuilder::NodeBuilder(ImTextureID texture, int textureWidth, int textureHeight) :
        CurrentNodeId(0),
        CurrentStage(Stage::Invalid)
    {
    }

    void NodeBuilder::Begin(ed::NodeId id)
    {
        ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
        ed::BeginNode(id);
        ImGui::PushID(id.AsPointer());
        CurrentNodeId = id;
        SetStage(Stage::Begin);
    }

    void NodeBuilder::End()
    {
        SetStage(Stage::End);
        ed::EndNode();
        CurrentNodeId = 0;
        ImGui::PopID();
        ed::PopStyleVar();
        SetStage(Stage::Invalid);
    }

    void NodeBuilder::Input(ed::PinId id)
    {
        if (CurrentStage == Stage::Begin)
            SetStage(Stage::Content);
        const auto applyPadding = (CurrentStage == Stage::Input);
        SetStage(Stage::Input);
        if (applyPadding)
            ImGui::Spring(0);
        Pin(id, ed::PinKind::Input);
        ImGui::BeginHorizontal(id.AsPointer());
    }

    void NodeBuilder::EndInput()
    {
        ImGui::EndHorizontal();
        EndPin();
    }

    void NodeBuilder::Middle()
    {
        if (CurrentStage == Stage::Begin)
            SetStage(Stage::Content);
        SetStage(Stage::Middle);
    }

    void NodeBuilder::Output(ed::PinId id)
    {
        if (CurrentStage == Stage::Begin)
            SetStage(Stage::Content);
        const auto applyPadding = (CurrentStage == Stage::Output);
        SetStage(Stage::Output);
        if (applyPadding)
            ImGui::Spring(0);
        Pin(id, ed::PinKind::Output);
        ImGui::BeginHorizontal(id.AsPointer());
    }

    void NodeBuilder::EndOutput()
    {
        ImGui::EndHorizontal();
        EndPin();
    }

    bool NodeBuilder::SetStage(Stage stage)
    {
        if (stage == CurrentStage)
            return false;

        auto oldStage = CurrentStage;
        CurrentStage = stage;
        ImVec2 cursor;
        switch (oldStage)
        {
        case Stage::Begin:
            break;
        case Stage::Content:
            break;
        case Stage::Input:
            ed::PopStyleVar(2);
            ImGui::Spring(1, 0);
            ImGui::EndVertical();
            break;
        case Stage::Middle:
            ImGui::EndVertical();
            break;
        case Stage::Output:
            ed::PopStyleVar(2);
            ImGui::Spring(1, 0);
            ImGui::EndVertical();
            break;
        case Stage::End:
            break;
        case Stage::Invalid:
            break;
        }

        switch (stage)
        {
        case Stage::Begin:
            ImGui::BeginVertical("node");
            break;
        case Stage::Content:
            if (oldStage == Stage::Begin)
                ImGui::Spring(0);
            ImGui::BeginHorizontal("content");
            ImGui::Spring(0, 0);
            break;
        case Stage::Input:
            ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);
            ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
            ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));
            ImGui::Spring(1, 0);
            break;
        case Stage::Middle:
            ImGui::Spring(1);
            ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
            break;
        case Stage::Output:
            if (oldStage == Stage::Middle || oldStage == Stage::Input)
                ImGui::Spring(1);
            else
                ImGui::Spring(1, 0);
            ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);
            ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
            ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));
            ImGui::Spring(1, 0);
            break;
        case Stage::End:
            if (oldStage == Stage::Input)
                ImGui::Spring(1, 0);
            if (oldStage != Stage::Begin)
                ImGui::EndHorizontal();
            ContentMin = ImGui::GetItemRectMin();
            ContentMax = ImGui::GetItemRectMax();
            ImGui::EndVertical();
            NodeMin = ImGui::GetItemRectMin();
            NodeMax = ImGui::GetItemRectMax();
            break;
        case Stage::Invalid:
            break;
        }
        return true;
    }

    void NodeBuilder::Pin(ed::PinId id, ed::PinKind kind)
    {
        ed::BeginPin(id, kind);
    }

    void NodeBuilder::EndPin()
    {
        ed::EndPin();
    }
#pragma endregion NodeBuilder
 
    AnimatorEditor::AnimatorEditor(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        getOnClosedEvent().addListener([this]() {
            if (isDirty()) {
                auto btn = MsgBox("Save", "Animator has changed, do you want to save?", MsgBox::EBtnLayout::yes_no, MsgBox::EMsgType::question).result();
                if (btn == MsgBox::EButton::yes) {
                    save();
                }
            }
            if(Editor::getCanvas() && Editor::getCanvas()->getAnimatorPreview())
                Editor::getCanvas()->getAnimatorPreview()->close();
            m_path.clear();
            m_currLayer = -1;
            clear();
        });
    }

    AnimatorEditor::~AnimatorEditor()
    {
        if (m_layerGroup)
            m_layerGroup->removeAllWidgets();
        m_layerGroup = nullptr;

        if (m_parameterGroup)
            m_parameterGroup->removeAllWidgets();
        m_parameterGroup = nullptr;

        m_controller = nullptr;
        m_path.clear();
        clear();
    }

    void AnimatorEditor::clear()
    {
        m_pyxaDragDropPlugin = nullptr;
        m_animDragDropPlugin = nullptr;

        m_nodes.clear();
        m_links.clear();

        ed::SetCurrentEditor(nullptr);
        ed::DestroyEditor(m_editor);
        m_editor = nullptr;

        try {
            fs::remove(fs::path("AnimatorLayout.ini"));
        }
        catch (std::exception e) {}

        setDirty(false);
        m_currState = nullptr;

        m_bInitialized = false;
    }

    void AnimatorEditor::initialize()
    {
        if (m_path.empty() || m_currLayer < 0)
            return Panel::initialize();

        clear();

        if (!m_bInitialized) {
            // Init Node Editor
            m_uniqueId = 1;
            ed::Config config;
            config.SettingsFile = "AnimatorLayout.ini";
            m_editor = ed::CreateEditor(&config);
            ed::SetCurrentEditor(m_editor);

            auto stateMachine = m_controller->getStateMachine(m_currLayer);

            if (stateMachine->getStates().size() > 0) {
                for (const auto& state : stateMachine->getStates()) {
                    auto& node = createNode(state->getName(), (NodeType)state->getType(), ImVec2(state->getPosition().X(), state->getPosition().Y()));
                    node->state = state;
                }
                for (const auto& node : m_nodes) {
                    if (node->state.expired()) continue;
                    auto state = node->state.lock();
                    for (const auto& transition : state->getTransitions()) {
                        if (!transition->destState.expired()) {
                            const auto& dstNode = findNode(transition->destState.lock());
                            if (dstNode) {
                                transition->linkId = getNextId();
                                auto link = std::make_shared<Link>(transition->linkId, node->outPin->id, dstNode->inPin->id);
                                link->color = transition->isMute ? ImColor(LINK_MUTE_COLOR) : ImColor(LINK_NORMAL_COLOR);
                                link->transition = transition;
                                m_links.push_back(link);
                            }
                        }
                    }
                }
            }

            m_pyxaDragDropPlugin = std::make_shared<DDTargetPlugin<std::string>>(".pyxa");
            std::dynamic_pointer_cast<DDTargetPlugin<std::string>>(m_pyxaDragDropPlugin)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                auto fsPath = fs::path(path);
                auto name = fsPath.stem().string();
                if (!m_controller->getStateMachine(m_currLayer)->hasState(name)) {
                    auto& node = createNode(name, NodeType::Normal, ImGui::GetIO().MousePos);
                    node->state = m_controller->getStateMachine(m_currLayer)->addState(name);
                    node->state.lock()->setPath(path);
                }
            });

            m_animDragDropPlugin = std::make_shared<DDTargetPlugin<std::string>>(".anim");
            std::dynamic_pointer_cast<DDTargetPlugin<std::string>>(m_animDragDropPlugin)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                TaskManager::getInstance()->addTask([this, path]() {
                    m_path = path;
                    initialize();
                });
            });

            ed::NavigateToContent();

            m_bLayerDirty = true;
            m_bParameterDirty = true;

            m_bInitialized = true;
        }
    }

    void AnimatorEditor::setDirty(bool dirty) {
        if (m_bDirty != dirty) {
            m_bDirty = dirty;
            if (isDirty()) {
                setName(getName() + "*");
            }
            else {
                auto name = getName();
                name.erase(name.end() - 1);
                setName(name);
            }
        }
    }

    void AnimatorEditor::openAnimator(const std::string& path)
    {
        if (m_path.compare(path) != 0) {
            clear();
            m_path = path;
            m_controller = std::make_shared<AnimatorController>();
            m_controller->setPath(m_path);
            setLayer(0);
            Editor::getCanvas()->getAnimatorPreview()->clear();
            Editor::getCanvas()->getAnimatorPreview()->setModelPath(m_controller->getBaseModelPath());
        }
        open();
        Editor::getCanvas()->getAnimatorPreview()->open();
    }

    bool AnimatorEditor::save()
    {
        if (!m_controller)
            return false;
        if (m_path.empty()) {
            m_path = SaveFileDialog("Save", "", { "anim", "*.anim" }).result();
        }
        if (!m_path.empty()) {
            for (const auto& node : m_nodes) {
                if (node->state.expired()) continue;
                auto state = node->state.lock();
                if (state) {
                    auto pos = ed::GetNodePosition(node->id);
                    state->setPosition({ pos.x, pos.y });
                }
            }
            m_controller->save(m_path);
            setDirty(false);
            return true;
        }
        return false;
    }

    std::shared_ptr<Node> AnimatorEditor::findNode(ed::NodeId id)
    {
        auto itr = std::find_if(m_nodes.begin(), m_nodes.end(), [&](const auto& node) {
            return node->id == id;
        });
        return itr != m_nodes.end() ? *itr : nullptr;
    }

    void AnimatorEditor::removeNode(ed::NodeId id)
    {
        auto itr = std::find_if(m_nodes.begin(), m_nodes.end(), [&](const auto& node) {
            return node->id == id;
        });
        if (itr != m_nodes.end()) {
            m_nodes.erase(itr);
        }
    }

    std::shared_ptr<Link> AnimatorEditor::findLink(ed::LinkId id)
    {
        auto itr = std::find_if(m_links.begin(), m_links.end(), [&](const auto& link) {
            return link->id == id;
        });
        return itr != m_links.end() ? *itr : nullptr;
    }

    void AnimatorEditor::removeLink(ed::LinkId id)
    {
        auto itr = std::find_if(m_links.begin(), m_links.end(), [&](const auto& link) {
            return link->id == id;
        });
        if (itr != m_links.end()) {
            m_links.erase(itr);
        }
    }

    std::shared_ptr<Pin> AnimatorEditor::findPin(ed::PinId id)
    {
        for (const auto& node : m_nodes) {
            if(node->inPin && node->inPin->id == id)
                return node->inPin;
            if (node->outPin && node->outPin->id == id)
                return node->outPin;
        }
        return nullptr;
    }

    bool AnimatorEditor::isPinLinked(ed::PinId id)
    {
        if (!id) return false;
        for (const auto& link : m_links)
            if (link->startPinID == id || link->endPinID == id)
                return true;
        return false;
    }

    bool AnimatorEditor::hasLink(ed::PinId id1, ed::PinId id2)
    {
        if (!id1 || !id2) return false;
        for (const auto& link : m_links)
            if ((link->startPinID == id1 && link->endPinID == id2)
                || (link->startPinID == id2 && link->endPinID == id1))
                return true;
        return false;
    }

    bool AnimatorEditor::canCreateLink(const std::shared_ptr<Pin>& p1, const std::shared_ptr<Pin>& p2)
    {
        if (!p1 || !p2 || p1 == p2 || p1->type == p2->type || p1->node.lock() == p2->node.lock())
            return false;
        return true;
    }

    void AnimatorEditor::drawPinIcon(bool connected, ImColor color, int alpha)
    {
        const auto size = ImVec2(24.f, 24.f);
        const auto innerColor = ImColor(32, 32, 32, alpha);

        if (ImGui::IsRectVisible(size)) {
            auto cursorPos = ImGui::GetCursorScreenPos();
            auto drawList = ImGui::GetWindowDrawList();

            auto rect = ImRect(cursorPos, cursorPos + size);
            auto rect_x = rect.Min.x;
            auto rect_y = rect.Min.y;
            auto rect_w = rect.Max.x - rect.Min.x;
            auto rect_h = rect.Max.y - rect.Min.y;
            auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
            auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
            auto rect_center = ImVec2(rect_center_x, rect_center_y);
            const auto outline_scale = rect_w / 24.0f;
            const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle
            auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);
            auto triangleStart = rect_center_x + 0.32f * rect_w;

            rect.Min.x += rect_offset;
            rect.Max.x += rect_offset;
            rect_x += rect_offset;
            rect_center_x += rect_offset * 0.5f;
            rect_center.x += rect_offset * 0.5f;

            if (!connected) {
                const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                if (innerColor & 0xFF000000) drawList->AddCircleFilled(rect_center, r, innerColor, 12 + extra_segments);
                drawList->AddCircle(rect_center, r, color, 12 + extra_segments, 2.0f * outline_scale);
            }
            else {
                drawList->AddCircleFilled(rect_center, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
            }

            const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

            drawList->AddTriangleFilled(
                ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
                ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
                ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
                color);
        }
        ImGui::Dummy(size);
    }

    std::shared_ptr<Node> AnimatorEditor::createNode(const std::string& name, NodeType nodeType, const ImVec2& position)
    {
        auto node = std::make_shared<Node>(getNextId(), name, position);
        if (nodeType != NodeType::Entry && nodeType != NodeType::Any)
            node->inPin = std::make_shared<Pin>(getNextId(), ed::PinKind::Input, node);
        if (nodeType != NodeType::Exit)
            node->outPin = std::make_shared<Pin>(getNextId(), ed::PinKind::Output, node);
        ed::SetNodePosition(node->id, position);
        m_nodes.push_back(node);
        return node;
    }

    std::shared_ptr<Node> AnimatorEditor::findNode(const std::shared_ptr<AnimatorState>& state) {
        if (state == nullptr) return nullptr;
        auto itr = std::find_if(m_nodes.begin(), m_nodes.end(), [&](const auto& node) {
            return !node->state.expired() && node->state.lock() == state;
        });
        if (itr != m_nodes.end()) return *itr;
        return nullptr;
    }

    void AnimatorEditor::showLeftPanel()
    {
        auto& io = ImGui::GetIO();
        static bool selected = false;
        ImGui::BeginChild("LeftPanel", ImVec2(m_leftPanelWidth, 0));
        {
            ImGui::BeginGroup();
            {
                ImGui::BeginChild("LeftPanelContent", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
                {
                    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                    {
                        // Draw layers
                        drawLayers();
                         
                        // Draw parameters
                        drawParameters();
                            
                        ImGui::EndTabBar();
                    }
                }
                ImGui::EndChild();
                ImGui::BeginHorizontal("###Bottom#Buttons", ImVec2(m_leftPanelWidth, 0));
                ImGui::Spring();
                if (ImGui::Button("Expand")) { ed::NavigateToContent(); }
                ImGui::Spring(0, 12.f);
                if (ImGui::Button("Save")) { if(isDirty()) save(); }
                ImGui::EndHorizontal();
            }
            ImGui::EndGroup();
        }        
        ImGui::EndChild();
        ImGui::SameLine(0.0f, 12.0f);
    }

    void AnimatorEditor::setLayer(int layer) {
        if (m_controller != nullptr && m_currLayer != layer) {
            m_currLayer = layer;
            initialize();
        }
    }

    void AnimatorEditor::drawLayers()
    {
        if (ImGui::BeginTabItem("Layers"))
        {
            if (m_bLayerDirty) {
                if (m_layerGroup == nullptr)
                    m_layerGroup = std::make_shared<Group>("Layers", false);
                m_layerGroup->removeAllWidgets();
                
                if (m_controller != nullptr) {
                    auto modelPath = m_controller->getBaseModelPath();
                    auto txtBaseModel = m_layerGroup->createWidget<TextField>("Base Model", modelPath, false, true);
                    txtBaseModel->getOnDataChangedEvent().addListener([this](const auto& txt) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        auto basePath = m_controller->getBaseModelPath();
                        if (basePath.compare(txt) != 0) {
                            m_controller->setBaseModelPath(txt);
                            if (Editor::getCanvas() && Editor::getCanvas()->getAnimatorPreview()) {
                                Editor::getCanvas()->getAnimatorPreview()->setModelPath(txt);
                            }
                            setDirty();
                        }
                    });

                    for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure)) {
                        txtBaseModel->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                            CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                            auto basePath = m_controller->getBaseModelPath();
                            if (basePath.compare(path) != 0) {
                                m_controller->setBaseModelPath(path);
                                if (Editor::getCanvas() && Editor::getCanvas()->getAnimatorPreview()) {
                                    Editor::getCanvas()->getAnimatorPreview()->setModelPath(path);
                                }
                                setDirty();
                            }
                        });
                    }

                    txtBaseModel->addPlugin<DDTargetPlugin<std::string>>(".pyxa")->getOnDataReceivedEvent().addListener([this](const auto& path) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        auto node = findNode(m_node);
                        if (node != nullptr && !node->state.expired()) {
                            node->state.lock()->setPath(path);
                            setInspectorDirty();
                            setDirty();
                        }
                    });

                    auto headerCols = m_layerGroup->createWidget<Columns<2>>();
                    headerCols->setColumnWidth(1, 32.f);
                    headerCols->createWidget<Label>("Layers");
                    headerCols->createWidget<Button>("+", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this](const auto& widget) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        if (m_controller->addLayer()) {
                            setLayer(m_controller->getStateMachines().size() - 1);
                            setLayersDirty();
                            setDirty();
                        }
                    });
                    m_layerGroup->createWidget<Separator>();

                    auto columns = m_layerGroup->createWidget<Columns<2>>();
                    columns->setColumnWidth(1, 32.f);
                    for (int i = 0; i < m_controller->getStateMachines().size(); ++i) {
                        m_controller->getStateMachines()[i]->setLayer(i);
                        auto layer = i;
                        auto label = layer == 0 ? "Layer A" : layer == 1 ? "Layer B" : "Layer C";
                        columns->createWidget<TreeNode>(label, m_currLayer == i, true)->getOnClickEvent().addListener([this, layer](const auto& widget) {
                            CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                            setLayer(layer);
                        });
                        columns->createWidget<Button>("-", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this, layer](const auto& widget) {
                            CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                            m_controller->removeLayer(layer);
                            if (m_currLayer == layer) setLayer(layer - 1);
                            setLayersDirty();
                        });
                    }
                }

                m_bLayerDirty = false;
            }
            if (m_layerGroup)
                m_layerGroup->drawWidgets();
            ImGui::EndTabItem();
        }
    }

    void AnimatorEditor::drawParameters()
    {
        if (ImGui::BeginTabItem("Parameters"))
        {
            if (m_bParameterDirty) {
                if (m_parameterGroup == nullptr)
                    m_parameterGroup = std::make_shared<Group>("Layers", false);
                m_parameterGroup->removeAllWidgets();

                auto selectCombo = m_parameterGroup->createWidget<ComboBox>("", m_selectedType);
                selectCombo->getOnDataChangedEvent().addListener([this](auto val) {
                    m_selectedType = val;
                });
                selectCombo->setEndOfLine(false);
                selectCombo->addChoice((int)AnimatorParameterType::Bool, "Bool");
                selectCombo->addChoice((int)AnimatorParameterType::Int, "Int");
                selectCombo->addChoice((int)AnimatorParameterType::Float, "Float");
                selectCombo->addChoice((int)AnimatorParameterType::Trigger, "Trigger");
                m_parameterGroup->createWidget<Button>("+", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this](const auto& widget) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto name = std::string("Parameter") + std::to_string(m_controller->getParameters().size() + 1);
                    m_controller->setParameter(name, m_selectedType, 0.f);
                    setParametersDirty();
                    setInspectorDirty();
                });

                m_parameterGroup->createWidget<Separator>();
                auto columns = m_parameterGroup->createWidget<Columns<3>>();
                auto width = (ImGui::GetWindowContentRegionWidth() - 32.f) / 3;
                columns->setColumnWidth(0, 2 * width);
                columns->setColumnWidth(1, width);
                columns->setColumnWidth(2, 32.f);
                for (const auto& param : m_controller->getParameters()) {
                    auto name = param.first;
                    auto type = param.second.first;
                    auto val = param.second.second;
                    auto nameTxt = columns->createWidget<TextField>("", name, false, true)->getOnDataChangedEvent().addListener([name, type, val, this](const auto& txt) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        m_controller->removeParameter(name);
                        m_controller->setParameter(txt, (int)type, val);
                        setParametersDirty();
                        setInspectorDirty();
                    });
                    switch (type)
                    {
                        case AnimatorParameterType::Bool:
                        {
                            auto selectCombo = columns->createWidget<ComboBox>("");
                            selectCombo->getOnDataChangedEvent().addListener([name, type, this](auto val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                m_controller->setParameter(name, (int)type, val);
                            });
                            selectCombo->setEndOfLine(false);
                            selectCombo->addChoice(0, "False");
                            selectCombo->addChoice(1, "True");
                            break;
                        }
                        case AnimatorParameterType::Trigger:
                        {
                            columns->createWidget<CheckBox>("", false)->getOnDataChangedEvent().addListener([name, type, this](bool val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                m_controller->setParameter(name, (int)type, false);
                                setParametersDirty();
                            });
                            break;
                        }
                        case AnimatorParameterType::Int:
                        case AnimatorParameterType::Float:
                        {
                            std::array vals = { val };
                            columns->createWidget<Drag<float>>("", (type == AnimatorParameterType::Int) ? ImGuiDataType_S32 : ImGuiDataType_Float, vals)->getOnDataChangedEvent().addListener([name, type, this](auto val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                m_controller->setParameter(name, (int)type, val[0]);
                            });
                            break;
                        }
                    }

                    columns->createWidget<Button>("-", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([name, this](const auto& widget) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        m_controller->removeParameter(name);
                        setParametersDirty();
                        setInspectorDirty();
                    });
                }
                m_bParameterDirty = false;
            }
            if(m_parameterGroup)
                m_parameterGroup->drawWidgets();
            ImGui::EndTabItem();
        }
    }

    void AnimatorEditor::update(float dt)
    {
        Panel::update(dt);

        if (!m_editor) return;
        
        if (m_controller) {
            if (m_controller->isDirty()) {
                setDirty();
                setLayersDirty();
                setParametersDirty();
                setInspectorDirty();
                setLayer(0);
                clear();
                m_controller->resetDirty();
                return;
            }

            m_controller->update(dt);

            auto currState = m_controller->getStateMachine(m_currLayer)->getCurrentState();
            if (currState != nullptr) {
                auto node = findNode(currState);
                if (node != nullptr) {
                    node->percentage = currState->getEndTime() > 0.f ? currState->getEvalTime() / currState->getEndTime() : 0.f;                     
                }
                if (m_currState != currState) {
                    if (m_currState != nullptr) {
                        auto node = findNode(m_currState);
                        if (node) {
                            node->percentage = 0.f;
                        }
                    }
                    m_currState = currState;
                }
            }
        }   
    }

    void AnimatorEditor::drawWidgets()
    {
        if (!m_bInitialized || !m_editor)
            return;

        static float leftPaneWidth = 400.0f;
        static float rightPaneWidth = 800.0f;
        ImGui_Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

        // Show left panel
        if (m_leftPanelWidth != leftPaneWidth - 4.f) {
            m_leftPanelWidth = leftPaneWidth - 4.f;
            setLayersDirty();
            setParametersDirty();
        }
        showLeftPanel();

        // Start interaction with editor.
        ed::Begin("Canvas");
        {
            auto cursorTopLeft = ImGui::GetCursorScreenPos();

            m_pyxaDragDropPlugin->execute();
            m_animDragDropPlugin->execute();

            auto builder = NodeBuilder();

            for (auto& node : m_nodes) {
                if (node->state.expired()) continue;
                auto state = node->state.lock();
                builder.Begin(node->id);
                {
                    // Input
                    {
                        auto input = node->inPin;
                        if (input) {
                            builder.Input(input->id);
                            drawPinIcon(isPinLinked(input->id), PIN_END_COLOR, (int)(ImGui::GetStyle().Alpha * 255));
                            ImGui::Spring(0);
                            builder.EndInput();
                        }
                    }

                    // Middle
                    {
                        builder.Middle();
                        ImGui::Spring(1, 0);
                        ImGui::ProgressBar(node->percentage, {150, 18}, node->name.c_str());
                        ImGui::Spring(1, 0);
                    }

                    // Output
                    {
                        auto output = node->outPin;
                        if (output) {
                            builder.Output(output->id);
                            ImGui::Spring(0);
                            drawPinIcon(isPinLinked(output->id), PIN_START_COLOR, (int)(ImGui::GetStyle().Alpha * 255));
                            builder.EndOutput();
                        }
                    }
                }
                builder.End();
            }

            for (auto& link : m_links)
            {
                ed::Link(link->id, link->startPinID, link->endPinID, link->color, 2.0f);
            }

            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char* label, ImColor color)
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    auto startPin = findPin(startPinId);
                    if (startPin)
                    {
                        if (startPin->type == ed::PinKind::Input)
                        {
                            std::swap(startPinId, endPinId);
                            startPin = findPin(startPinId);
                        }
                        
                        auto endPin = findPin(endPinId);
                        if (startPinId == endPinId || startPin->node.expired() || endPin->node.expired() || hasLink(startPinId, endPinId))
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->type == startPin->type)
                        {
                            showLabel("x Incompatible Pin", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->node.lock() == startPin->node.lock())
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Transition", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR_ADD, m_controller);
                                auto linkId = getNextId();
                                auto link = std::make_shared<Link>(linkId, startPinId, endPinId);
                                link->color = LINK_NORMAL_COLOR;
                                m_links.push_back(link);
                                auto state = startPin->node.lock()->state;
                                auto dstState = endPin->node.lock()->state;
                                if (!state.expired() && !dstState.expired()) {
                                    auto transition = state.lock()->addTransition(dstState.lock());
                                    transition->setName(state.lock()->getName() + "_" + dstState.lock()->getName());
                                    link->transition = transition;
                                    transition->linkId = linkId;
                                    setDirty();
                                }
                            }
                        }
                    }
                }
            }
            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR_ADD, m_controller);
                        const auto& link = findLink(linkId);
                        if (link != nullptr) {
                            const auto& startPin = findPin(link->startPinID);
                            const auto& endPin = findPin(link->endPinID);
                            if (!startPin->node.expired() && !endPin->node.expired()) {
                                auto state = startPin->node.lock()->state;
                                auto dstState = endPin->node.lock()->state;
                                if (!state.expired() && !dstState.expired()) {
                                    auto transition = state.lock()->findTransition(dstState.lock());
                                    state.lock()->removeTransition(transition);
                                    setDirty();
                                }
                            }
                        }
                        removeLink(linkId);
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR_ADD, m_controller);
                        auto node = findNode(nodeId);
                        if (node != nullptr) {
                            if (!node->state.expired()) {
                                m_controller->getStateMachine(m_currLayer)->removeState(node->state.lock());
                                setDirty();
                            }
                        }
                        removeNode(nodeId);
                    }
                }
            }
            ed::EndDelete();

            if (ed::GetSelectedObjectCount() > 0) {
                if (ed::NodeId node; ed::GetSelectedNodes(&node, 1) == 1) {
                    if (m_node != node) {
                        m_bInspectDirty = true;
                        m_node = node;
                    }
                    m_link = -1;
                }
                else if (ed::LinkId link; ed::GetSelectedLinks(&link, 1) == 1) {
                    if (m_link != link) {
                        m_bInspectDirty = true;
                        m_link = link;
                    }
                    m_node = -1;
                }
            }
            else {
                m_link = -1;
                m_node = -1;
            }

            ImGui::SetCursorScreenPos(cursorTopLeft);
        }
        ed::End();
    }

    bool AnimatorEditor::shouldDrawInspector() {
        return m_bFocus && (m_node != (ed::NodeId)-1 || m_link != (ed::LinkId)-1);
    }

    void AnimatorEditor::setFocus(bool focus) {
        if (m_bFocus != focus) {
            m_bFocus = focus;
            setInspectorDirty();

            if (!focus)
                Editor::getCanvas()->getInspector()->redraw();
        }
    }

    void AnimatorEditor::setFigure(Figure* figure)
    {
        if (m_controller && m_currLayer >= 0) {
            m_controller->setFigure(figure);
            auto enterState = m_controller->getStateMachine(m_currLayer)->getEnterState();
            m_controller->getStateMachine(m_currLayer)->setCurrentState(enterState);
        }
    }

    void AnimatorEditor::drawInspector()
    {
        if (!shouldDrawInspector()) return;
        if (m_bInspectDirty) {
            m_inspectGroup = nullptr;
            Editor::getCanvas()->getInspector()->clear();
            m_inspectGroup = Editor::getCanvas()->getInspector()->createWidget<Group>("Animator_Inspector_Group", false);

            // Draw node
            drawNode();

            // Draw link
            drawLink();

            m_bInspectDirty = false;
        }
    }

    void AnimatorEditor::drawNode() {
        auto node = findNode(m_node);
        if (node != nullptr && !node->state.expired()) {
            auto state = node->state.lock();
            m_inspectGroup->createWidget<TextField>("UUID", state->getUUID(), true);
            m_inspectGroup->createWidget<TextField>("Name", state->getName(), false, true)->getOnDataChangedEvent().addListener([this](const auto& txt) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto node = findNode(m_node);
                if (node != nullptr && !node->state.expired()) {
                    node->state.lock()->setName(txt);
                    setInspectorDirty();
                    setDirty();
                }
            });
            auto animTxt = m_inspectGroup->createWidget<TextField>("AnimClip", state->getPath(), false, true);
            animTxt->getOnDataChangedEvent().addListener([this](const auto& txt) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto node = findNode(m_node);
                if (node != nullptr && !node->state.expired()) {
                    node->state.lock()->setPath(txt);
                    setInspectorDirty();
                    setDirty();
                }
            });
            animTxt->addPlugin<DDTargetPlugin<std::string>>(".pyxa")->getOnDataReceivedEvent().addListener([this](const auto& path) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto node = findNode(m_node);
                if (node != nullptr && !node->state.expired()) {
                    node->state.lock()->setPath(path);
                    setInspectorDirty();
                    setDirty();
                }
            });

            if (state->getTransitions().size() > 0) {
                auto transitionGroup = m_inspectGroup->createWidget<Group>("Transitions");
                auto columns = transitionGroup->createWidget<Columns<2>>();
                columns->setColumnWidth(1, 40.f);
                columns->createWidget<Label>("Transition");
                columns->createWidget<Label>("Mute");
                for (const auto& tran : state->getTransitions()) {
                    if (tran && !tran->destState.expired()) {
                        auto dstStateUUID = tran->destState.lock()->getUUID();
                        columns->createWidget<Label>(tran->getName());
                        columns->createWidget<CheckBox>("", tran->isMute)->getOnDataChangedEvent().addListener([this, dstStateUUID](bool val) {
                            CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                            auto node = findNode(m_node);
                            if (node != nullptr && !node->state.expired()) {
                                auto dstState = node->state.lock()->stateMachine.lock()->findState(dstStateUUID);
                                if (dstState != nullptr) {
                                    auto tran = node->state.lock()->findTransition(dstState);
                                    if (tran) {
                                        tran->isMute = val;
                                        auto link = findLink(tran->linkId);
                                        if (link) {
                                            link->color = tran->isMute ? ImColor(LINK_MUTE_COLOR) : ImColor(LINK_NORMAL_COLOR);
                                        }
                                        setDirty();
                                    }
                                }
                            }
                        });
                    }
                }
            }            

            if (state->getAnimator() != nullptr) {
                auto animClipGroup = m_inspectGroup->createWidget<Group>("AnimationClip");
                std::array startTimes = { state->getStartTime() };
                animClipGroup->createWidget<Drag<float>>("StartTime", ImGuiDataType_Float, startTimes)->getOnDataChangedEvent().addListener([this](auto val) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto node = findNode(m_node);
                    if (node != nullptr && !node->state.expired()) {
                        node->state.lock()->setStartTime(val[0]);
                        setDirty();
                    }
                });
                std::array evalTimes = { state->getEvalTime() };
                animClipGroup->createWidget<Drag<float>>("EvalTime", ImGuiDataType_Float, evalTimes)->getOnDataChangedEvent().addListener([this](auto val) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto node = findNode(m_node);
                    if (node != nullptr && !node->state.expired()) {
                        node->state.lock()->setEvalTime(val[0]);
                        setDirty();
                    }
                });
                std::array speeds = { state->getSpeed() };
                animClipGroup->createWidget<Drag<float>>("Speed", ImGuiDataType_Float, speeds)->getOnDataChangedEvent().addListener([this](auto val) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto node = findNode(m_node);
                    if (node != nullptr && !node->state.expired()) {
                        node->state.lock()->setSpeed(val[0]);
                        setDirty();
                    }
                });
                animClipGroup->createWidget<CheckBox>("Loop", state->isLoop())->getOnDataChangedEvent().addListener([this](bool val) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto node = findNode(m_node);
                    if (node != nullptr && !node->state.expired()) {
                        node->state.lock()->setLoop(val);
                        setDirty();
                    }
                });
            }
        }
    }

    void AnimatorEditor::drawLink() {
        auto link = findLink(m_link);
        if (link != nullptr && !link->transition.expired()) {
            auto transition = link->transition.lock();
            m_inspectGroup->createWidget<TextField>("Name", transition->getName(), false, true)->getOnDataChangedEvent().addListener([this](const auto& txt) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto link = findLink(m_link);
                if (link != nullptr && !link->transition.expired()) {
                    link->transition.lock()->setName(txt);
                    setDirty();
                }
            });
            m_inspectGroup->createWidget<CheckBox>("Mute", transition->isMute)->getOnDataChangedEvent().addListener([this](bool val) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto link = findLink(m_link);
                if (link != nullptr && !link->transition.expired()) {
                    link->transition.lock()->isMute = val;
                    link->color = val ? ImColor(LINK_MUTE_COLOR) : ImColor(LINK_NORMAL_COLOR);
                    setDirty();
                }
            });
            std::array offsets = { transition->offset };
            m_inspectGroup->createWidget<Drag<float>>("Transit Time", ImGuiDataType_Float, offsets)->getOnDataChangedEvent().addListener([this](auto val) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto link = findLink(m_link);
                if (link != nullptr && !link->transition.expired()) {
                    link->transition.lock()->offset = val[0];
                    setDirty();
                }
            });
            m_inspectGroup->createWidget<CheckBox>("HasExitTime", transition->hasExitTime)->getOnDataChangedEvent().addListener([this](bool val) {
                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                auto link = findLink(m_link);
                if (link != nullptr && !link->transition.expired()) {
                    link->transition.lock()->hasExitTime = val;
                    setInspectorDirty();
                    setDirty();
                }
            });
            if (transition->hasExitTime) {
                std::array exitTimes = { transition->exitTime };
                m_inspectGroup->createWidget<Drag<float>>("ExitTime", ImGuiDataType_Float, exitTimes)->getOnDataChangedEvent().addListener([this](auto val) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto link = findLink(m_link);
                    if (link != nullptr && !link->transition.expired()) {
                        link->transition.lock()->exitTime = val[0];
                        setDirty();
                    }
                });
                m_inspectGroup->createWidget<CheckBox>("FixedDuration", transition->hasFixedDuration)->getOnDataChangedEvent().addListener([this](bool val) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto link = findLink(m_link);
                    if (link != nullptr && !link->transition.expired()) {
                        link->transition.lock()->hasFixedDuration = val;
                        setInspectorDirty();
                        setDirty();
                    }
                });
                if (transition->hasFixedDuration) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    std::array durations = { transition->duration };
                    m_inspectGroup->createWidget<Drag<float>>("Duration", ImGuiDataType_Float, durations)->getOnDataChangedEvent().addListener([this](auto val) {
                        auto link = findLink(m_link);
                        if (link != nullptr && !link->transition.expired()) {
                            link->transition.lock()->duration = val[0];
                            setDirty();
                        }
                    });
                }
            }

            auto conditionGroup = m_inspectGroup->createWidget<Group>("Conditions");
            auto columns = conditionGroup->createWidget<Columns<4>>();
            columns->setColumnWidth(3, 32.f);
            for (const auto& cond : transition->conditions) {
                auto condId = cond->id;
                auto [type, value] = m_controller->getParameter(cond->parameter);

                columns->createWidget<Label>(cond->parameter);

                if (type == AnimatorParameterType::Trigger) {
                    columns->createWidget<Label>(""); // Not configurable
                }
                else {
                    auto modeCombo = columns->createWidget<ComboBox>("", (int)cond->mode);
                    modeCombo->getOnDataChangedEvent().addListener([this, condId](auto val) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        auto link = findLink(m_link);
                        if (link != nullptr && !link->transition.expired()) {
                            link->transition.lock()->getCondition(condId)->mode = (AnimatorCondition::Mode)val;
                            setInspectorDirty();
                            setDirty();
                        }
                    });
                    for (auto mode : AnimatorCondition::getValidModes(type)) {
                        modeCombo->addChoice((int)mode, AnimatorCondition::getMode(mode));
                    }
                }

                if (cond->mode == AnimatorCondition::Mode::If || cond->mode == AnimatorCondition::Mode::IfNot) {
                    columns->createWidget<Label>("");  // Not configurable
                }
                else {
                    switch (type)
                    {
                        case AnimatorParameterType::Bool: {
                            auto selectCombo = columns->createWidget<ComboBox>("", (int)cond->threshold);
                            selectCombo->getOnDataChangedEvent().addListener([this, condId](auto val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                auto link = findLink(m_link);
                                if (link != nullptr && !link->transition.expired()) {
                                    link->transition.lock()->getCondition(condId)->threshold = val;
                                    setDirty();
                                }
                            });
                            selectCombo->setEndOfLine(false);
                            selectCombo->addChoice(1, "True");
                            selectCombo->addChoice(0, "False");
                            break;
                        }
                        case AnimatorParameterType::Int:
                        case AnimatorParameterType::Float: {
                            std::array vals = { cond->threshold };
                            columns->createWidget<Drag<float>>("", (type == AnimatorParameterType::Int) ? ImGuiDataType_S32 : ImGuiDataType_Float, vals)->getOnDataChangedEvent().addListener([this, condId](auto val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                auto link = findLink(m_link);
                                if (link != nullptr && !link->transition.expired()) {
                                    link->transition.lock()->getCondition(condId)->threshold = val[0];
                                    setDirty();
                                }
                            });
                            break;
                        }
                    }
                }
                columns->createWidget<Button>("-", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))->getOnClickEvent().addListener([condId, this](const auto& widget) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto link = findLink(m_link);
                    if (link != nullptr && !link->transition.expired()) {
                        link->transition.lock()->removeCondition(condId);
                    }
                    setInspectorDirty();
                    setDirty();
                });
            }

            static std::vector<std::string> params; params.clear();
            for (const auto& param : m_controller->getParameters()) {
                params.push_back(param.first);
            }

            if (params.size() > 0) {
                static int selectedParmeter = 0;
                conditionGroup->createWidget<Separator>();
                auto columns = conditionGroup->createWidget<Columns<4>>();
                columns->setColumnWidth(3, 32.f);

                auto selectCombo = columns->createWidget<ComboBox>("", selectedParmeter);
                selectCombo->getOnDataChangedEvent().addListener([this](auto val) {
                    selectedParmeter = val;
                    setInspectorDirty();
                });
                for (int i = 0; i < params.size(); ++i) {
                    selectCombo->addChoice(i, params[i]);
                }

                auto param = params[selectedParmeter];
                auto [type, val] = m_controller->getParameter(param);

                static int selectedMode = 0;
                if (type == AnimatorParameterType::Trigger) {
                    columns->createWidget<Label>(""); // Not configurable
                }
                else {
                    auto modeCombo = columns->createWidget<ComboBox>("", selectedMode);
                    modeCombo->getOnDataChangedEvent().addListener([this](auto val) {
                        CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                        selectedMode = val;
                        setInspectorDirty();
                    });
                    for (auto mode : AnimatorCondition::getValidModes(type)) {
                        modeCombo->addChoice((int)mode, AnimatorCondition::getMode(mode));
                    }
                }

                static float threshold = 0.f;
                threshold = val;
                if (selectedMode == (int)AnimatorCondition::Mode::If || selectedMode == (int)AnimatorCondition::Mode::IfNot) {
                    columns->createWidget<Label>("");  // Not configurable
                }
                else {
                    switch (type)
                    {
                        case AnimatorParameterType::Bool: {
                            auto selectCombo = columns->createWidget<ComboBox>("");
                            selectCombo->getOnDataChangedEvent().addListener([this](auto val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                threshold = val;
                            });
                            selectCombo->setEndOfLine(false);
                            selectCombo->addChoice(0, "False");
                            selectCombo->addChoice(1, "True");
                            break;
                        }
                        case AnimatorParameterType::Trigger: {
                            columns->createWidget<CheckBox>("", false);
                            break;
                        }
                        case AnimatorParameterType::Int:
                        case AnimatorParameterType::Float: {
                            std::array vals = { val };
                            auto drag = columns->createWidget<Drag<float>>("", (type == AnimatorParameterType::Int) ? ImGuiDataType_S32 : ImGuiDataType_Float, vals);
                            drag->getOnDataChangedEvent().addListener([](auto val) {
                                threshold = val[0];
                            });
                            drag->getOnDataFinishChangedEvent().addListener([this](auto val) {
                                CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                                });
                            break;
                        }
                    }
                }

                columns->createWidget<Button>("+", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))->getOnClickEvent().addListener([this](const auto& widget) {
                    CommandManager::getInstance()->PushCommand(COMMAND_TYPE::ANIMATOR, m_controller);
                    auto link = findLink(m_link);
                    if (link != nullptr && !link->transition.expired()) {
                        link->transition.lock()->addCondition(params[selectedParmeter], AnimatorCondition::Mode(selectedMode), threshold);
                    }
                    setInspectorDirty();
                    setDirty();
                });
            }
        }
    }
}
