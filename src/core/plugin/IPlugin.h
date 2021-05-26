#pragma once

#include <string>
#include <memory>
#include <vector>

namespace ige::creator
{
    class IPlugin
    {
    public:
        virtual ~IPlugin() {}
        virtual void execute() = 0;

    protected:
        void* userData = nullptr;
    };

    class Pluggable
    {
    public:
        Pluggable() {}

        virtual ~Pluggable()
        {
            removeAllPlugins();
        }
        
        template<typename T, typename... Args>
        std::shared_ptr<T> addPlugin(Args&& ... p_args)
        {
            static_assert(std::is_base_of<IPlugin, T>::value, "T should derive from IPlugin");

            auto newPlugin = std::make_shared<T>(std::forward<Args>(p_args)...);
            m_plugins.push_back(newPlugin);
            return newPlugin;
        }
        
        template<typename T>
        std::shared_ptr<T> getPlugin()
        {
            static_assert(std::is_base_of<IPlugin, T>::value, "T should derive from IPlugin");

            for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it)
            {
                auto result = std::dynamic_pointer_cast<T>(*it);
                if (result) return result;
            }

            return nullptr;
        }

        /**
        * Execute every plugins
        */
        void executePlugins()
        {
            for (auto& plugin : m_plugins)
                plugin->execute();
        }

        /**
        * Remove every plugins
        */
        void removeAllPlugins()
        {
            for (auto& plugin : m_plugins)
                plugin = nullptr;

            m_plugins.clear();
        }

    private:
        std::vector<std::shared_ptr<IPlugin>> m_plugins;
    };
}