#include "ParticleManager.h"
#include "App.h"
#include <math.h>
#include <time.h>
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
	std::srand(time(NULL));

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

	std::list<Emisor*>::iterator tmp2 = emisorList.begin();

	while (tmp2 != emisorList.end())
	{
		if ((*tmp2)->update(dt) == false)
		{
			(*tmp2)->destroy();
			RELEASE((*tmp2));
			tmp2 = emisorList.erase(tmp2);
		}
		else
		{
			++tmp2;
		}
	}

	return ret;
}

bool ParticleManager::postUpdate()
{
	bool ret = true;

	std::list<Particle*>::reverse_iterator tmp = particleList.rbegin();

	for (; tmp != particleList.rend(); ++tmp)
	{
		(*tmp)->postUpdate();
	}

	std::list<Emisor*>::iterator tmp2 = emisorList.begin();

	for (; tmp2 != emisorList.end(); ++tmp2)
	{
		(*tmp2)->postUpdate();
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

	std::list<Emisor*>::iterator tmp2 = emisorList.begin();

	for (; tmp2 != emisorList.end(); ++tmp2)
		RELEASE((*tmp2));

	emisorList.clear();

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

void ParticleManager::setSpeed(float velocity, fPoint& speed, float minAngle, float maxAngle)
{
	float angle = rand() % 360;
	speed.x = velocity * cos(angle * (PI / 180));
	speed.y = velocity * sin(angle * (PI / 180));
	LOG("Angle: %f", angle);
	LOG("Speed x: %f. Speed y: %f", speed.x, speed.y);
}

Particle* ParticleManager::addParticle(const Particle& p, int x, int y, Uint32 secLife, const char* imageFile, const char* audioFile)
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
	else if (p.image != NULL)
	{
		part->image = p.image;
	}
	else
	{
		part->quad.w = part->quad.h = 20;
		part->quad.x = x;
		part->quad.y = y;
	}

	if (audioFile != NULL)
	{
		part->fx = app->audio->loadFx(audioFile);
	}

	part->timer.start();

	particleList.push_back(part);

	return part;
}

Emisor* ParticleManager::addEmisor(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity,
	float minAngle, float maxAngle, const char* imageFile)  // If all particles are load at creation point
{
	Emisor* ret = NULL;

	ret = new Emisor(p);
	ret->position.set(x, y);
	ret->duration = emisorDuration;
	ret->particleEmited.life = particleLife;
	ret->velocity = particleVelocity;
	ret->minAngle = minAngle;
	ret->maxAngle = maxAngle;

	if (imageFile != NULL)
	{
		app->tex->unloadTexture(ret->particleEmited.image);
		ret->particleEmited.image = app->tex->loadTexture(imageFile);
	}


	ret->timer.start();
	ret->active = ret->alive = true;

	emisorList.push_back(ret);

	return ret;
}

// Particle


Particle::Particle() : fx(0), life(0), fxPlayed(false), alive(false)
{
	position.setZero();
	speed.setZero();
}

Particle::Particle(const Particle& p)
{
	fx = p.fx;
	life = p.life;
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

	if ( alive == true && active == true)
	{
		position.x += speed.x * dt / 1000;
		position.y += speed.y * dt / 1000;
	}

	return ret;
}
bool Particle::postUpdate()
{
	if (alive && active)
	{
		if (image != NULL)
		{
			SDL_Rect sect = anim.getCurrentFrame();
			app->render->blit(image, position.x, position.y, &sect);
		}
		else
		{
			app->render->DrawQuad(quad, 255, 0, 0);
		}

		if (fxPlayed == false)
		{
			fxPlayed = true;
			app->audio->playFx(fx);
		}
	}


	return true;
}

void Particle::enable()
{
	active = true;
	timer.start();
}

void Particle::disable()
{
	active = false;
}

void Particle::setSpeed(float velocity, float minAngle, float maxAngle)
{
	float angle = minAngle + (rand() % (int)(maxAngle - minAngle));
	speed.x = velocity * cos(angle * (PI / 180));
	speed.y = velocity * sin(angle * (PI / 180));
	//LOG("Angle: %f", angle);
	//LOG("Speed x: %f. Speed y: %f", speed.x, speed.y);
}

// Emisor

Emisor::Emisor()
{
	position.setZero();
	speed.setZero();
	active = alive = fxPlayed = false;
}

Emisor::Emisor(Particle& p)
{
	particleEmited = p;
	position.setZero();
	speed.setZero();
	fxPlayed = false;
}

Emisor::~Emisor()
{

}

bool Emisor::update(float dt) // If particles are created each frame
{
	bool ret = true;

	if (timer.read() >= duration * 1000 || alive == false)
	{
		ret = false;
	}

	if (alive && active)
	{
		Particle* q = app->particle->addParticle(particleEmited, position.x, position.y, particleEmited.life);
		q->setSpeed(velocity, minAngle, maxAngle);

		position.x += speed.x * dt / 1000;
		position.y += speed.y * dt / 1000;
	}

	return ret;
}

bool Emisor::postUpdate()
{
	if (alive)
	{
		if (fxPlayed == false)
		{
			fxPlayed = true;
			app->audio->playFx(fx);
		}
	}

	return true;
}

void Emisor::setParticle(Particle& particle)
{
	particleEmited = particle;
}

void Emisor::enable()
{
	active = true;
	timer.start();
}

void Emisor::disable()
{
	active = false;
}

void Emisor::destroy()
{
	alive = false;
}

