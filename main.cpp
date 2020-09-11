
#include <windows.h>
#include <SDL.h>
#include <SDL_thread.h>

#include <utils/PyxieHeaders.h>
#include "core/Editor.h"

#define SCREEN_WIDTH	1920
#define SCREEN_HEIGHT	1080

using namespace ige::creator;

extern std::shared_ptr<Application> gApp = nullptr;

int main(void* data) {
	FileIO::Instance().SetRoot(".");

	// Create window
	gApp = std::make_shared<Application>();
	gApp->createAppWindow();

	// Initialize
	if (gApp->isInitialized())
	{
		// Create editor instance
		auto& editor = Editor::getInstance();
		editor->registerApp(gApp);
		editor->initialize();

		// Set game size width based
		SystemInfo::Instance().SetGemeScreenSize(SCREEN_WIDTH);

		// Show window
		gApp->showAppWindow(true, SCREEN_WIDTH, SCREEN_HEIGHT, false, true);
		
		// Register input handler
		gApp->getInputHandler()->setRawInputHandlerFunc(&Editor::handleEvent);
		
		// Main loop
		while (gApp->isRunning())
		{
			// Update
			gApp->update();
			editor->update((float)Time::Instance().GetElapsedTime());

			// Render
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
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					_In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine,
					_In_ int nCmdShow)
{
	auto mainThread = SDL_CreateThreadWithStackSize(main, "MainThread", 32 * 1024 * 1024, (void*)nullptr);
	SDL_WaitThread(mainThread, NULL);
	return 0;
}
