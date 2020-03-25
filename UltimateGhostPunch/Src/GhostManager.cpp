#include "GhostManager.h"

#include <GameObject.h>
#include <sstream>

#include "Movement.h"
#include "GhostMovement.h"
#include "Health.h"
#include "RigidBody.h"
#include "PlayerController.h"
#include "MeshRenderer.h"

GhostManager::GhostManager(GameObject* gameObject) : UserComponent(gameObject)
{

}

void GhostManager::start()
{
	mov = gameObject->getComponent<Movement>();
	gMov = gameObject->getComponent<GhostMovement>();
	health = gameObject->getComponent<Health>();
	rb = gameObject->getComponent<RigidBody>();
	transform = gameObject->transform;
	mesh = gameObject->getComponent<MeshRenderer>();

	ghost = false;
	ghostAble = true;

	// Store some data for player resurrection
	if (rb != nullptr) playerGravity = rb->getGravity().y;
	if (mesh != nullptr) aliveMeshId = mesh->getMeshId();
	if(mesh!= nullptr) aliveMeshName = mesh->getMeshName();
	aliveScale = transform->getScale();
}

void GhostManager::update(float deltaTime)
{
	if (ghost && ghostTime > 0)
		ghostTime -= deltaTime;
	else if (ghost && ghostTime <= 0)
		if (health != nullptr) health->die();

}

void GhostManager::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties()) {
		std::stringstream ss(prop.second);

		if (prop.first == "ghostTime") {
			if (!(ss >> ghostTime))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostMesh") {
			if (!(ss >> ghostMeshId >> ghostMeshName))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostScale") {
			double x, y, z;
			if (!(ss >> x >> y >> z))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
			else
				ghostScale = { x,y,z };
		}
		else if (prop.first == "spawnOffset") {
			double x, y, z;
			if (!(ss >> x >> y >> z))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
			else
				ghostSpawnOffset = { x,y,z };
		}
		else
			LOG("GHOST MANAGER: Invalid property name \"%s\"", prop.first.c_str());
	}
}

void GhostManager::onObjectEnter(GameObject* other)
{
	if (ghost	// If this player is in ghost mode
		&& other->getTag() == "player" // and other is a player
		&& other->getComponent<Health>() != nullptr && other->getComponent<Health>()->getHealth() > 0) { // and it is alive

		deactivateGhost();
	}
}

bool GhostManager::isGhost()
{
	return ghost;
}

bool GhostManager::hasGhost()
{
	return ghostAble;
}

void GhostManager::activateGhost()
{
	ghost = true;
	ghostAble = false;

	if (mov != nullptr) mov->setActive(false);
	if (gMov != nullptr) gMov->setActive(true);
	if (rb != nullptr) {
		rb->setTrigger(true);
		rb->setGravity({ 0,0,0 });
	}

	// Change player's mesh -> ghost mesh
	if(mesh!=nullptr) mesh->changeMesh(ghostMeshId, ghostMeshName);
	// Change scale
	transform->setScale(ghostScale);
	// Apply position offset
	transform->setPosition({ transform->getPosition().x + ghostSpawnOffset.x, transform->getPosition().y + ghostSpawnOffset.y, transform->getPosition().z + ghostSpawnOffset.z });
}

void GhostManager::deactivateGhost()
{
	ghost = false;

	if (mov != nullptr) mov->setActive(true);
	if (gMov != nullptr) gMov->setActive(false);
	if (rb != nullptr) {
		rb->setTrigger(false);
		rb->setGravity({ 0, playerGravity, 0 });
	}

	// Change player's mesh -> alive mesh
	if(mesh!=nullptr) mesh->changeMesh(aliveMeshId, aliveMeshName);
	if (health != nullptr) health->resurrect();
	transform->setScale(aliveScale);
}

