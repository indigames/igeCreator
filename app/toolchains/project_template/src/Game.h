#pragma once

#ifdef USE_IGESCENE
#include <utils/PyxieHeaders.h>
#include <event/InputProcessor.h>
using namespace ige::scene;

class Game : public Application
{
public:
    Game();
    virtual ~Game();

    bool init();
    void updateTouch();

    virtual bool onUpdate() override;
    virtual void onRender() override;
    virtual void onShutdown() override;
    virtual void onSize(int w, int h) override;

protected:
    std::shared_ptr<InputProcessor> m_inputProcessor = nullptr;
};
#endif