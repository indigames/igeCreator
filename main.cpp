
#include <windows.h>
#include <SDL.h>
#include <SDL_thread.h>

#include <pyxieApplication.h>
#include <pyxieSystemInfo.h>
#include <pyxieFios.h>
#include <pyxieTime.h>
#include <input/pyxieInputHandler.h>

#include "core/Editor.h"

#define SCREEN_WIDTH	1280
#define SCREEN_HEIGHT	720

void CreateConsole(void) {
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
}

using namespace pyxie;
using namespace ige::creator;

int main(void* data) {
	CreateConsole();

	pyxieFios::Instance().SetRoot(".");

	// Create window
	auto app = std::make_shared<pyxieApplication>();
	app->createAppWindow();	

	// Initialize
	if (app->isInitialized())
	{
		// Create editor instance
		auto editor = Editor::getInstance();
		editor->registerApp(app);
		editor->initialize();

		pyxieSystemInfo::Instance().SetGemeScreenSize(SCREEN_WIDTH);
		app->showAppWindow(true, SCREEN_WIDTH, SCREEN_HEIGHT);
		
		// Register input handler
		app->getInputHandler()->setRawInputHandlerFunc(&Editor::handleEvent);
		
		// Main loop
		while (app->isRunning())
		{
			// Update
			app->update();			
			editor->update((float)pyxieTime::Instance().GetElapsedTime());

			// Render
			editor->render();
			app->swap();
		}
	}

	// Destroy
	app->destroy();
	app = nullptr;
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
