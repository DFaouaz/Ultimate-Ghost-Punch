#include "PlayerIndex.h"

#include <ComponentRegister.h>
#include <GameObject.h>
#include <sstream>

REGISTER_FACTORY(PlayerIndex);

PlayerIndex::PlayerIndex(GameObject* gameObject) :UserComponent(gameObject), index(0), pos(0)
{

}

PlayerIndex::~PlayerIndex()
{

}

void PlayerIndex::handleData(ComponentData* data)
{
	checkNullAndBreak(data);
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "index")
		{
			setInt(index);
		}
		else
			LOG("PLAYER INDEX: Invalid property name \"%s\"", prop.first.c_str());
	}
}

int PlayerIndex::getIndex()
{
	return index;
}

void PlayerIndex::setIndex(int index)
{
	this->index = index;
}

int PlayerIndex::getPos()
{
	return pos;
}

void PlayerIndex::setPos(int pos)
{
	this->pos = pos;
}