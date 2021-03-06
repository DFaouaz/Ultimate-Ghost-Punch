#include "UltimateGhostPunch.h"

#include <ComponentRegister.h>
#include <GameObject.h>
#include <RigidBody.h>
#include <Scene.h>
#include <Camera.h>
#include <MathUtils.h>
#include <sstream>

#include "Health.h"
#include "GhostMovement.h"

REGISTER_FACTORY(UltimateGhostPunch);

UltimateGhostPunch::UltimateGhostPunch(GameObject* gameObject) : UserComponent(gameObject), rigidBody(nullptr), ghostMovement(nullptr),
direction(Vector3::ZERO), state(State::NONE), duration(0.0f), force(0.0f), ghostSpeed(0.0f), chargeSpeed(0.0f)
{

}

UltimateGhostPunch::~UltimateGhostPunch()
{
	rigidBody = nullptr;
	ghostMovement = nullptr;
}

void UltimateGhostPunch::start()
{
	checkNullAndBreak(gameObject);

	state = AVAILABLE;

	rigidBody = gameObject->getComponent<RigidBody>();
	ghostMovement = gameObject->getComponent<GhostMovement>();

	checkNull(rigidBody);
	checkNullAndBreak(ghostMovement);

	ghostSpeed = ghostMovement->getSpeed();
}

void UltimateGhostPunch::preUpdate(float deltaTime)
{
	checkNullAndBreak(gameObject);

	if (notNull(gameObject->transform) && (state == USED || state == SUCCESS || state == FAIL))
	{
		Vector3 rotation = gameObject->transform->getRotation();
		rotation.z = 0.0;
		gameObject->transform->setRotation(rotation);
		return;
	}

	// Update the cooldown
	if (duration > 0 && state == PUNCHING)
		duration -= deltaTime;
	else if (duration <= 0)
	{
		state = FAIL;
	}
}

void UltimateGhostPunch::postUpdate(float deltaTime)
{
	if (state == SUCCESS || state == FAIL)
		state = USED;
}

void UltimateGhostPunch::handleData(ComponentData* data)
{
	checkNullAndBreak(data);
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "force")
		{
			setFloat(force);
		}
		else if (prop.first == "duration")
		{
			setFloat(duration);
		}
		else if (prop.first == "chargeSpeedMult")
		{
			setFloat(chargeSpeed);
		}
		else
			LOG("ULTIMATE GHOST PUNCH: Invalid property name \"%s\"", prop.first.c_str());
	}
}

void UltimateGhostPunch::charge()
{
	if (state == AVAILABLE)
	{
		state = CHARGING;

		checkNullAndBreak(ghostMovement);
		ghostMovement->setSpeed(ghostMovement->getSpeed() * chargeSpeed);
	}
}

void UltimateGhostPunch::aim(double x, double y)
{
	checkNullAndBreak(gameObject);
	if (x == 0 && y == 0 || state != CHARGING) return;

	direction = { x, y, 0.0 };
	direction.normalize();

	float flippedX = direction.x >= 0 ? 1.0f : -1.0f;
	float flippedY = direction.y >= 0 ? 1.0f : -1.0f;

	if (notNull(gameObject->transform) && direction.x != 0)
	{
		float angle = acos(direction.dot(Vector3::RIGHT * flippedX));
		Vector3 finalDirection = Vector3(0.0, 90.0f * flippedX, angle * RAD_TO_DEG * flippedX * flippedY);

		gameObject->transform->setRotation(finalDirection);
	}
}

void UltimateGhostPunch::ghostPunch()
{
	if (state != CHARGING) return;

	if (notNull(rigidBody)) rigidBody->addImpulse(direction * force);

	if (notNull(ghostMovement)) ghostMovement->setSpeed(ghostSpeed);

	state = PUNCHING;
}

void UltimateGhostPunch::punchSucceeded()
{
	state = SUCCESS;
}

bool UltimateGhostPunch::isUsed() const
{
	return state == USED;
}

bool UltimateGhostPunch::isAiming() const
{
	return state == CHARGING;
}

bool UltimateGhostPunch::isPunching() const
{
	return state == PUNCHING;
}
bool UltimateGhostPunch::punchSuccess() const
{
	return state == SUCCESS;
}

bool UltimateGhostPunch::punchFail() const
{
	return state == FAIL;
}