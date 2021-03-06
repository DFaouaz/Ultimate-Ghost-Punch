#pragma once
#ifndef GHOST_MOVEMENT_H
#define GHOST_MOVEMENT_H

#include <UserComponent.h>

class RigidBody;

class GhostMovement : public UserComponent
{
private:
	RigidBody* rigidBody;
	float maxSpeed;

protected:
	virtual void start();
	virtual void handleData(ComponentData* data);

public:
	GhostMovement(GameObject* gameObject);
	virtual ~GhostMovement();


	void move(Vector3 dir);
	void stop();

	void setSpeed(float speed);
	float getSpeed() const;

	bool isGhostMoving() const;
};

#endif