#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#include "Module.h"
#include "Animation.h"
#include "Point2d.h"
#include <list>
#include "FileSystem.h"

#define PI 3.14159265

struct SDL_Texture;
class Timer;
class Particle;
class Emisor;
class FireEmisor;
class BurstEmisor;

class ParticleManager : public Module
{
public:
	ParticleManager();
	~ParticleManager();

	bool awake(pugi::xml_node &node);

	bool start();

	bool update(float dt);

	bool postUpdate();

	bool cleanUp();

	bool cleanActiveParticles();

	Particle* addParticle(const Particle& p, int x, int y, Uint32 secLife = INT_MAX, SDL_Texture* texture = NULL,
		unsigned int sfx = 0, uint32 delay = 0);

	Emisor* addEmisor(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity, float minAngle = 0.0f, float maxAngle = 360.0f,
		SDL_Texture* tex = NULL);

	FireEmisor* addFire(int x, int y, float duration);

	BurstEmisor* addBurst(int x, int y);

private:
	SDL_Texture* texture;
	std::list<Particle*> particleList;
	std::list<Emisor*> emisorList;
	std::string textureFile;

	bool loadParticlesFile(pugi::xml_document& file);

public:
	pugi::xml_document particle_file;

};

struct Particle
{
	Animation			anim;
	unsigned int		fx; //This is for audio
	fPoint				position;
	fPoint				initialPosition;
	fPoint				speed;
	Uint32				life;	// Time the particle life
	bool				fxPlayed;
	Timer				timer;
	SDL_Texture*		image = NULL;
	bool				alive;
	bool				active = true;
	bool			    on = false; 

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool update(float dt);
	bool postUpdate();
	void followPoint(int x, int y);
	void destroyParticle();
	void enable();
	void disable();

	void setSpeed(float velocity,float minAngle = 0.0f, float maxAngle = 360.0f);
};


class Emisor
{
public:
	fPoint		position;
	fPoint		speed;
	float		duration;
	Timer		timer;
	bool		active;
	bool		alive;
	uint	    fx;
	bool		fxPlayed;
	Particle    particleEmited;
	float	    velocity;
	float		minAngle;
	float		maxAngle;

public:
	Emisor();
	Emisor(Particle& p);
	virtual ~Emisor();
	virtual bool update(float dt);
	virtual bool postUpdate();
	void setParticle(Particle& particle);
	void enable();
	void disable();
	void destroy();
};

class FireEmisor : public Emisor
{
public:
	Particle	fire;
	Particle	smoke;
	

	float		smokeFrequence;
	float		smokeStart;
	bool	    fireStarted = false;
	iPoint		smokeOffset;


public:
	FireEmisor(float time);
	~FireEmisor();

	bool update(float dt);
	bool postUpdate();

private:
	float	   acumulator = 0.0f;
};

class BurstEmisor : public Emisor
{
public:
	Particle	burst;
	Emisor		emisor_burst;
	bool	    burstStarted = false;

public:
	BurstEmisor();
	~BurstEmisor();

	bool update(float dt);
	bool postUpdate();

};


#endif