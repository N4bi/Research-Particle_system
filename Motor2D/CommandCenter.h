#ifndef __COMMANDCENTER_H__
#define __COMMANDCENTER_H__
#include "ParticleManager.h"
#include "Building.h"

class CommandCenter : public Building
{
public:

	Animation	idle;

	Particle*   fire_particle_low;
	Particle*	fire_particle_huge;
	Particle*   explosion;


public:

	CommandCenter(iPoint &p)
	{
		// Positions and dimensions
		center = { (float)p.x, (float)p.y };

		tex_width = 128;
		tex_height = 100;
		collider_offset.setZero();
		pos = { (float)p.x - (tex_width / 2), (float)p.y - (tex_height / 2) };
		tile_pos = app->map->worldToMap(app->map->data.back(), center.x, center.y);

		// Animations
		tex = app->tex->loadTexture("Building/CommandCenter.png"); //Sprites/Animations etc..
		idle.frames.push_back({ 0, 27, 128, 100 });
		idle.speed = 1.0f;
		idle.loop = false;
		current_animation = &idle;



		// Colliders
		coll = app->collision->addCollider({ pos.x + collider_offset.x, pos.y + collider_offset.y, 128, 100 }, COLLIDER_BUILDING);

		// Another stuff
		specialization = COMMANDCENTER;
		max_hp = 1500;
		current_hp = 1500;
		max_hp_bars = 36;
		offset_life = { -80, 57 };
		particles_offset = { 50, 20 };
		time_to_die = 2000.0f;
		

		state = IDLE;
		faction = PLAYER;
		selection_type = { 226, 66, 146, 88 };
		circle_selection_offset = { 0, 34 };
		specialization = COMMANDCENTER;
	}

	bool update(float dt)
	{
		
		switch (state)
		{
		case IDLE:
			if (current_hp < max_hp)
			{
				if (current_hp <= max_hp / 2 && building_fire_low.on == false)
				{
					building_fire_low.on = true;
					if (building_fire_low.on)
					{
						fire_particle_low = app->particle->addParticle(building_fire_low, pos.x + particles_offset.x, pos.y + particles_offset.y,INT_MAX,"Particles/Burn/Building_Burn_1.png");
					}

				}

				if (current_hp <= max_hp / 3 && building_fire_huge.on == false)
				{
					building_fire_huge.on = true;
					if (building_fire_huge.on)
					{
						fire_particle_huge = app->particle->addParticle(building_fire_huge, center.x + particles_offset.x - 15, center.y + particles_offset.y - 15, INT_MAX, "Particles/Burn/Building_Burn_1.png");
					}

				}
			}
			else
			{
				if (building_fire_low.on == true)
				{
					building_fire_low.on = false;
					fire_particle_low->destroyParticle();

					if (building_fire_huge.on == true)
					{
						building_fire_huge.on = false;
						fire_particle_huge->destroyParticle();
					}
				
				}
			}


			break;
		case MOVE:
			break;
		case MOVE_ALERT:
			break;
		case ATTACK:
		case DYING:
			//current_animation = &dead;
			building_fire_low.on = false;
			building_fire_huge.on = false;
			// In debug mode lags the game a bit, if you try this in Release mode, the game runs perfect.
			explosion = app->particle->addParticle(building_explosion, center.x, center.y, 1, "Particles/Burn/Building_Burn_1.png");
			
			if (timer_to_check.read() >= time_to_die)
			{
					fire_particle_low->destroyParticle();
					fire_particle_huge->destroyParticle();
					to_delete = true;
					coll->to_delete = true;
			}
			break;
		case REPAIR:
			break;
		}
		return true;
	}

};

#endif !__COMMANDCENTER_H__