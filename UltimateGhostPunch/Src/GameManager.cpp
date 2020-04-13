#include "GameManager.h"
#include <ComponentRegister.h>

#include "Timer.h"

REGISTER_FACTORY(GameManager);

GameManager* GameManager::instance = nullptr;

GameManager::GameManager() : UserComponent(nullptr), paused(false)
{

}

GameManager::GameManager(GameObject* gameObject) : UserComponent(gameObject)
{
	if (instance == nullptr)
		instance = this;
	else
		destroy(gameObject);
}

GameManager::~GameManager()
{
	if (instance == this)
		instance = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return instance;
}

void GameManager::start()
{
	level = "";
	song = "";

	dontDestroyOnLoad(gameObject);
}

void GameManager::setNumPlayers(int nPlayers)
{
	this->numPlayers = nPlayers;
	
}

int GameManager::getNumPlayers()
{
	return numPlayers;
}

void GameManager::setPlayerIndexes(std::vector<int> playerIndexes)
{
	this->playerIndexes = playerIndexes;
}

std::vector<int>& GameManager::getPlayerIndexes()
{
	return playerIndexes;
}

std::vector<GameObject*>& GameManager::getKnights()
{
	return knights;
}

void GameManager::setLevel(std::string level)
{
	this->level = level;
	this->lastLevel = level;
	//Leer archivo config del nivel y guardar las posiciones de players/obst�culos para crearlos
	//Leer el l�mite inferior del nivel: bottomLimit = ...
}

std::string GameManager::getLevel()
{
	return level;
}

std::string GameManager::getLastLevel()
{

	return lastLevel;
}

void GameManager::setSong(std::string song)
{
	this->lastSong = song;
	this->song = song;
}

std::string GameManager::getSong()
{
	return song;
}

std::string GameManager::getLastSong()
{
	return lastSong;
}

void GameManager::setHealth(int health)
{
	this->health = health;
}

int GameManager::getHealth()
{
	return health;
}

void GameManager::setTime(int time)
{
	this->time = time;
	this->maxTime = time;
}

int GameManager::getTime()
{
	return time;
}

int GameManager::getInitialTime()
{
	return maxTime;
}

Score* GameManager::getScore()
{
	return &scores;
}

void GameManager::reset()
{
	Timer::GetInstance()->setTimeScale(1.0f);
	paused = false;
}

void GameManager::pauseGame(bool setPaused)
{
	if (paused == setPaused) return;

	paused = setPaused;

	if (paused)
		Timer::GetInstance()->setTimeScale(0.0f); //Pause the game
	else
		Timer::GetInstance()->setTimeScale(1.0f); //Resume the game
}

bool GameManager::gameIsPaused()
{
	return paused;
}
