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

        void addTask(const std::function<void()>& task)
        {
            functors.push_back(task);
        }
        
        void update() {
            for (auto fun: functors) fun();
            functors.clear();
        }

    protected:
        std::vector<std::function<void()>> functors;
    };
}
