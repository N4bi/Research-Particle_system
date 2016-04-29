#include "App.h"
#include "GameManager.h"
#include "EntityManager.h"
#include "Input.h"
#include "p2Log.h"
#include <time.h>
#include "SDL\include\SDL_mouse.h"
#include "Render.h"
#include "Gui.h"
#include "Textures.h"
#include "Window.h"
#include "Audio.h"
#include "map.h"
#include <map>
#include "Entity.h"

#include "GuiImage.h"
#include "GuiLabel.h"

using namespace std;

bool GameManager::start()
{
	bool ret = true;

	LOG("LAST HOPE GAME STARTS!");
	app->render->start_transition({ 900, 1974 });
	startGame();
	
	return ret;
}

bool GameManager::update(float dt)
{
	bool ret = true;


	return ret;
}

bool GameManager::postUpdate()
{
	return true;
}


bool GameManager::cleanUp()
{
	bool ret = false;

	return ret;
}

void GameManager::startGame()
{
	

		iPoint p = COMMANDCENTERPOSITION;
		app->entity_manager->addEntity(p, COMMANDCENTER);  //BASE CREATION

		unsigned int size_marines_x = SIZEMARINESX;
		unsigned int size_marines_y = SIZEMARINESY;

}


//unsigned int is intended ask me WHY I do it instead of uint.
void GameManager::createMarines(iPoint position,unsigned int sizex, unsigned int sizey)
{
	

	for (int i = 0; i < sizex; i++)
	{
		for (int j = 0; j < sizey; j++)
		{
			int posx = position.x + (sizex * i * 10);
			int posy = position.y + (sizey * j * 10);

			app->entity_manager->addEntity(iPoint({ posx, posy }), MARINE);
		}
	}

}


bool GameManager::isGameStarted() const
{
	return start_game;
}


