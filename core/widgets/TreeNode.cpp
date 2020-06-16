#include <imgui.h>
#include "core/widgets/TreeNode.h"

namespace ige::creator
{
    TreeNode::TreeNode(const std::string& name, bool isSelected, bool isLeaf)
        : m_name(name), m_bIsSelected(isSelected), m_bIsLeaf(isLeaf)
    {
    }

    TreeNode::~TreeNode()
    {
    }

    void TreeNode::_drawImpl()
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (m_bIsSelected)			flags |= ImGuiTreeNodeFlags_Selected;
        if (m_bIsLeaf)				flags |= ImGuiTreeNodeFlags_Leaf;

        bool opened = ImGui::TreeNodeEx(m_name.c_str(), flags);

        if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
            getOnClickEvent().invoke();

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