#ifndef __COMMANDCENTER_H__
#define __COMMANDCENTER_H__

#include "Building.h"

class CommandCenter : public Building
{
public:

	Animation idle;

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

		state = IDLE;
		faction = PLAYER;
		selection_type = { 226, 66, 146, 88 };
		circle_selection_offset = { 0, 34 };
		specialization = COMMANDCENTER;
	}

};

#endif !__COMMANDCENTER_H__