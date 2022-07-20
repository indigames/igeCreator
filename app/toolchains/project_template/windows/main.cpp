
#include <windows.h>

#ifdef USE_IGESCENE
    #include "Game.h"
#else
    #include "pythonStarter.h"
#endif

#include <pyxieApplication.h>
#include <pyxieFios.h>
#include <pyxieResourceManager.h>
#include <pyxieSystemInfo.h>
using namespace pyxie;

#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 960

extern std::shared_ptr<pyxieApplication> gApp;

void CreateConsole(void) 
{
    FILE* fp;
    AllocConsole();
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
}

int main(void *data)
{
    CreateConsole();

#ifdef USE_IGESCENE
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

        // Main loop
        while (gApp->isRunning())
        {
            // Update
            gApp->update();

            // Swap buffers
            gApp->swap();
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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    return main((void *)nullptr);
}
