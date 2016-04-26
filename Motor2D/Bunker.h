#ifndef __BUNKER_H__
#define __BUNKER_H__

#include "Building.h"

class Bunker : public Building
{
public:

	Animation					idle;
	map<uint, Entity*>			units_inside;
	uint						max_capacity;

	// SFX
	unsigned int bunker_attack_fx;

	Bunker(iPoint &p)
	{
		// Positions and dimensions
		center = { (float)p.x, (float)p.y };

		tex_width = 96;
		tex_height = 128;
		collider_offset.set(0, 36);
		pos = { (float)p.x - (tex_width / 2), (float)p.y - (tex_height / 2) };
		tile_pos = app->map->worldToMap(app->map->data.front(), center.x, center.y);

		// Animations and FX
		tex = app->tex->loadTexture("Building/Bunker.png"); //Sprites/Animations etc..
		bunker_attack_fx = app->audio->loadFx("Audio/FX/Marine/Marine_attack.wav");
		idle.frames.push_back({ 0, 0, 96, 128 });
		idle.speed = 1.0f;
		idle.loop = false;
		current_animation = &idle;

		// Colliders
		coll = app->collision->addCollider({ pos.x + collider_offset.x, pos.y + collider_offset.y, 96, 59 }, COLLIDER_BUILDING);

		// Another stuff
		specialization = BUNKER;
		max_hp = 350;
		current_hp = 350.0f;
		max_hp_bars = 10;
		offset_life = { -23, 35 };
		max_capacity = 4;

		range_of_vision = 150;
		range_to_attack = 150;
		damage = 0; //we change it according to the amount of marines inside;
		//attack_delay = 200.0f;

		state = IDLE;
		faction = PLAYER;
		selection_type = { 261, 1, 94, 56 };
		circle_selection_offset = { 0, 25 };
	}

	bool update(float dt)
	{
		switch (state)
		{
		case IDLE:
			if (specialization == BUNKER)
			{
				Bunker* bunker = (Bunker*)this;
				if (bunker->units_inside.size() > 0)
				{
					if (timer_to_check.read() >= TIME_TO_CHECK)
					{
						if (searchNearestEnemy())
							LOG("Enemy found");
						timer_to_check.start();
					}
				}
			}
			break;
		case MOVE:
			break;
		case MOVE_ALERT:
			break;
		case ATTACK:
			/*if (specialization == BUNKER)
			{
				Bunker* bunker = (Bunker*)this;
				if (bunker->units_inside.size() > 0)
				{
					app->audio->playFx(bunker_attack_fx);
					damage = bunker->units_inside.size() * 4;
					if ((timer_attack_delay.read() >= attack_delay)
					{
						attack();
						timer_attack_delay.start();

						if (state == ATTACK)
							searchNearestEnemy();
					}
				}
				else
					state = IDLE;
			}
			else
				state = IDLE;
			break;*/
		case DYING:
			//current_animation = &dead;
			if (timer_to_check.read() >= time_to_die)
			{
				leave_bunker();
				to_delete = true;
				coll->to_delete = true;
			}
			break;
		case REPAIR:
			break;
		}
		return true;
	}

	void leave_bunker() {
		if (!units_inside.empty())
		{
			map<uint, Entity*>::iterator it = units_inside.begin();
			for (; it != units_inside.end(); ++it)
			{
				it->second->inside_bunker = false;
				it->second->to_delete = false;
				app->entity_manager->active_entities.insert(pair<uint, Entity*>(it->first, it->second));
				//++capacity;
			}
			units_inside.clear();
		}
	}
};

#endif !__BUNKER_H__