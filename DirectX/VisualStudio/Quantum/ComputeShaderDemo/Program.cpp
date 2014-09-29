#include <Windows.h>
#include <memory>
#include <Window\Game.h>
#include "RenderingGame.h"
#include <Exceptions\GameException.h>

#if defined(DEBUG) || defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif

using namespace Library;


int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	std::unique_ptr<Rendering::RenderingGame> game((new Rendering::RenderingGame(instance, L"RenderingClass", L"Real-Time Rendering", showCommand)));

	try
	{
		game->run();
	}
	catch (GameException ex)
	{
		MessageBox(game->windowHandle(), ex.whatw().c_str(), L"DXWindow", MB_ABORTRETRYIGNORE);
	}

	return 0;
}