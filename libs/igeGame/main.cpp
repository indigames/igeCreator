
#include <windows.h>
#include <utils/PyxieHeaders.h>

#include "Game.h"

#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 960

extern std::shared_ptr<Application> gApp = nullptr;

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


    // Create the game instance
    gApp = std::make_shared<Game>();
    gApp->createAppWindow();

    // Initialize
    if (gApp->isInitialized())
    {
        // Set game size width based
        SystemInfo::Instance().SetGemeScreenSize(SCREEN_WIDTH);

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
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    return main((void *)nullptr);
}
