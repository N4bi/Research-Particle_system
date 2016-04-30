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
	bool ret = true;
	LOG("Particle Manager: Start");
	texture = app->tex->loadTexture(textureFile.c_str());

	ret = loadParticlesFile();
	particles_node;
	return ret;
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

bool ParticleManager::loadParticlesFile()
{
	bool ret = true;

	pugi::xml_document	particles_file;
	char* buff;
	int size = app->fs->load("Particles/particles.xml", &buff);
	pugi::xml_parse_result result = particles_file.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load particles xml file. Pugi error: %s", result.description());
		ret = false;
	}
	else
		particles_node = particles_file.child("particles");

	return ret;
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
		app->audio->playFx(part->fx);
	}

	part->timer.start();

	particleList.push_back(part);

	return part;
}

Particle* ParticleManager::addParticle2(const Particle& p, int x, int y, Uint32 secLife, SDL_Texture* texture, unsigned int sfx, uint32 delay)
{
	Particle* part = NULL;

	part = new Particle(p);
	part->position.x = x - p.anim.peekCurrentFrame().w / 2;
	part->position.y = y - p.anim.peekCurrentFrame().h / 2;
	part->initialPosition = p.position;
	part->alive = true;
	part->life = secLife;
	if (texture)
		part->image = texture;
	else
		part->image = p.image;

	part->fx = sfx;
	part->timer.start();

	particleList.push_back(part);

	return part;
}

Emisor* ParticleManager::addEmisor(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity,
	float minAngle, float maxAngle, const char* imageFile,const char* audioFile) 
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

	if (audioFile != NULL)
	{
		ret->fx = app->audio->loadFx(audioFile);
		if (ret->fxPlayed == false)
		{
			ret->fxPlayed = true;
			app->audio->playFx(ret->fx);
		}
		
	}



	ret->timer.start();
	ret->active = ret->alive = true;

	emisorList.push_back(ret);

	return ret;
}

Emisor* ParticleManager::addEmisor2(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity,
	float minAngle, float maxAngle, SDL_Texture* tex)  // If all particles are load at creation point
{
	Emisor* ret = NULL;

	ret = new Emisor(p);
	ret->position.set(x, y);
	ret->duration = emisorDuration;
	ret->particleEmited.life = particleLife;
	ret->velocity = particleVelocity;
	ret->minAngle = minAngle;
	ret->maxAngle = maxAngle;
	ret->particleEmited.image = tex;

	ret->timer.start();
	ret->active = ret->alive = true;

	emisorList.push_back(ret);

	return ret;
}

FireEmisor* ParticleManager::addFire(int x, int y, float duration)
{
	FireEmisor* ret = NULL;

	ret = new FireEmisor(duration);
	ret->position.set(x, y);

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
	image = p.image;
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
		/*Particle* q = app->particle->addParticle2(particleEmited, position.x, position.y, particleEmited.life, particleEmited.image);*/

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

// FireEmisor
FireEmisor::FireEmisor(float time) : Emisor()
{
	duration = time;

	/*pugi::xml_node fire1 = app->particle->particles_node.child("fire1");

	int fireX = fire1.child("fire_anim").attribute("x").as_int();
	int fireY = fire1.child("fire_anim").attribute("y").as_int();
	int fireW = fire1.child("fire_anim").attribute("w").as_int();
	int fireH = fire1.child("fire_anim").attribute("h").as_int();
	int fireFPR = fire1.child("fire_anim").attribute("frames_per_row").as_int();
	int fireFPC = fire1.child("fire_anim").attribute("frames_per_column").as_int();*/

	fire.anim.setAnimations(0, 0, 64, 64, 8, 4, 8*4);
	fire.anim.speed = 0.01f;
	fire.anim.loop = true;
	fire.anim.current_frame = 0.0f;
	fire.speed.set(0, 0);
	fire.image = app->tex->loadTexture("Particles/Flame/torusflame1.png");
	fire.life = duration;

	/*int smokeX = fire1.child("smoke_anim").attribute("x").as_int();
	int smokeY = fire1.child("smoke_anim").attribute("y").as_int();
	int smokeW = fire1.child("smoke_anim").attribute("w").as_int();
	int smokeH = fire1.child("smoke_anim").attribute("h").as_int();
	int smokeFPR = fire1.child("smoke_anim").attribute("frames_per_row").as_int();
	int smokeFPC = fire1.child("smoke_anim").attribute("frames_per_column").as_int();*/

	smoke.anim.setAnimations(0, 0, 64, 64, 8, 4, 8 * 4);
	smoke.anim.speed = 0.02f;
	smoke.anim.loop = true;
	smoke.anim.current_frame = 0.0f;
	smoke.speed.set(-100, -100);
	smokeFrequence = 1.0f;
	smokeStart = 0.0f;
	smokeOffset.set(0, -1);
	smoke.image = app->tex->loadTexture("Particles/Flame/smokeball.png");
	smoke.life = 2.0f;
}

FireEmisor::~FireEmisor()
{

}

bool FireEmisor::update(float dt)
{
	bool ret = true;

	if (timer.read() >= duration * 1000 || alive == false)
	{
		ret = false;
	}
	if (alive && active)
	{
		if (!fireStarted)
		{
			fireStarted = true;
			app->particle->addParticle2(fire, position.x, position.y, fire.life);
		}
		if (timer.read() >= smokeStart * 1000)
		{
			if (acumulator >= smokeFrequence * 1000)
			{
				app->particle->addParticle2(smoke, position.x + smokeOffset.x, position.y + smokeOffset.y, smoke.life);
				acumulator = 0.0f;
			}
			acumulator += dt;
		}

		position.x += speed.x * dt / 1000;
		position.y += speed.y * dt / 1000;
	}


	return ret;
}

bool FireEmisor::postUpdate()
{
	bool ret = true;

	if (alive && active)
	{
		if (!fxPlayed)
		{
			fxPlayed = true;
			app->audio->playFx(fx);
		}
	}

	return ret;
}