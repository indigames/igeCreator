#pragma once

#include "core/Panel.h"
#include "core/layout/Group.h"

#include <components/animation/AnimatorController.h>
#include <components/animation/AnimatorState.h>
using namespace ige::scene;

#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

namespace ige::creator
{
    struct Node;

    enum class NodeType {
        Normal,
        Entry,
        Exit,
        Any
    };

    struct Pin {
        ed::PinId   id;
        ed::PinKind type;
        Node* node;
        Pin(ed::PinId id, ed::PinKind type, Node* node = nullptr)
            : id(id), type(type), node(node)
        {}
    };

    struct Node {
        ed::NodeId id;
        std::string name;
        Pin* inPin;
        Pin* outPin;
        ImColor color;
        ImVec2 position;
        ImVec2 size;
        void* userPtr;

        Node(ed::NodeId id, const std::string& name, ImVec2 position = {}, ImColor color = ImColor(255, 255, 255))
            : id(id), name(name), color(color), inPin(nullptr), outPin(nullptr), userPtr(nullptr)
        {}
    };

    struct Link {
        ed::LinkId id;
        ed::PinId startPinID;
        ed::PinId endPinID;
        ImColor color;
        Link(ed::LinkId id, ed::PinId startPinID, ed::PinId endPinID)
            : id(id), startPinID(startPinID), endPinID(endPinID)
        {}
    };

    struct NodeBuilder
    {
        NodeBuilder(ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0);

        void Begin(ed::NodeId id);
        void End();

        void Input(ed::PinId id);
        void EndInput();

        void Middle();

        void Output(ed::PinId id);
        void EndOutput();

    private:
        enum class Stage
        {
            Invalid,
            Begin,
            Content,
            Input,
            Output,
            Middle,
            End
        };

        bool SetStage(Stage stage);

        void Pin(ed::PinId id, ax::NodeEditor::PinKind kind);
        void EndPin();

        ed::NodeId  CurrentNodeId;
        Stage       CurrentStage;
        ImVec2      NodeMin;
        ImVec2      NodeMax;
        ImVec2      ContentMin;
        ImVec2      ContentMax;
    };

    class AnimatorEditor: public Panel
    {
    public:
        AnimatorEditor(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AnimatorEditor();

        void openAnimator(const std::string& path);
        bool save();

    protected:
        virtual void initialize() override;
        virtual void clear();
        virtual void drawWidgets() override;

        void showLeftPanel();

        void drawLayers();
        void setLayersDirty(bool dirty = true) { m_bLayerDirty = dirty; }

        void drawParameters();
        void setParametersDirty(bool dirty = true) { m_bParameterDirty = dirty; }

        bool isDirty() { return m_bDirty; }
        void setDirty(bool dirty = true);

        Node* createNode(const std::string& name, NodeType type = NodeType::Normal, const ImVec2& position = {});
        Node* findNode(AnimatorState* state);

        int getNextId() { return m_uniqueId++; }
        Node* findNode(ed::NodeId id);
        Link* findLink(ed::LinkId id);
        Pin* findPin(ed::PinId id);

        bool isPinLinked(ed::PinId id);
        bool hasLink(ed::PinId id1, ed::PinId id2);
        bool canCreateLink(Pin* p1, Pin* p2);

        void drawPinIcon(const Pin& pin, bool connected, int alpha);

    protected:
        std::string m_path;
        std::shared_ptr<AnimatorController> m_controller = nullptr;
        std::shared_ptr<IPlugin> m_pyxaDragDropPlugin = nullptr;
        std::shared_ptr<IPlugin> m_animDragDropPlugin = nullptr;
        int m_uniqueId = 0;
        float m_leftPanelWidth = 0.f;

        std::vector<Node> m_nodes;
        std::vector<Link> m_links;
        ed::EditorContext* m_editor = nullptr;
        bool m_bDirty = false;

        std::shared_ptr<Group> m_layerGroup = nullptr;
        bool m_bLayerDirty = false;

        std::shared_ptr<Group> m_parameterGroup = nullptr;
        bool m_bParameterDirty = false;
        int m_selectedType = 0;
    };
}
