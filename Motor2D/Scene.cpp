#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "Map.h"
#include "PathFinding.h"
#include "Scene.h"
#include "ParticleManager.h"

#include "GuiImage.h"
#include "GuiLabel.h"

Scene::Scene() : Module()
{
	name.assign("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::awake(pugi::xml_node &node)
{
	LOG("Loading Scene");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Scene::start()
{	
   	int w, h;
	uchar *buffer = NULL;
	if(app->map->createWalkabilityMap(w, h, &buffer))
		app->path->setMap(w, h, buffer);
	
	return true;
}

// Called each loop iteration
bool Scene::preUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::update(float dt)
{
	float cam_speed = 1.0f;

	if (app->input->getKey(SDL_SCANCODE_KP_PLUS) == KEY_UP)
		app->audio->volumeUp();

	if (app->input->getKey(SDL_SCANCODE_KP_MINUS) == KEY_UP)
		app->audio->volumeDown();

	// Paint Layers -- Draw or undraw the map
	if (app->input->getKey(SDL_SCANCODE_P) == KEY_REPEAT)
		app->map->setLayerProperty("PROTOTYPE_LOGIC_MAP.tmx", "Logic_Layer", "NoDraw", 0);

	// Paint Layers -- Draw or undraw the map
	if (app->input->getKey(SDL_SCANCODE_O) == KEY_REPEAT)
		app->map->setLayerProperty("PROTOTYPE_LOGIC_MAP.tmx", "Logic_Layer", "NoDraw", 1);

	if (app->input->getKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		iPoint m;
		iPoint m2;
		app->input->getMousePosition(m);
		m2 = app->render->screenToWorld(m.x, m.y);

		Particle p;
		p.life = 5;
		p.speed.set(0, 0);
		p.anim.frames.push_back({ 0, 0, 128, 128 });
		p.anim.frames.push_back({ 0, 128, 128, 128 });
		p.anim.frames.push_back({ 0, 256, 128, 128 });
		p.anim.frames.push_back({ 0, 384, 128, 128 });
		p.anim.frames.push_back({ 0, 512, 128, 128 });
		p.anim.frames.push_back({ 0, 640, 128, 128 });
		p.anim.frames.push_back({ 0, 768, 128, 128 });
		p.anim.frames.push_back({ 0, 896, 128, 128 });
		p.anim.frames.push_back({ 0, 1024, 128, 128 });
		p.anim.speed = 0.05f;
		p.anim.loop = true;
		p.anim.current_frame = 0.0f;

		particle = app->particle->addParticle(p, m2.x, m2.y, 15, "Particles/Explosion/Small_Explosion.png");
	}

	/*if (particle)
	{
		iPoint m;
		iPoint m2;
		app->input->getMousePosition(m);
		m2 = app->render->screenToWorld(m.x, m.y);

		particle->followPoint(m2.x, m2.y);
	}*/

	return true;
}

// Called each loop iteration
bool Scene::postUpdate()
{
	bool ret = true;

	if(app->input->getKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::cleanUp()
{
	LOG("Freeing scene");
	return true;
}

//GUI
void Scene::onGui(GuiElements* ui, GUI_EVENTS event)
{
	
}