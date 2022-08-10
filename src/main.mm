
#include "main.h"

#include <utils/PyxieHeaders.h>
#include "core/Editor.h"

using namespace ige::creator;

#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#import <Cocoa/Cocoa.h>
#include <string>
#include <fstream>
#include <ftw.h>
#include <sys/stat.h>

#define SDL_MAIN_AVAILABLE
#include <SDL.h>

std::string getDocumentPath() {
    @autoreleasepool {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex:0];
        return std::string([documentsDir UTF8String]);
    }
}

std::string getBundlePath() {
    @autoreleasepool {

        NSString *bundleDir = [[NSBundle mainBundle] resourcePath];
        return std::string([bundleDir UTF8String]);
    }
}

#ifdef __cplusplus
extern "C" {
#endif

extern std::shared_ptr<pyxieApplication> gApp;

int main(int argc, char *argv[])
{
    @autoreleasepool {
        pyxieFios::Instance().SetRoot(getBundlePath().c_str());

        // Set game size width based
        pyxieSystemInfo::Instance().UseNativeSize();

        // Create the game instance
        gApp = std::make_shared<Application>();
        gApp->showAppWindow(true, -1, -1, true, true);

        // Initialize
        if(gApp->isInitialized())
        {
            // Create editor instance
            auto& editor = Editor::getInstance();
            editor->registerApp(gApp);
            editor->initialize();

            // Register input handler
            gApp->getInputHandler()->setRawInputHandlerFunc(&Editor::handleEvent);

            // Main loop
            while (gApp->isRunning())
            {
                // Update
                gApp->update();
                editor->update((float)Time::Instance().GetElapsedTime());

                // Swap buffers
                editor->render();
                gApp->swap();
            }

            // Destroy
            Editor::destroy();
            editor = nullptr;
        }

        // Destroy
        gApp->destroy();
        gApp = nullptr;
    }
    return NSApplicationMain(argc, (const char **)argv);
}
#ifdef __cplusplus
}
#endif
