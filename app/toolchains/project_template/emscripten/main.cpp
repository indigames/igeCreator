#ifdef USE_IGESCENE
    #include "Game.h"
#else
    #include "pythonStarter.h"
#endif

#include <emscripten.h>
#include <SDL.h>

#include <pyxieApplication.h>
#include <pyxieFios.h>
#include <pyxieResourceManager.h>
#include <pyxieSystemInfo.h>
using namespace pyxie;

#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 960

extern std::shared_ptr<pyxieApplication> gApp;

int main(int argc, char* argv[])
{
#ifdef USE_IGESCENE
    const int FPS = 60;
    const float FRAME_DELAY = 1000.f / FPS;
    uint32_t frameStart, frameDelay;
    
    // Create the game instance
    gApp = std::make_shared<Game>();
    gApp->createAppWindow();

    // Initialize
    if (gApp->isInitialized())
    {
        // Set game size width based
        pyxieSystemInfo::Instance().UseNativeSize();

        // Show window
        gApp->showAppWindow(true, SCREEN_WIDTH, SCREEN_HEIGHT);

        while (gApp->isRunning()) {            
            frameStart = SDL_GetTicks();
            
            // Update
            gApp->update();

            // Swap buffers
            gApp->swap();
            
            frameDelay = FRAME_DELAY - (SDL_GetTicks() - frameStart);
            
            // Sleep
            if(frameDelay <= 0) {
                frameDelay = 1;
            }
            
            emscripten_sleep(frameDelay);
        }
    }

    // Destroy
    gApp->destroy();
    gApp = nullptr;
#else
    pyxieFios::Instance().SetRoot(".");
    pyxieResourceManager::New();

    // Run Python script
    auto pythonStarter = std::make_shared<PythonStarter>();
    pythonStarter->Run();

    // If script exit but app still run, it's a crash
    if (gApp != nullptr && gApp->isRunning()) {
        pythonStarter->ShowError();

        // Destroy
        gApp->destroy();
        gApp = nullptr;
    }
#endif
    return 0;
}
