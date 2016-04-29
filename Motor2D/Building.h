#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "ParticleManager.h"
#include "queue"

class Building : public Entity
{
public:

	uint				capacity;
	bool				storage;
	Particle			building_fire_low;
	Particle			building_fire_huge;
	Particle			building_explosion;
	

	//AleixBV Research
	queue<Entity*>		queue;
	Timer				creation_timer;

	

	Building() 
	{
		type = BUILDING;
		state = IDLE;



		// FIRE ANIMATION----
		building_fire_low.image = app->tex->loadTexture("Particles/Burn/Building_Burn_1.png");
		building_fire_huge.image = building_fire_low.image;
		building_explosion.image = app->tex->loadTexture("Particles/Explosion/Small_Explosion.png");

		building_fire_low.life = 5;
		building_fire_low.speed.set(0, 0);
		building_fire_low.anim.frames.push_back({ 0, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 64, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 128, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 192, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 256, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 320, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 384, 0, 64, 64 });
		building_fire_low.anim.frames.push_back({ 448, 0, 64, 64 });
		building_fire_low.anim.speed = 0.05f;
		building_fire_low.anim.loop = true;
		building_fire_low.anim.current_frame = 0.0f;

		building_fire_huge.life = 5;
		building_fire_huge.speed.set(0, 0);
		building_fire_huge.anim.frames.push_back({ 768, 0, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 832, 0, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 896, 0, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 960, 0, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 1024, 0, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 0, 95, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 64, 95, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 128, 95, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 192, 95, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 256, 95, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 320, 95, 64, 74 });
		building_fire_huge.anim.frames.push_back({ 384, 95, 64, 74 });
		building_fire_huge.anim.speed = 0.05f;
		building_fire_huge.anim.loop = true;
		building_fire_huge.anim.current_frame = 0.0f;

		building_explosion.life = 5;
		building_explosion.speed.set(0, 0);
		building_explosion.anim.frames.push_back({ 0, 0, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 128, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 256, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 384, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 512, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 640, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 768, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 896, 128, 128 });
		building_explosion.anim.frames.push_back({ 0, 1024, 128, 128 });
		building_explosion.anim.speed = 0.05f;
		building_explosion.anim.loop = false;
		building_explosion.anim.current_frame = 0.0f;

	};

	virtual bool update(float dt)
	{
		switch (state)
		{
		case IDLE:
			if (queue.size() > 0 && creation_timer.readSec() >= queue.front()->time_to_create)
			{
				Entity *e = queue.front();
				app->entity_manager->addEntity(e);
				queue.pop();
				creation_timer.start();
			}
			break;
		case MOVE:
			break;
		case MOVE_ALERT:
			break;
		case ATTACK:
			break;
		case DYING:
			//current_animation = &dead;
			if (timer_to_check.read() >= time_to_die)
			{
				to_delete = true;
				coll->to_delete = true;
			}
			break;
		case REPAIR:
			break;
		}
		return true;


	}

	virtual bool start()
	{

		return true;

	}




};


#endif __BUILDING_H__