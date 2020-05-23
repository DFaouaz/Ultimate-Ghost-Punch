#include "Credits.h"
#include <ComponentRegister.h>
#include <InputSystem.h>
#include <InterfaceSystem.h>
#include <SceneManager.h>
#include <GameObject.h>
#include <UILayout.h>

REGISTER_FACTORY(Credits);

bool Credits::checkControllersInput()
{
	bool result = false;

	int i = 0;
	while (i < 4 && !result)
	{
		if (notNull(inputSystem) && inputSystem->getButtonPress(i, "B"))
			result = true;

		i++;
	}

	return result;
}

Credits::Credits(GameObject* gameObject) : Menu(gameObject)
{
	names = { "Mart�n Aritz Amechazurra Falag�n\n\n", "Alejandro Ans�n Alcolea\n\n", "Dany Faouaz Santillana\n\n", "Ismael Fern�ndez Pereira\n\n",
		"Arturo Garc�a C�rdenas\n\n", "Daniel Gil Aguilar\n\n", "�lvaro Poyatos Morate\n\n", "Mario Tabasco Vargas" };
}

Credits::~Credits()
{

}

void Credits::start()
{
	Menu::start();

	GameObject* camera = findGameObjectWithName("MainCamera");
	if (notNull(camera))
	{
		UILayout* layout = camera->getComponent<UILayout>();
		if (notNull(layout))
		{
			std::string aux = "";
			for (int i = 0; i < names.size(); i++)
				aux += names[i];

			layout->getRoot().getChild("Text").setText(aux);
		}
	}
}

void Credits::update(float deltaTime)
{
	if ((notNull(inputSystem) && inputSystem->getKeyPress("ESCAPE")) || checkControllersInput())
		backButtonClick();
}