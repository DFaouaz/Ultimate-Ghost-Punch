#pragma once
#ifndef GHOST_MANAGER_H
#define GHOST_MANAGER_H

#include <UserComponent.h>

class Game;
class Transform;
class MeshRenderer;
class RigidBody;
class Movement;
class GhostMovement;
class Health;
class PlayerUI;
class PlayerController;
class CameraEffects;
class Camera;
class PlayerState;

class GhostManager : public UserComponent
{
private:
	bool used;
	bool success;
	bool positionChanged;
	
	enum GhostMode
	{
		RESURRECT, ALIVE, DYING, APPEAR, GHOST, DISAPPEAR, DEAD
	};

	float resurrectTime;
	float dyingTime;
	float appearTime;
	float disappearTime;

	float ghostTime;
	float playerGravity;

	int ghostDamage;
	int resurrectionHealth;

	Game* game;
	Transform* transform;
	MeshRenderer* meshRenderer;
	RigidBody* rigidBody;
	Movement* movement;
	GhostMovement* ghostMovement;
	Health* health;
	PlayerUI* playerUI;
	PlayerState* playerState;

	Camera* cam;
	CameraEffects* cameraEffects;

	Vector3 aliveScale;
	Vector3 ghostScale;

	Vector3 playerColour;
	Vector3 deathPosition;
	Vector3 spawnOffset;

	GhostMode mode;

protected:
	virtual void start();
	virtual void update(float deltaTime);
	virtual void handleData(ComponentData* data);
	virtual void onObjectStay(GameObject* other);

public:
	GhostManager(GameObject* gameObject);
	virtual ~GhostManager();

	bool isGhost() const;

	bool ghostUsed() const;
	bool ghostSuccess() const;
	bool ghostDeath() const;

	float getGhostTime() const;

	void setPlayerColour(const Vector3& colour);
	void setDeathPosition(const Vector3& position);

	void activateGhost();
	void deactivateGhost();
	void deactivatePlayer();

	bool isResurrecting() const;
	bool isDying() const;
	bool isAppearing() const;
	bool isDisappearing() const;
	bool isDead() const;

private:
	void handleStates(float deltaTime);
};

#endif