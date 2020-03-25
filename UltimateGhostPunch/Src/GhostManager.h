#pragma once
#include <UserComponent.h>
#include <string>

class Movement;
class Health;
class GhostMovement;
class RigidBody;
class Transform;
class MeshRenderer;

class GhostManager : public UserComponent
{
private:
	bool ghost, ghostAble;
	float ghostTime = 10;
	Vector3 ghostSpawnOffset = { 0,0,0 };

	Movement* mov;
	GhostMovement* gMov;
	Health* health;
	RigidBody* rb;
	Transform* transform;
	MeshRenderer* mesh;

	std::string ghostMeshId, ghostMeshName;
	Vector3 ghostScale = { 1,1,1 };
	std::string aliveMeshId, aliveMeshName;
	Vector3 aliveScale;
	float playerGravity;

public:
	GhostManager(GameObject* gameObject);

	virtual void start();
	virtual void update(float deltaTime);

	virtual void handleData(ComponentData* data);

	virtual void onObjectEnter(GameObject* other);

	bool isGhost();
	bool hasGhost();

	void activateGhost();
	void deactivateGhost();

};

