#pragma once

#include <memory>

namespace ige::creator
{
    class Canvas;
    class Editor
    {
    public:
        Editor();
        virtual ~Editor();

        virtual void update(float dt);

        virtual void render();
        
        static void handleEvent(const void* event);

    protected:
        virtual void initialize();

    protected:
        std::shared_ptr<Canvas> m_canvas;
    };
}
