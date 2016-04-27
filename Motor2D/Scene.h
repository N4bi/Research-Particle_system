#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Animation.h"
#include <vector>
using namespace std;

struct SDL_Texture;
class GuiImage;
class GuiLabel;
class Particle;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool awake(pugi::xml_node&);

	// Called before the first frame
	bool start();

	// Called before all updates
	bool preUpdate();

	// Called each loop iteration
	bool update(float dt);

	// Called before all updates
	bool postUpdate();

	// Called before quitting
	bool cleanUp();
	//GUI
	void onGui(GuiElements* ui, GUI_EVENTS event);

private:

	bool debug = false;
	Particle* particle = NULL;
};

#endif // __SCENE_H__
