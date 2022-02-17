#pragma once
#include "core/Widget.h"

#include <imgui.h>
#include <limits>

namespace ige::creator
{
    template <typename T, size_t N = 1>
    class Drag: public DataWidget<std::array<T, N>>
    {
        static_assert(std::is_scalar<T>::value, "Invalid Drag<T>, scalar type expected!");

    public:
        Drag(const std::string& label, ImGuiDataType type, const std::array<T, N>& val, float speed = 0.05f, const T& minVal = std::numeric_limits<T>::lowest(), const T& maxVal = std::numeric_limits<T>::max());
        virtual ~Drag();

        ige::scene::Event<const std::array<T, N>&>& getOnDataFinishChangedEvent() { return m_onDataFinishChangedEvent; }
        ige::scene::Event<const std::array<T, N>&>& getOnDataBeginChangedEvent() { return m_onDataBeginChangedEvent; }

    protected:
        virtual void _drawImpl() override;
        virtual void notifyFinishChange(const std::array<T, N>& data);
        virtual void notifyBeginChange(const std::array<T, N>& data);


        std::string m_label;
        ImGuiDataType m_dataType;
        float m_speed;
        T m_min;
        T m_max;
        bool m_dragging;
        ige::scene::Event<const std::array<T, N>&> m_onDataBeginChangedEvent;
        ige::scene::Event<const std::array<T, N>&> m_onDataFinishChangedEvent;
    };
}

#include "core/widgets/Drag.hpp"
