#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#include "Module.h"
#include "Animation.h"
#include "Point2d.h"
#include <list>

#define PI 3.14159265

struct SDL_Texture;
class Timer;
class Particle;
class Emisor;

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

	Particle* addParticle(const Particle& p, int x, int y, Uint32 secLife = INT_MAX, const char* imageFile = NULL,
		const char* audioFile = NULL);

	Emisor* addEmisor(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity, float minAngle = 0.0f, float maxAngle = 360.0f,
		const char* imageFile = NULL);

	Particle* addParticle2(const Particle& p, int x, int y, Uint32 secLife = INT_MAX, SDL_Texture* texture = NULL,
		unsigned int sfx = 0, uint32 delay = 0);

	Emisor* addEmisor2(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity, float minAngle = 0.0f, float maxAngle = 360.0f,
		SDL_Texture* tex = NULL);

private:
	SDL_Texture* texture;
	std::list<Particle*> particleList;
	std::list<Emisor*> emisorList;
	std::string textureFile;

	void setSpeed(float velocity, fPoint& speed, float minAngle = 0.0f, float maxAngle = 360.0f);

public:


};

struct Particle
{
	Animation anim;
	unsigned int fx; //This is for audio
	fPoint position;
	fPoint initialPosition;
	fPoint speed;
	Uint32 life;	// Time the particle life
	bool fxPlayed;
	Timer timer;
	SDL_Texture* image = NULL;
	SDL_Rect	 quad;
	bool alive;
	bool active = true;
	bool on = false;

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
	fPoint position;
	fPoint speed;
	float duration;
	Timer timer;
	bool active;
	bool alive;
	uint fx;
	bool fxPlayed;
	float frequance; //Particle ammount per second
	Particle particleEmited;
	float velocity;
	float minAngle;
	float maxAngle;

public:
	Emisor();
	Emisor(Particle& p);
	~Emisor();
	bool update(float dt);
	bool postUpdate();
	void setParticle(Particle& particle);
	void enable();
	void disable();
	void destroy();
};





#endif