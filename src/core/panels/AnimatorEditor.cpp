#include "core/panels/AnimatorEditor.h"

#include <components/animation/AnimatorStateMachine.h>
#include <components/animation/AnimatorState.h>
#include <components/animation/AnimatorTransition.h>

#include "core/Editor.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/task/TaskManager.h"

#include <utils/filesystem.h>
namespace fs = ghc::filesystem;

#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

namespace ige::creator
{
    static inline ImRect ImGui_GetItemRect()
    {
        return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
    {
        auto result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
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
    {}

    AnimatorEditor::~AnimatorEditor()
    {
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

        m_controller = nullptr;
        m_bInitialized = false;
    }

    void AnimatorEditor::initialize()
    {
        clear();

        // States -> nodes
        // Transitions -> Links

        //1. Add/remove/update nodes
        // - Drag & Drop
        // - Right click menu


        //2. Add/remove/update links
        //  drag
        // info

        if (!m_bInitialized) {
            // Init Node Editor
            m_uniqueId = 1;
            ed::Config config;
            config.SettingsFile = "AnimatorLayout.ini";
            m_editor = ed::CreateEditor(&config);
            ed::SetCurrentEditor(m_editor);

            m_controller = std::make_shared<AnimatorController>(m_path);

            if (m_controller->getStateMachine()->getStates().size() > 0) {
                for (const auto& state : m_controller->getStateMachine()->getStates()) {
                    auto* node = createNode(state->getName(), (NodeType)state->getType(), ImVec2(state->getPosition().X(), state->getPosition().Y()));
                    node->userPtr = state.get();
                }
                for (const auto& node : m_nodes) {
                    auto state = (AnimatorState*)node.userPtr;
                    for (const auto& transition : state->getTransitions()) {
                        if (!transition->destState.expired()) {
                            auto dstNode = findNode(transition->destState.lock().get());
                            if (dstNode) {
                                m_links.push_back(Link(getNextId(), node.outPin->id, dstNode->inPin->id));
                                m_links.back().color = ImColor(147, 226, 74);
                            }
                        }
                    }
                }
            }

            m_pyxaDragDropPlugin = std::make_shared<DDTargetPlugin<std::string>>(".pyxa");
            std::dynamic_pointer_cast<DDTargetPlugin<std::string>>(m_pyxaDragDropPlugin)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                auto fsPath = fs::path(path);
                if (!m_controller->getStateMachine()->hasState(fsPath.stem().string())) {
                    m_controller->getStateMachine()->addState(fsPath.stem().string());
                    createNode(fsPath.stem().string(), NodeType::Normal, ImVec2(0, 0));
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
            m_bInitialized = true;
        }
    }

    void AnimatorEditor::openAnimator(const std::string& path)
    {
        if (m_path.compare(path) != 0) {
            m_path = path;
            initialize();
        }
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
                auto* state = (AnimatorState*)node.userPtr;
                if (state) {
                    auto pos = ed::GetNodePosition(node.id);
                    state->setPosition({ pos.x, pos.y });
                }
            }
            m_controller->save(m_path);
            return true;
        }
        return false;
    }

    Node* AnimatorEditor::findNode(ed::NodeId id)
    {
        auto itr = std::find_if(m_nodes.begin(), m_nodes.end(), [&](const auto& node) {
            return node.id == id;
        });
        return itr != m_nodes.end() ? &(*itr) : nullptr;
    }

    Link* AnimatorEditor::findLink(ed::LinkId id)
    {
        auto itr = std::find_if(m_links.begin(), m_links.end(), [&](const auto& link) {
            return link.id == id;
        });
        return itr != m_links.end() ? &(*itr) : nullptr;
    }

    Pin* AnimatorEditor::findPin(ed::PinId id)
    {
        for (auto& node : m_nodes) {
            if(node.inPin && node.inPin->id == id)
                return node.inPin;
            if (node.outPin && node.outPin->id == id)
                return node.outPin;
        }
        return nullptr;
    }

    bool AnimatorEditor::isPinLinked(ed::PinId id)
    {
        if (!id) return false;
        for (const auto& link : m_links)
            if (link.startPinID == id || link.endPinID == id)
                return true;
        return false;
    }

    bool AnimatorEditor::hasLink(ed::PinId id1, ed::PinId id2)
    {
        if (!id1 || !id2) return false;
        for (const auto& link : m_links)
            if ((link.startPinID == id1 && link.endPinID == id2)
                || (link.startPinID == id2 && link.endPinID == id1))
                return true;
        return false;
    }

    bool AnimatorEditor::canCreateLink(Pin* p1, Pin* p2)
    {
        if (!p1 || !p2 || p1 == p2 || p1->type == p2->type || p1->node == p2->node)
            return false;
        return true;
    }

    void AnimatorEditor::drawPinIcon(const Pin& pin, bool connected, int alpha)
    {
        const auto size = ImVec2(24.f, 24.f);
        const auto color = ImColor(147, 226, 74, alpha);
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

    Node* AnimatorEditor::createNode(const std::string& name, NodeType nodeType, const ImVec2& position)
    {
        m_nodes.emplace_back(getNextId(), name, position);
        auto& node = m_nodes.back();
        if (nodeType != NodeType::Entry)
            node.inPin = new Pin(getNextId(), ed::PinKind::Input, &node);
        if (nodeType != NodeType::Exit)
            node.outPin = new Pin(getNextId(), ed::PinKind::Output, &node);
        ed::SetNodePosition(node.id, position);
        return &node;
    }

    Node* AnimatorEditor::findNode(AnimatorState* state) {
        if (state == nullptr) return nullptr;
        auto itr = std::find_if(m_nodes.begin(), m_nodes.end(), [&](const auto& node) {
            return node.userPtr == (void*)state;
        });
        if (itr != m_nodes.end()) return &(*itr);
        return nullptr;
    }

    void AnimatorEditor::drawWidgets()
    {
        if (!m_bInitialized)
            return;

        // Start interaction with editor.
        ed::Begin("Canvas", ImVec2(0.0, 0.0f));
        {
            m_pyxaDragDropPlugin->execute();
            m_animDragDropPlugin->execute();

            auto cursorTopLeft = ImGui::GetCursorScreenPos();

            auto builder = NodeBuilder();

            for (auto& node : m_nodes) 
            {
                auto* state = (AnimatorState*)node.userPtr;
                builder.Begin(node.id);
                {
                    // Input
                    {
                        auto input = node.inPin;
                        if (input) {
                            builder.Input(input->id);
                            drawPinIcon(*input, isPinLinked(input->id), (int)(ImGui::GetStyle().Alpha * 255));
                            ImGui::Spring(0);
                            builder.EndInput();
                        }
                    }

                    // Middle
                    {
                        builder.Middle();
                        ImGui::Spring(1, 0);
                        ImGui::ProgressBar(0.f, {150, 18}, node.name.c_str());
                        ImGui::Spring(1, 0);
                    }

                    // Output
                    {
                        auto output = node.outPin;
                        if (output) {
                            builder.Output(output->id);
                            ImGui::Spring(0);
                            drawPinIcon(*output, isPinLinked(output->id), (int)(ImGui::GetStyle().Alpha * 255));
                            builder.EndOutput();
                        }
                    }
                }
                builder.End();
            }

            for (auto& link : m_links)
            {
                ed::Link(link.id, link.startPinID, link.endPinID, link.color, 2.0f);
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
                    auto endPin = findPin(endPinId);

                    if (startPin && endPin)
                    {
                        if (startPin->type == ed::PinKind::Input)
                        {
                            std::swap(startPin, endPin);
                            std::swap(startPinId, endPinId);
                        }
                        if (endPin == startPin || hasLink(startPin->id, endPin->id))
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->type == startPin->type)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->node == startPin->node)
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                m_links.emplace_back(Link(getNextId(), startPinId, endPinId));
                                m_links.back().color = ImColor(147, 226, 74);
                                auto state = (AnimatorState*)startPin->node->userPtr;
                                auto dstState = (AnimatorState*)endPin->node->userPtr;
                                state->addTransition(dstState->getSharedPtr());
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
                        auto itr = std::find_if(m_links.begin(), m_links.end(), [linkId](auto& link) { return link.id == linkId; });
                        if (itr != m_links.end()) {
                            auto startPin = findPin((*itr).startPinID);
                            auto endPin = findPin((*itr).endPinID);
                            auto state = (AnimatorState*)startPin->node->userPtr;
                            auto dstState = (AnimatorState*)endPin->node->userPtr;
                            state->removeTransition(state->findTransition(dstState->getSharedPtr()));
                            m_links.erase(itr);
                        }
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto itr = std::find_if(m_nodes.begin(), m_nodes.end(), [nodeId](auto& node) { return node.id == nodeId; });
                        if (itr != m_nodes.end()) {
                            auto* state = (AnimatorState*)((*itr).userPtr);
                            m_controller->getStateMachine()->removeState(state->getSharedPtr());
                            m_nodes.erase(itr);
                        }
                    }
                }
            }
            ed::EndDelete();
            ImGui::SetCursorScreenPos(cursorTopLeft);
        }
        ed::End();
    }
}
