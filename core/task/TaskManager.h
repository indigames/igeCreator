#pragma once

#include "core/Widget.h"
#include "core/Container.h"

#include <utils/Singleton.h>
#include <taskflow.hpp>

namespace ige::creator
{
    class TaskManager: public ige::scene::Singleton<TaskManager>
    {
    public:        
        virtual ~TaskManager() {}

        tf::Taskflow& getTaskflow() {
            return m_taskflow;
        }
        
        void update() {
            if(!m_taskflow.empty())
            {
                m_executor.run(m_taskflow).wait();
                m_taskflow.clear();
            }
        }

    protected:
        tf::Executor m_executor;
        tf::Taskflow m_taskflow;
    };
}
