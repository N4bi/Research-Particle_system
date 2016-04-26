#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

#include "Module.h"
#include "Timer.h"
#include "Point2d.h"
#include "Collision.h"
#include "Gui.h"

//Number to change the number of waves
#define TOTALWAVES 2
#define WAVETIME1 5.0f //120 = 2 minutes per wave in the future or some other game designish number
#define WAVETIME2 50.0f
#define	SIZE1X 1 // Changes number of zerglings per wave
#define SIZE1Y 1
#define	SIZE2X 10 // Changes number of zerglings per wave
#define SIZE2Y 1
#define	SIZE3X 5 // Changes number of zerglings per wave
#define SIZE3Y 2
#define TOTALUNITSALLWAVES (SIZE1X*SIZE1Y + SIZE2X*SIZE2Y + SIZE3X*SIZE2Y)
#define ZERGLINGSCORE 20
#define COMMANDCENTERPOSITION {1500, 2250}
#define SIZEMARINESX 4
#define SIZEMARINESY 2

class GuiImage;
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