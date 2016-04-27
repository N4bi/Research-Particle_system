#include "ParticleManager.h"
#include "App.h"
#include <math.h>
#include "Textures.h"
#include "Render.h"
#include "p2Log.h"
#include "Timer.h"
#include "Audio.h"

ParticleManager::ParticleManager() : Module()
{

}

ParticleManager::~ParticleManager()
{
	name.assign("particle_manager");
}

bool ParticleManager::awake(pugi::xml_node &node)
{
	LOG("Particle Manager: Awake");
	textureFile.assign(node.child("particle_manager").first_attribute().as_string());

	return true;
}

bool ParticleManager::start()
{
	LOG("Particle Manager: Start");
	texture = app->tex->loadTexture(textureFile.c_str());

	return true;
}

bool ParticleManager::update(float dt)
{
	bool ret = true;

	std::list<Particle*>::iterator tmp = particleList.begin();

	while (tmp != particleList.end())
	{
		if ((*tmp)->update(dt) == false)
		{
			RELEASE((*tmp));
			tmp = particleList.erase(tmp);
		}
		else
		{
			++tmp;
		}
	}

	return ret;
}

bool ParticleManager::postUpdate()
{
	bool ret = true;

	std::list<Particle*>::iterator tmp = particleList.begin();

	for (; tmp != particleList.end(); ++tmp)
	{
		(*tmp)->postUpdate();
	}

	return ret;
}

bool ParticleManager::cleanUp()
{
	LOG("Particle Manager: CleanUp");
	app->tex->unloadTexture(texture);

	std::list<Particle*>::iterator tmp = particleList.begin();

	for (; tmp != particleList.end(); ++tmp)
		RELEASE((*tmp));

	particleList.clear();

	return true;
}

bool ParticleManager::cleanActiveParticles()
{
	std::list<Particle*>::iterator tmp = particleList.begin();

	for (; tmp != particleList.end(); ++tmp)
		RELEASE((*tmp));

	particleList.clear();

	return true;
}

Particle* ParticleManager::addParticle(const Particle& p, int x, int y, Uint32 secLife, const char* imageFile, const char* audioFile, uint32 delay)
{
	Particle* part = NULL;

	part = new Particle(p);
	part->position.x = x - p.anim.peekCurrentFrame().w / 2;
	part->position.y = y - p.anim.peekCurrentFrame().h / 2;
	part->initialPosition = p.position;
	part->image = texture;
	part->alive = true;
	part->life = secLife;

	if (imageFile != NULL)
	{
		app->tex->unloadTexture(part->image);
		part->image = app->tex->loadTexture(imageFile);
	}

	if (audioFile != NULL)
	{
		part->fx = app->audio->loadFx(audioFile);
	}

	part->timer.start();

	particleList.push_back(part);

	return part;
}


// Particle


Particle::Particle() : fx(0), life(0), fxPlayed(false), follow(false), lifes(0), alive(false)
{
	position.setZero();
	speed.setZero();
}

Particle::Particle(const Particle& p)
{
	fx = p.fx;
	life = p.life;
	lifes = p.lifes;
	follow = p.follow;
	initialPosition = p.position;
	speed = p.speed;
	alive = p.alive;
	anim = p.anim;
}

Particle::~Particle()
{

}

void Particle::followPoint(int x, int y)
{
	position.x = x - anim.peekCurrentFrame().w / 2;
	position.y = y - anim.peekCurrentFrame().h / 2;
}

void Particle::destroyParticle()
{
	alive = false;
}

bool Particle::update(float dt)
{
	bool ret = true;

	if (life > 0)
	{
		if (timer.read() >= life * 1000 || alive == false)
		{
			ret = false;
		}
	}
	else
	{
		if (anim.finished() || alive == false)
			ret = false;
	}

	if (alive == true)
	{
		position.x += speed.x * dt;
		position.y += speed.y * dt;
	}

	return ret;
}
bool Particle::postUpdate()
{
	if (alive)
	{
		SDL_Rect sect = anim.getCurrentFrame();
		app->render->blit(image, position.x, position.y, &sect);

		if (fxPlayed == false)
		{
			fxPlayed = true;
			app->audio->playFx(fx);
		}
	}

	return true;
}
