#include "event/Event.h"

namespace ige::creator
{
    template<class... Args>
    uint64_t Event<Args...>::s_idCounter = 0;
        
    template<class... Args>
    uint64_t Event<Args...>::addListener(Callback callback)
    {
        m_callbacks.emplace(s_idCounter, callback);
        return s_idCounter++;
    }

    template<class... Args>
    bool Event<Args...>::removeListener(uint64_t id)
    {
        return (m_callbacks.erase(id) > 0);
    }

    template<class... Args>
    void Event<Args...>::removeAllListeners()
    {
        m_callbacks.clear();
    }

    template<class... Args>
    size_t Event<Args...>::getListenerCount()
    {
        return m_callbacks.size();
    }

    template<class... Args>
    void Event<Args...>::invoke(Args... args)
    {
        for (auto const& [key, value] : m_callbacks)
			value(args...);
    }
}