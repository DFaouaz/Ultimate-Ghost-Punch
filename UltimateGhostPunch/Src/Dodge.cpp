#include "Dodge.h"
#include <ComponentRegister.h>
#include <GameObject.h>
#include <RigidBody.h>
#include <SoundEmitter.h>
#include <sstream>

#include "PlayerState.h"

REGISTER_FACTORY(Dodge);

Dodge::Dodge(GameObject* gameObject) : UserComponent(gameObject), rigidBody(nullptr), state(IDLE), playerGravity(0, -10, 0), cooldown(1.0f), force(10.0f), time(0.0f), duration(0.25f), atenuation(0.3f)
{

}

Dodge::~Dodge()
{
	rigidBody = nullptr;
}

void Dodge::start()
{
	checkNullAndBreak(gameObject);

	rigidBody = gameObject->getComponent<RigidBody>();
	checkNullAndBreak(rigidBody);

	playerGravity = rigidBody->getGravity();
}

void Dodge::update(float deltaTime)
{
	if (state != State::IDLE)
	{
		if (time > 0.0f) time -= deltaTime;

		if (time <= 0.0f)
		{
			if (state == State::DODGING)
				endDodge();
			else
				state = State::IDLE;
		}
	}
}


void Dodge::handleData(ComponentData* data)
{
	checkNullAndBreak(data);
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "force")
		{
			setFloat(force);
		}
		else if (prop.first == "cooldown")
		{
			setFloat(cooldown);
		}
		else if (prop.first == "duration")
		{
			setFloat(duration);
		}
		else if (prop.first == "atenuation")
		{
			setFloat(atenuation);
		}
		else
			LOG("DODGE: Invalid property name %s", prop.first.c_str());
	}
}

void Dodge::dodge()
{
	checkNullAndBreak(gameObject);

	PlayerState* aux = gameObject->getComponent<PlayerState>();

	if (state == State::IDLE && notNull(aux) && aux->canDodge())
	{
		Vector3 dir = Vector3::ZERO;
		if (notNull(gameObject->transform)) dir.x = (gameObject->transform->getRotation().y > 0) ? 1 : -1;

		if (notNull(rigidBody))
		{
			rigidBody->setGravity({ 0,0,0 });
			rigidBody->setLinearVelocity({ 0,0,0 });
			rigidBody->addImpulse(dir * force);
		}

		state = State::DODGING;
		time = duration;
	}
}

void Dodge::endDodge()
{
	state = State::CD;
	time = cooldown;

	if (notNull(rigidBody))
	{
		rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() * atenuation);
		rigidBody->setGravity(playerGravity);
	}
}

bool Dodge::isDodging()
{
	return state == State::DODGING;
}

bool Dodge::isOnCooldown()
{
	return state != IDLE;
}
