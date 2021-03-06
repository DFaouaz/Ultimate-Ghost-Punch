#pragma once
#ifndef AI_STATE_MACHINE_H
#define AI_STATE_MACHINE_H
#include "StateMachine.h"

class Movement;
class Dodge;
class Jump;
class GhostMovement;
class UltimateGhostPunch;

class PlatformGraph;
class PlatformNavigation;
class PlatformMovement;
class GhostNavigation;
class GhostManager;
class Attack;
class Block;
class Grab;
class FightingState;
class PlayerState;

enum class ActionInput {
	MOVE_RIGHT, MOVE_LEFT, JUMP, CANCEL_JUMP, DODGE, STOP, GHOST_MOVE, GHOST_PUNCH, QUICK_ATTACK, STRONG_ATTACK, BLOCK, UNBLOCK, GRAB, DROP, FACE_RIGHT, FACE_LEFT
};

class AIStateMachine :	public StateMachine
{
public:
private:
	/* Knight it will be focused on */
	GameObject* target;

	/* GRAPH INFO */
	PlatformGraph* platformGraph;

	/* COMPONENTS NEEDED */
	Movement* movement;
	Dodge* dodge;
	Jump* jump;
	GhostMovement* ghostMovement;
	UltimateGhostPunch* ghostPunch;
	GhostManager* ghostManager;
	Attack* attack;
	Block* block;
	Grab* grab;
	PlayerState* playerState;

	/* States */
	PlatformNavigation* platformNavigation;
	PlatformMovement* platformMovement;
	GhostNavigation* ghostNavigation;
	FightingState* fightingState;

	/* Auxiliar variables */
	Vector3 dir;
	float timeTargetChange;
	float timerTargetChange;
	float avoidGhostDist;

protected:
	virtual void start();
	virtual void update(float deltaTime);
	virtual void fixedUpdate(float deltaTime);
	virtual void onCollisionEnter(GameObject* other);

public:
	AIStateMachine(GameObject* gameObject);
	virtual ~AIStateMachine();

	void startPlatformNavigation();
	void startPlatformMovement();
	void startGhostNavigation();
	void startFightingState();
	void startFleeingState(GameObject* fleeTarget);

	void changeTarget();
	void setTarget(GameObject* newTarget);

private:
	virtual void processActionInput();

	/* Call in start function */
	void createPlatformNavigation();
	void createPlatformMovement();
	void createGhostNavigation();
	void createFightingState();

	/* Funcs */
	void selectPlatformState();
	void updateState();
};

#endif