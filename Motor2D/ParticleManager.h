#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#include "Module.h"
#include "Animation.h"
#include "Point2d.h"
#include <list>

#define PI 3.14159265

struct SDL_Texture;
class Timer;

struct Particle
{
	Animation anim;
	unsigned int fx; //This is for audio
	iPoint position;
	iPoint initialPosition;
	iPoint speed;
	Uint32 life;	// Time the particle life
	bool fxPlayed;
	bool follow;
	unsigned int lifes;
	Timer timer;
	SDL_Texture* image = NULL;
	bool alive;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool update(float dt);
	bool postUpdate();
	void followPoint(int x, int y);
	void destroyParticle();
};


struct Emisor
{
	iPoint position;
	iPoint speed;
	uint range;
	Uint32 duration;
	Timer timer;
	bool active;
	bool alive;
	uint fx;
	bool fxPlayed;
	Particle* particleEmited = NULL;


	Emisor();
	Emisor(Particle& p);
	~Emisor();
	bool update(float dt);
	bool postUpdate();
	void setParticle(Particle& particle);
	void enable();
	void disable();
	void destroy();
	void generateParticle();
};


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

	Particle* addParticle(const Particle& p, int x, int y, Uint32 secLife, const char* imageFile = NULL, const char* audioFile = NULL, uint32 delay = 0);

	Emisor* addEmisor(Particle& p, int x, int y, Uint32 emisorDuration, Uint32 particleLife, const char* imageFile = NULL);

private:
	SDL_Texture* texture;
	std::list<Particle*> particleList;
	std::string textureFile;

public:


};



#endif