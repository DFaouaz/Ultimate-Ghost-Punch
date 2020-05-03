#include "Grab.h"

#include <GameObject.h>
#include <RigidBody.h>
#include <sstream>

#include "Block.h"
#include "PlayerAnimController.h"

#include "ComponentRegister.h"
#include "GameManager.h"
#include "Score.h"
#include "PlayerIndex.h"
#include "MeshRenderer.h"
#include "Block.h"
#include "Dodge.h"
#include "Attack.h"
#include "GhostManager.h"

REGISTER_FACTORY(Grab);

Grab::Grab(GameObject* gameObject) : UserComponent(gameObject), grabDuration(1.5f), freezeDuration(1.0f), remain(0.0f), throwForce(15.0f), state(IDLE), enemy(nullptr),
controller(nullptr), enemyController(nullptr), enemyDiff(Vector3()), cooldown(2.00f), grabTimer(0.0f), grabVerticalOffset(3.0f), dropHorizontalOffset(0.50f), prevOrientation(1), enemyFollowingThreshold(0.3f),
dodge(nullptr), block(nullptr)
{

}

void Grab::start()
{
	parent = gameObject->getParent();
	if (parent != nullptr) {
		controller = parent->getComponent<PlayerController>();
		id = parent->getComponent<PlayerIndex>()->getIndex();
		dodge = parent->getComponent<Dodge>();
		myAnim = parent->getComponent<PlayerAnimController>();

		std::vector<GameObject*> aux = parent->findChildrenWithTag("groundSensor");
		if (aux.size() > 0) block = aux[0]->getComponent<Block>();

		aux = parent->findChildrenWithTag("attackSensor");
		if (aux.size() > 0) attack = aux[0]->getComponent<Attack>();

	}

	score = GameManager::GetInstance()->getScore();
}

void Grab::update(float deltaTime)
{
	int newOrientation = (parent->transform->getRotation().y >= 0) ? 1 : -1;

	if (remain > 0.0f) remain -= deltaTime;

	if (grabTimer > 0.0f) grabTimer -= deltaTime;

	if (state == GRABBED) {
		if (enemy != nullptr && parent != nullptr)
		{
			Vector3 objPos;
			float dist = (grabbedPosition - enemy->transform->getPosition()).magnitude();
			if (!enemyFollowing && dist > enemyFollowingThreshold)
			{
				objPos = enemy->transform->getPosition();
				objPos.lerp(grabbedPosition, 0.2f);
			}
			else if (dist <= enemyFollowingThreshold)
			{
				grabbedPosition = parent->transform->getPosition() + Vector3(0, parent->transform->getScale().y * grabVerticalOffset, 0);
				objPos = grabbedPosition;
			}

			enemy->transform->setPosition(objPos);
			if (newOrientation != prevOrientation)
				enemy->transform->setRotation({ 0,double(90 * newOrientation),0 });
		}
	}

	if (remain <= 0.0f && state == GRABBED) {
		drop();

		//Reset state
		enemyController = nullptr;
		enemy = nullptr;
		state = IDLE;
	}
	else if (remain <= 0.0f && state == BLOCKED) {
		if (controller != nullptr)controller->setActive(true);
		state = IDLE;
	}

	prevOrientation = newOrientation;
}

void Grab::onObjectStay(GameObject* other)
{
	if (other->getTag() == "Player") {
		GhostManager* enemyGM = other->getComponent<GhostManager>();//Si no es un fantasma
		if (enemyGM != nullptr && enemyGM->isGhost()) {
			if (enemy == other) enemy = nullptr;
		}
		else if (enemy == nullptr && parent != nullptr && other != parent)//If it hits a player different than myself
			enemy = other;
	}
}

void Grab::onObjectEnter(GameObject* other)
{
	if (other->getTag() == "Player") {
		GhostManager* enemyGM = other->getComponent<GhostManager>();//Si no es un fantasma
		if (enemyGM != nullptr && enemyGM->isGhost())
			if (parent != nullptr && other != parent)//If it hits a player different than myself
				enemy = other;
	}
}

void Grab::onObjectExit(GameObject* other)
{
	if (other == enemy && state != GRABBED)
		enemy = nullptr;
}

void Grab::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties()) {
		std::stringstream ss(prop.second);

		if (prop.first == "freezeDuration")
		{
			setFloat(freezeDuration);
		}
		else if (prop.first == "throwForce") {
			setFloat(throwForce);
		}
		else if (prop.first == "grabDuration") {
			setFloat(grabDuration);
		}
		else if (prop.first == "cooldown") {
			if (!(ss >> cooldown))
				LOG("GRAB: Invalid value for property %s", prop.first.c_str());
		}
		else
			LOG("GRAB: Invalid property name %s", prop.first.c_str());
	}
}

void Grab::grab()
{
	if (state == IDLE && grabTimer <= 0 && canGrab())
	{
		if (enemy != nullptr) grabEnemy();
		else grabMissed();

		grabTimer = cooldown;
	}
}

void Grab::drop()
{
	if (state != GRABBED || enemy == nullptr) return;

	resetEnemy();

	//Throw enemy
	Vector3 dir = Vector3(0, 0, 0);
	dir.x = (parent->transform->getRotation().y >= 0) ? 1 : -1;
	enemy->transform->setPosition(parent->transform->getPosition() + Vector3((parent->transform->getScale().x / 2) + (dropHorizontalOffset * dir.x), enemy->transform->getPosition().y - parent->transform->getPosition().y, 0));
	RigidBody* enemyRb = enemy->getComponent<RigidBody>();
	if (enemyRb != nullptr) enemyRb->addImpulse(dir * throwForce);

	//Reset state
	enemyController = nullptr;
	enemy = nullptr;
	state = IDLE;
	grabTimer = cooldown;
}

void Grab::grabMissed()
{
	if (myAnim != nullptr) myAnim->grabFailedAnimation();
	state = IDLE;
}

bool Grab::isGrabbing() const
{
	return state == GRABBED;
}

bool Grab::isOnCooldown() const
{
	return cooldown > 0;
}

bool Grab::canGrab() const
{
	return (block == nullptr || !block->blocking()) && (dodge == nullptr || !dodge->isDodging()) && (attack == nullptr || !attack->isAttacking());
}

void Grab::resetEnemy()
{
	grabTimer = cooldown;
	if (enemy == nullptr) return;

	//Reactivate enemy's RigidBody
	RigidBody* enemyRB = enemy->getComponent<RigidBody>();
	if (enemyRB != nullptr) { enemyRB->setTrigger(false); enemyRB->setActive(true); }

	//Return control to the enemy
	if (enemyController != nullptr)enemyController->setActive(true);

	if (enemyAnim != nullptr) enemyAnim->thrownAwayAnimation();

	//Play throw animation
	if (myAnim != nullptr) myAnim->throwEnemyAnimation();

	grabbedPosition = Vector3();
}

void Grab::grabEnemy()
{
	if (enemy == nullptr || parent == nullptr) return;

	enemyAnim = enemy->getComponent<PlayerAnimController>();


	//Check if we have been blocked
	std::vector<GameObject*> aux = enemy->findChildrenWithTag("groundSensor");
	Block* enemyBlock = nullptr;
	if (aux.size() > 0) enemyBlock = aux[0]->getComponent<Block>();


	if (enemyBlock != nullptr && enemyBlock->getGrabBlock()) {
		int dir = (parent->transform->getRotation().y >= 0) ? 1 : -1;
		int enemyDir = (enemy->transform->getRotation().y >= 0) ? 1 : -1;
		if (dir != enemyDir) // ONLY BLOCK if blocking dir is correct
		{
			LOG("GRAB BLOCKED!");
			state = BLOCKED;
			remain = freezeDuration;
			if (controller != nullptr)controller->setActive(false);//freeze our character


			if (enemyAnim != nullptr) enemyAnim->blockedEnemyGrabAnimation();
			if (myAnim != nullptr) myAnim->enemyBlockedMyGrabAnimation();

			return;
		}


	}

	//Grab the enemy
	if (score != nullptr)
		score->grabbedBy(enemy->getComponent<PlayerIndex>()->getIndex(), id);

	state = GRABBED;
	remain = grabDuration;
	enemyController = enemy->getComponent<PlayerController>();
	enemyDiff = enemy->transform->getPosition() - parent->transform->getPosition();
	if (enemyController != nullptr) enemyController->setActive(false);//freeze the enemy
	grabbedPosition = parent->transform->getPosition() + Vector3(0, parent->transform->getScale().y * grabVerticalOffset, 0);

	RigidBody* enemyRB = enemy->getComponent<RigidBody>();
	if (enemyRB != nullptr) { enemyRB->setTrigger(true); enemyRB->setActive(false); }


	if (myAnim != nullptr) myAnim->grabAnimation();
	if (enemyAnim != nullptr) enemyAnim->grabbedByEnemyAnimation();
}
