
#pragma once

#include <mutex>
#include <memory>

namespace ige::creator
{
    template<typename T>
    class Singleton
    {
    protected:        
        Singleton<T>() = default;

        Singleton<T>(const Singleton<T>&) = delete;
        Singleton<T>& operator = (const Singleton<T>&) = delete;

        static std::unique_ptr<T> m_instance;
        static std::once_flag m_once;

    public:
        virtual ~Singleton<T>() = default;

        static T* getInstance()
        {
            std::call_once(m_once, [&]() {
                m_instance.reset(new T());
            });
            return m_instance.get();
        }

        static void setInstance(T* instance) {
            if (instance != nullptr) {
                m_instance.reset(new T());
            }
            else {
                m_instance = nullptr;
            }
        }
    };

    template<typename T> std::unique_ptr<T> Singleton<T>::m_instance;
    template<typename T> std::once_flag Singleton<T>::m_once;
}
