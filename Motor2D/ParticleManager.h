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
	fPoint position;
	fPoint initialPosition;
	fPoint speed;
	Uint32 life;	// Time the particle life
	bool fxPlayed;
	bool follow;
	unsigned int lifes;
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


struct Emisor
{
	fPoint position;
	fPoint speed;
	uint range;
	float duration;
	Timer timer;
	bool active;
	bool alive;
	uint fx;
	bool fxPlayed;
	float frequance; //Particle ammount per second
	uint particleQuantity;
	Particle particleEmited;
	std::vector<Particle*> particles;
	uint particlesPerFrame = 0;
	uint particlesOut = 0;
	float velocity;
	float minAngle;
	float maxAngle;


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
		const char* audioFile = NULL, uint32 delay = 0);

	Emisor* addEmisor(Particle& p, int x, int y, float emisorDuration, Uint32 particleLife, int particleVelocity, float minAngle = 0.0f, float maxAngle = 360.0f,
		const char* imageFile = NULL, const char* audioFile = NULL);

private:
	SDL_Texture* texture;
	std::list<Particle*> particleList;
	std::list<Emisor*> emisorList;
	std::string textureFile;

	void setSpeed(float velocity, fPoint& speed, float minAngle = 0.0f, float maxAngle = 360.0f);

public:


};



#endif