#pragma once

#include <unordered_map>
#include <functional>

namespace ige::creator
{
    template<class... Args>
    class Event
    {
    public:
        using Callback = std::function<void(Args...)>;

        uint64_t addListener(Callback callback);

        bool removeListener(uint64_t id);

        void removeAllListeners();

        size_t getListenerCount();

        void invoke(Args... args);

    protected:
        std::unordered_map<uint64_t, Callback>	m_callbacks;
        static uint64_t s_idCounter;
    };
}

#include "EventImpl.hpp"
