#pragma once

#include "core/Panel.h"

#include <components/animation/AnimatorController.h>
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

        Node(ed::NodeId id, const std::string& name, ImVec2 position = {}, ImColor color = ImColor(255, 255, 255))
            : id(id), name(name), color(color), inPin(nullptr), outPin(nullptr)
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

        void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
        void EndHeader();

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
            Header,
            Content,
            Input,
            Output,
            Middle,
            End
        };

        bool SetStage(Stage stage);

        void Pin(ed::PinId id, ax::NodeEditor::PinKind kind);
        void EndPin();

        ImTextureID HeaderTextureId;
        int         HeaderTextureWidth;
        int         HeaderTextureHeight;
        ed::NodeId  CurrentNodeId;
        Stage       CurrentStage;
        ImU32       HeaderColor;
        ImVec2      NodeMin;
        ImVec2      NodeMax;
        ImVec2      HeaderMin;
        ImVec2      HeaderMax;
        ImVec2      ContentMin;
        ImVec2      ContentMax;
        bool        HasHeader;
    };

    class AnimatorEditor: public Panel
    {
    public:
        AnimatorEditor(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AnimatorEditor();

        void openAnimator(const std::string& path);

    protected:
        virtual void initialize() override;
        virtual void clear();
        virtual void drawWidgets() override;

        Node* createNode(const std::string& name, NodeType type = NodeType::Normal, const ImVec2& position = {});

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
        std::shared_ptr<IPlugin> m_dragDropPlugin = nullptr;
        int m_uniqueId = 0;

        std::vector<Node> m_nodes;
        std::vector<Link> m_links;
        ed::EditorContext* m_editor = nullptr;
    };
}
