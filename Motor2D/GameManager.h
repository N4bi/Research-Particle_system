#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "Module.h"
#include "Timer.h"
#include "Point2d.h"
#include "Collision.h"
#include "Gui.h"

//Number to change the number of waves
#define COMMANDCENTERPOSITION {1500, 2250}
#define SIZEMARINESX 4
#define SIZEMARINESY 2

using namespace std;

//Class that manages the game. The win lose cycle, the points earned, the resources, waves etc..

/*
Place a timer, entity manager communicate zerg list/map etc..
*/
class GameManager : public Module
{
public:


	iPoint initial_position;

public:

	GameManager()
	{
		name.assign("GameManager");
	}

	void startGame();

	bool start();

	bool update(float dt);
	
	bool postUpdate();

	bool cleanUp();

	void createMarines(iPoint position,unsigned int sizex, unsigned int sizey);

public:

	bool isGameStarted() const;

private:

	bool start_game = false;
	
	unsigned int size_marines_x = SIZEMARINESX;
	unsigned int size_marines_y = SIZEMARINESY;
};

#endif