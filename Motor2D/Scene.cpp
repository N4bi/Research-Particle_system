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
#include "Textures.h"

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

	if (app->input->getKey(SDL_SCANCODE_U) == KEY_DOWN)
	{
		iPoint m;
		app->input->getMousePosition(m);
		m = app->render->screenToWorld(m.x, m.y);

		Particle p;
		p.image = app->tex->loadTexture("Particles/Explosion/Small_Explosion.png");
		p.fx = app->audio->loadFx("Particles/explo1.wav");
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
		p.anim.loop = false;
		p.anim.current_frame = 0.0f;

		float emisorDuration = 0.05f;
		Uint32 particleLife = 4;
		int particleVelocity = 10;
		float min_angle = 220.0f;
		float max_angle = 310.0f;

		// TODO 6: Uncomment this and try diferent values on attributes 
		app->particle->addEmisor(p, m.x, m.y, emisorDuration, particleLife, particleVelocity, min_angle, max_angle, p.image);
	}

	if (app->input->getKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		iPoint m;
		app->input->getMousePosition(m);
		m = app->render->screenToWorld(m.x, m.y);

		Particle p;
		p.image = app->tex->loadTexture("Particles/Explosion/fire.png");
		p.anim.frames.push_back({ 0, 0, 78, 156 });
		p.anim.frames.push_back({ 78, 0, 78, 156 });
		p.anim.frames.push_back({ 156, 0, 78, 156 });
		p.anim.frames.push_back({ 234, 0, 78, 156 });
		p.anim.frames.push_back({ 312, 0, 78, 156 });
		p.anim.frames.push_back({ 390, 0, 78, 156 });
		p.anim.frames.push_back({ 468, 0, 78, 156 });
		p.anim.frames.push_back({ 546, 0, 78, 156 });
		p.anim.frames.push_back({ 624, 0, 78, 156 });
		p.anim.speed = 0.01f;
		p.anim.loop = true;
		p.anim.current_frame = 0.0f;


		float emisorDuration = 0.07f;
		Uint32 particleLife = 4;
		int particleVelocity = 40;
		float min_angle = 0.0f;
		float max_angle = 360.0f;

		// TODO 6: Uncomment this and try diferent values on attributes 
		Emisor* e = app->particle->addEmisor(p, m.x, m.y, emisorDuration, particleLife, 
			particleVelocity, min_angle, max_angle, p.image);
	
		e->speed.set(0, 0);
	}

	// TODO 7: Uncomment this and try to change values in particles.xml

	if (app->input->getKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		iPoint m;
		app->input->getMousePosition(m);
		m = app->render->screenToWorld(m.x, m.y);

		app->particle->addFire(m.x, m.y, 5.0f);

	}

	if (app->input->getKey(SDL_SCANCODE_Y) == KEY_DOWN)
	{
		iPoint m;
		app->input->getMousePosition(m);
		m = app->render->screenToWorld(m.x, m.y);

		app->particle->addBurst(m.x, m.y);
	}

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