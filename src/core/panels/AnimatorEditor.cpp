#include "core/panels/AnimatorEditor.h"

#include <components/animation/AnimatorStateMachine.h>
#include <components/animation/AnimatorState.h>
#include <components/animation/AnimatorTransition.h>

#include "core/plugin/DragDropPlugin.h"

#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

namespace ige::creator
{
    // Struct to hold basic information about connection between
    // pins. Note that connection (aka. link) has its own ID.
    // This is useful later with dealing with selections, deletion
    // or other operations.
    struct LinkInfo
    {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
    };

    static ed::EditorContext* g_Context = nullptr;    // Editor context, required to trace a editor state.
    static bool g_FirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
    static ImVector<LinkInfo>   g_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
    static int                  g_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.


    void ImGuiEx_BeginColumn()
    {
        ImGui::BeginGroup();
    }

    void ImGuiEx_NextColumn()
    {
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void ImGuiEx_EndColumn()
    {
        ImGui::EndGroup();
    }

    AnimatorEditor::AnimatorEditor(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {}

    AnimatorEditor::~AnimatorEditor()
    {
        clear(); 
    }

    void AnimatorEditor::clear()
    {
        ed::DestroyEditor(g_Context);
        g_Context = nullptr;

        m_controller = nullptr;
        m_path.clear();
    }

    void AnimatorEditor::initialize()
    {
        clear();

        if (!m_bInitialized) {
			
            m_dragDropPlugin = std::make_shared<DDTargetPlugin<std::string>>(".pyxa");
            std::dynamic_pointer_cast<DDTargetPlugin<std::string>>(m_dragDropPlugin)->getOnDataReceivedEvent().addListener([](const auto& path) {
                //  TODO: catch drag drop target to create new state
                pyxie_printf(path.c_str());
            });

            if (m_controller)
                m_controller = nullptr;
            m_controller = std::make_shared<AnimatorController>(m_path);

            // States -> nodes
            // Transitions -> Links

            //1. Add/remove/update nodes
            // - Drag & Drop
            // - Right click menu


            //2. Add/remove/update links
            //  drag
            // info


            
            m_uniqueId = 1;

            // Init Node Editor
            ed::Config config;
            config.SettingsFile = "AnimatorLayout.ini";
            g_Context = ed::CreateEditor(&config);

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

    void AnimatorEditor::drawWidgets()
    {
        if (!m_bInitialized)
            return;

        ed::SetCurrentEditor(g_Context);

        // Start interaction with editor.
        ed::Begin("Canvas", ImVec2(0.0, 0.0f));

        m_dragDropPlugin->execute();

        for (const auto& state : m_controller->getStateMachine()->getStates()) {
            // Submit Node A
            ed::NodeId nodeId = state->getId();
            ed::PinId inputPinId = state->getInputId();
            ed::PinId outputPinId = state->getOutputId();

            if (g_FirstFrame) {
                auto pos = state->getPosition();
                ed::SetNodePosition(nodeId, ImVec2(pos.X(), pos.Y()));
            }

            ed::BeginNode(nodeId);
            ImGuiEx_BeginColumn();

            if (!state->isEnter()) {
                ed::BeginPin(inputPinId, ed::PinKind::Input);
                ImGui::Text(">");
                ed::EndPin();
                ImGuiEx_NextColumn();
            }
            
            ImGui::Text(state->getName().c_str());

            if (!state->isExit()) {
                ImGuiEx_NextColumn();
                ed::BeginPin(outputPinId, ed::PinKind::Output);
                ImGui::Text(">");
                ed::EndPin();
            }

            ImGuiEx_EndColumn();
            ed::EndNode();

            //for (const auto& transition : state->transitions) {

            //}
        }


        // Submit Links
        for (auto& linkInfo : g_Links)
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        //
        // 2) Handle interactions
        //

        // Handle creation action, returns true if editor want to create new object (node or link)
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ed::AcceptNewItem() return true when user release mouse button.
                    if (ed::AcceptNewItem())
                    {
                        // Since we accepted new link, lets add one to our list of links.
                        g_Links.push_back({ ed::LinkId(g_NextLinkId++), inputPinId, outputPinId });

                        // Draw new link.
                        ed::Link(g_Links.back().Id, g_Links.back().InputId, g_Links.back().OutputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ed::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.


        // Handle deletion action
        if (ed::BeginDelete())
        {
            // There may be many links marked for deletion, let's loop over them.
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ed::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (auto& link : g_Links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            g_Links.erase(&link);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ed::EndDelete(); // Wrap up deletion action

        // End of interaction with editor.
        ed::End();

        if (g_FirstFrame)
            ed::NavigateToContent(0.0f);

        ed::SetCurrentEditor(nullptr);

        g_FirstFrame = false;
    }
}
