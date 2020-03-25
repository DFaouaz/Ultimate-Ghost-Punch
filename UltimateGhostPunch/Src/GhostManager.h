#pragma once
#include <UserComponent.h>

class Movement;
class Health;
class GhostMovement;
class RigidBody;

class GhostManager : public UserComponent
{
private:
	bool ghost, ghostAble;
	float ghostTime = 10;

	Movement* mov;
	GhostMovement* gMov;
	Health* health;
	RigidBody* rb;

public:
	GhostManager(GameObject* gameObject);

	virtual void start();
	virtual void update(float deltaTime);

	virtual void handleData(ComponentData* data);

	virtual void OnObjectEnter(GameObject* other);//Para desactivar el fantasma

	bool isGhost();
	bool hasGhost();

	void activateGhost();
	void deactivateGhost();
};
