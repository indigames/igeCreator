#include <imgui.h>
#include "core/widgets/ComboBox.h"

namespace ige::creator
{
    ComboBox::ComboBox(const int& selectedIdx, bool enable, bool eol)
        : DataWidget<int>(selectedIdx, enable, eol), m_selectedIdx(selectedIdx)
    {
    }

    ComboBox::~ComboBox()
    {
    }

    void ComboBox::addChoice(int idx, const std::string& item)
    {
        m_choices.emplace(idx, item);
    }

    void ComboBox::_drawImpl()
    {
        if (m_choices.find(m_selectedIdx) == m_choices.end())
            m_selectedIdx = m_choices.begin()->first;

        if (ImGui::BeginCombo(getIdAString().c_str(), m_choices[m_selectedIdx].c_str()))
        {
            for (const auto&[key, value] : m_choices)
            {
                bool selected = key == m_selectedIdx;

                if (ImGui::Selectable(value.c_str(), selected))
                {
                    if (!selected)
                    {
                        ImGui::SetItemDefaultFocus();
                        m_selectedIdx = key;
                        this->notifyChange(m_selectedIdx);
                    }
                }
            }
            ImGui::EndCombo();
        }
    }
}
