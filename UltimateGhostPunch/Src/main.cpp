#include "GaiaCore.h"

#include "InterfaceSystem.h"
#include "SceneManager.h"
#include "GameManager.h"

//------------------ EVENTS IMPLEMENTATION (esto ira en un archivo aparte!)

// GENERIC EVENTS
bool backButtonClick()
{
	SceneManager::GetInstance()->changeScene("mainMenu");
	GameManager::GetInstance()->pauseGame(false);
	return false;
}

// MAIN MENU EVENTS
bool singlePlayerButtonClick()
{
	SceneManager::GetInstance()->changeScene("mainScene");
	return false;
}

bool multiplayerButtonClick()
{
	SceneManager::GetInstance()->changeScene("fightConfig");
	return false;
}

bool optionsButtonClick()
{
	SceneManager::GetInstance()->changeScene("options");
	return false;
}

bool exitButtonClick()
{
	//close application
	return false;
}

//-------------

#ifdef _DEBUG
int main()
#else
#include <Windows.h>
int WINAPI
WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	// ---Events registering

	// generic
	InterfaceSystem::GetInstance()->registerEvent("backButtonClick", UIEvent("ButtonClicked", &backButtonClick));

	// mainMenu
	InterfaceSystem::GetInstance()->registerEvent("singlePlayerButtonClick", UIEvent("ButtonClicked", &singlePlayerButtonClick));
	InterfaceSystem::GetInstance()->registerEvent("multiplayerButtonClick", UIEvent("ButtonClicked", &multiplayerButtonClick));
	InterfaceSystem::GetInstance()->registerEvent("optionsButtonClick", UIEvent("ButtonClicked", &optionsButtonClick));
	InterfaceSystem::GetInstance()->registerEvent("exitButtonClick", UIEvent("ButtonClicked", &exitButtonClick));

	// fightConfigMenu
	//InterfaceSystem::GetInstance()->registerEvent("fightButtonClick", UIEvent("ButtonClicked", &fightButtonClick));



	GaiaCore* g = new GaiaCore();
	g->init();
	g->run();
	g->close();
	
	delete g;

	return 0;
}