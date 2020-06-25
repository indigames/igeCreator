#include <imgui.h>
#include <algorithm>

#include "core/widgets/TreeNode.h"

namespace ige::creator
{
    TreeNode::TreeNode(const std::string& name, bool isSelected, bool isLeaf, bool opened)
        : m_name(name), m_bIsSelected(isSelected), m_bIsLeaf(isLeaf), m_bIsDefaultOpened(opened)
    {
    }

    TreeNode::~TreeNode()
    {
        getOnOpenedEvent().removeAllListeners();
        getOnClosedEvent().removeAllListeners();
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

    void TreeNode::_drawImpl()
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
        if (m_bIsDefaultOpened) flags |= ImGuiTreeNodeFlags_DefaultOpen;
        if (m_bIsSelected)      flags |= ImGuiTreeNodeFlags_Selected;
        if (m_bIsLeaf)          flags |= ImGuiTreeNodeFlags_Leaf;

        bool opened = ImGui::TreeNodeEx((m_name + m_id).c_str(), flags);

        executePlugins();

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