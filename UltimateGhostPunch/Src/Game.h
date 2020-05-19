#pragma once
#ifndef GAME_H
#define GAME_H

#include <UserComponent.h>
#include <UIElement.h>
#include <Vector3.h>

#include <vector>
#include <string>

class InputSystem;
class GameManager;
class SongManager;
class UILayout;
class Countdown;
class CameraEffects;

class Game : public UserComponent
{
private:
	struct LightData
	{
		std::string type;
		Vector3 position;

		float intensity;
		Vector3 colour;
		Vector3 direction;
	};

	GameManager* gameManager;
	SongManager* songManager;

	UILayout* gameLayout;
	Countdown* countdown;

	CameraEffects* cameraEffects;
	bool fadeIn;
	bool darkness;
	bool end;

	UIElement timePanel;

	std::vector<int> playerIndexes;
	std::vector<Vector3> playerColours;

	std::vector<std::pair<Vector3, Vector3>> playerTransforms;
	std::vector<std::pair<Vector3, Vector3>> spikesTransforms;
	std::vector<LightData> lights; // position, intensity, colour, direction

	int nSpikes;
	int nLights;
	int winner;

	float timer; // If time is -1, then infinite

	void setCameraLimits(GameObject* mainCamera);
	void createLevel();
	void createKnights();
	void createSpikes();
	void createLights();
	void createAI();

	void playSong();

	void configureLevelRender(const std::string& name);
	void configureLevelCollider(const std::string& name);

	void chooseWinner();

	std::pair<std::string, std::string> timeToText();

public:
	Game(GameObject* gameObject);
	virtual ~Game();

	virtual void start();
	virtual void update(float deltaTime);

	void playerDie(int index);
	Vector3 getPlayerInitialPosition(int player);
	CameraEffects* getCameraEffects();
};

#endif