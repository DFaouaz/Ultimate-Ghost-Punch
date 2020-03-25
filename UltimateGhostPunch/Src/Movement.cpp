#include "Movement.h"
#include <sstream>
#include <RigidBody.h>
#include <GameObject.h>

Movement::Movement(GameObject* gameObject) : UserComponent(gameObject)
{

}

void Movement::move(Vector3 dir)
{
	if(rigidBody != nullptr) rigidBody->addForce(dir * force);
}

void Movement::start()
{
	rigidBody = gameObject->getComponent<RigidBody>();
}

void Movement::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "force")
		{
			if(!(ss >> force))
				LOG("MOVEMENT: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else
			LOG("MOVEMENT: Invalid property name \"%s\"", prop.first.c_str());
	}
}