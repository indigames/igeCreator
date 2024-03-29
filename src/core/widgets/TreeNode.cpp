#include "core/widgets/TreeNode.h"

#include <imgui.h>
#include <algorithm>

namespace ige::creator
{
    TreeNode::TreeNode(const std::string& name, bool isSelected, bool isLeaf, bool opened)
        : m_name(name), m_bIsSelected(isSelected), m_bIsLeaf(isLeaf), m_bIsDefaultOpened(opened), m_color(1.f, 1.f, 1.f, 1.f)
    {
    }

    TreeNode::~TreeNode()
    {
        getOnOpenedEvent().removeAllListeners();
        getOnClosedEvent().removeAllListeners();

        removeAllWidgets();
        removeAllPlugins();
    }

    void TreeNode::draw()
    {
        if (isEnable())
        {
            _drawImpl();

            if (!isEndOfLine())
            {
                ImGui::SameLine();
            }
        }
    }

    void TreeNode::open()
    {
        m_shouldOpen = true;
        m_shouldClose = false;
    }

    void TreeNode::close()
    {
        m_shouldClose = true;
        m_shouldOpen = false;
    }

    void TreeNode::_drawImpl()
    {
        if (m_shouldOpen)
        {
            ImGui::SetNextTreeNodeOpen(true);
            m_shouldOpen = false;
        }
        else if (m_shouldClose)
        {
            ImGui::SetNextTreeNodeOpen(false);
            m_shouldClose = false;
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (m_bIsDefaultOpened) flags |= ImGuiTreeNodeFlags_DefaultOpen;
        if (m_bIsSelected)      flags |= ImGuiTreeNodeFlags_Selected;
        if (m_bIsLeaf)          flags |= ImGuiTreeNodeFlags_Leaf;
        if (m_bIsHighlighted)   flags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Bullet;

        ImGui::PushStyleColor(ImGuiCol_Text, m_color);
        bool opened = ImGui::TreeNodeEx((m_name + getIdAsString()).c_str(), flags);
        ImGui::PopStyleColor();

        executePlugins();

        // Use mouse release to allow drag&drop without changing focus

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
            getOnDoubleClickEvent().invoke(this);
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
            getOnClickEvent().invoke(this);
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
            getOnRightClickEvent().invoke(this);
        }

        if (opened)
        {
            if (!m_opened)
                getOnOpenedEvent().invoke();
            m_opened = true;
            drawWidgets();
            ImGui::TreePop();
        }
        else
        {
            if (m_opened)
                getOnClosedEvent().invoke();
            m_opened = false;
        }
    }
}