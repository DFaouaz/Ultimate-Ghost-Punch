#include "GhostManager.h"
#include <ComponentRegister.h>
#include <GameObject.h>
#include <sstream>

#include "Respawn.h"
#include "Movement.h"
#include "GhostMovement.h"
#include "Health.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "PlayerUI.h"
#include "FightManager.h"

REGISTER_FACTORY(GhostManager);

GhostManager::GhostManager(GameObject* gameObject) : UserComponent(gameObject),deathPosChanged(false), ghost(false), used(false), movement(nullptr), ghostMovement(nullptr), health(nullptr),
													 transform(nullptr), meshRenderer(nullptr), rigidBody(nullptr), fightManager(nullptr), resurrectionHealth(2),aliveMeshId(""), aliveMeshName(""),
													 ghostMeshId(""), ghostMeshName(""), playerGravity(-10.0f), ghostTime(10.0f), ghostDamage(1), aliveScale(Vector3()), ghostScale(Vector3()), 
												     deathPosition(Vector3()), ghostSpawnOffset(Vector3())
{

}

GhostManager::~GhostManager()
{

}

void GhostManager::start()
{
	movement = gameObject->getComponent<Movement>();
	ghostMovement = gameObject->getComponent<GhostMovement>();
	health = gameObject->getComponent<Health>();
	transform = gameObject->transform;
	meshRenderer = gameObject->getComponent<MeshRenderer>();
	rigidBody = gameObject->getComponent<RigidBody>();

	GameObject* aux = findGameObjectWithName("FightManager");
	if (aux != nullptr) fightManager = aux->getComponent<FightManager>();

	// Store some data for player resurrection
	if (meshRenderer != nullptr) aliveMeshId = meshRenderer->getMeshId();
	if (meshRenderer != nullptr) aliveMeshName = meshRenderer->getMeshName();
	if (rigidBody != nullptr) playerGravity = rigidBody->getGravity().y;
	if (transform != nullptr) aliveScale = transform->getScale();
}

void GhostManager::update(float deltaTime)
{
	if (health != nullptr && !health->isAlive())
	{
		if (!used && !ghost)
			activateGhost();
		else if (used && !ghost && fightManager != nullptr)
			fightManager->playerDie();
	}

	if (ghost)
	{
		if (ghostTime > 0)
			ghostTime -= deltaTime;
		else if(fightManager != nullptr)
			fightManager->playerDie();
	}
}

void GhostManager::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "ghostTime")
		{
			if (!(ss >> ghostTime))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostDamage")
		{
			if (!(ss >> ghostDamage))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "resurrectionHealth")
		{
			if (!(ss >> resurrectionHealth))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostMesh")
		{
			if (!(ss >> ghostMeshId >> ghostMeshName))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostScale")
		{
			if (!(ss >> ghostScale.x >> ghostScale.y >> ghostScale.z))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "spawnOffset")
		{
			if (!(ss >> ghostSpawnOffset.x >> ghostSpawnOffset.y >> ghostSpawnOffset.z))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "deathPosition")
		{
			if (!(ss >> deathPosition.x >> deathPosition.y >> deathPosition.z))
				LOG("GHOST MANAGER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else
			LOG("GHOST MANAGER: Invalid property name \"%s\"", prop.first.c_str());
	}
}

void GhostManager::onObjectEnter(GameObject* other)
{
	// If this player is in ghost mode and other is a player
	if (ghost && other->getTag() == "Player")
	{
		Health* aux = other->getComponent<Health>();

		//If the other player is alive
		if (aux != nullptr && aux->isAlive())
		{
			aux->receiveDamage(ghostDamage);

			deactivateGhost();
		}
	}
}

bool GhostManager::isGhost()
{
	return ghost;
}

void GhostManager::activateGhost()
{
	ghost = true;
	used = true;

	if (movement != nullptr) movement->setActive(false);
	if (ghostMovement != nullptr) ghostMovement->setActive(true);

	if (rigidBody != nullptr){
		rigidBody->setTrigger(true);
		rigidBody->setGravity({ 0,0,0 });
	}

	// Change player's mesh -> ghost mesh
	if (meshRenderer != nullptr) meshRenderer->changeMesh(ghostMeshId, ghostMeshName);

	if (transform != nullptr) {
		if (!deathPosChanged) deathPosition = transform->getPosition();
		
		// Change scale
		transform->setScale(ghostScale);

		// Apply position offset
		transform->setPosition(transform->getPosition() + ghostSpawnOffset);
	}
}

void GhostManager::deactivateGhost()
{
	ghost = false;

	if (movement != nullptr) movement->setActive(true);
	if (ghostMovement != nullptr) ghostMovement->setActive(false);

	if (rigidBody != nullptr)
	{
		rigidBody->setTrigger(false);
		rigidBody->setGravity({ 0, playerGravity, 0 });
	}

	// Change player's mesh -> alive mesh
	if (meshRenderer != nullptr) meshRenderer->changeMesh(aliveMeshId, aliveMeshName);

	// Change scale
	if(transform!= nullptr) transform->setScale(aliveScale);

	//Set the player alive
	if (health != nullptr) {
		health->setAlive(true);
		health->setHealth(resurrectionHealth);
	}

	//Respawn the player
	Respawn* respawn = gameObject->getComponent<Respawn>();
	if (respawn != nullptr) respawn->spawn(deathPosition);
}

void GhostManager::setDeathPosition(const Vector3& dPos)
{
	deathPosChanged = true;
	deathPosition = dPos;
}