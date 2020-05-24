#pragma once
#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "Menu.h"

#include <UIElement.h>

#include <vector>
#include <string>

const int MAX_VALUE = 100;

class RenderSystem;
class SoundSystem;
class WindowManager;

class OptionsMenu : public Menu
{
protected:
	RenderSystem* renderSystem;
	SoundSystem* soundSystem;
	WindowManager* windowManager;

	UIElement applyButton;
	UIElement restoreButton;

	UIElement brightnessScroll;
	UIElement soundScroll;
	UIElement musicScroll;

	UIElement resolutionText;
	UIElement fullscreenText;
	UIElement brightnessText;
	UIElement soundText;
	UIElement musicText;

	UIElement root;

	float brightness;
	float soundVolume;
	float musicVolume;

	bool fullscreen;
	int resolution;
	int initialResolution;
	int currentResolution;

	std::vector<std::string> resolutionNames;
	std::vector<std::string> screenNames;
	std::vector<std::pair<unsigned int, unsigned int>> resolutions;

protected:
	bool applyButtonClick();
	bool restoreButtonClick();

	bool changeResolution(int value);
	bool changeFullscreen(int value);

	bool changeBrightness();
	bool changeSoundVolume();
	bool changeMusicVolume();

	bool checkControllersInput();

	virtual void start();
	virtual void update(float deltaTime);

public:
	OptionsMenu(GameObject* gameObject);
	virtual ~OptionsMenu();
};

#endif