#pragma once
#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <UserComponent.h>
#include <UIElement.h>

#include <chrono>

class UILayout;
class CameraController;

class Countdown : public UserComponent
{
private:
	UIElement panel;

	std::vector<GameObject*> players;
	CameraController* cameraControl;

	float time;
	std::chrono::steady_clock::time_point last;

	bool startCounting;
	bool countingDown;

protected:
	virtual void start();
	virtual void update(float deltaTime);
	virtual void handleData(ComponentData* data);

public:
	Countdown(GameObject* gameObject);
	virtual ~Countdown();
	bool hasStarted() const;
	bool isCounting() const;

	float getRemainingTime() const;
};

#endif