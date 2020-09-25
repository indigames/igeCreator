#pragma once

#include <utils/PyxieHeaders.h>

class Game : public Application
{
public:
    Game();
    virtual ~Game();

    virtual bool onInit(DeviceHandle dh) override;
    virtual bool onUpdate() override;
    virtual void onRender() override;
    virtual void onShutdown() override;

protected:
};