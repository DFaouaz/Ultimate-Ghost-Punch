#pragma once
#ifndef ULTIMATE_GHOST_PUNCH_H
#define ULTIMATE_GHOST_PUNCH_H

#include <UserComponent.h>

class RigidBody;
class GhostMovement;
class PlayerAnimController;
class SoundEmitter;

class UltimateGhostPunch : public UserComponent
{
public:
	enum class State { NONE, AVAILABLE, CHARGING, PUNCHING, USED, SUCCESS };

private:
	RigidBody* rigidBody;
	GhostMovement* ghostMovement;
	PlayerAnimController* anim;
	SoundEmitter* soundEmitter;

	Vector3 direction;
	State state;

	float duration;
	float force;
	float ghostSpeed;

	// Speed multiplyer for punch charging speed (from 0.0 to 1.0)
	float chargeSpeedMult;

public:
	UltimateGhostPunch(GameObject* gameObject);
	virtual ~UltimateGhostPunch();

	virtual void start();
	virtual void preUpdate(float deltaTime);
	virtual void handleData(ComponentData* data);

	void charge();
	void aim(double x, double y);
	void ghostPunch();

	const State& getState();
	const Vector3& getDirection();

	bool isPunching();
	void punchSucceeded();
};

#endif