#include "App.h"
#include "EntityManager.h"
#include "Input.h"
#include "Window.h"
#include "p2Log.h"
#include "PathFinding.h"
#include "Marine.h"
#include "Zergling.h"
#include "Scv.h"
#include "CommandCenter.h"
#include "Bunker.h"
#include "GameManager.h"


EntityManager::EntityManager() : Module()
{
	name.assign("EntityManager");
}

// Destructor
EntityManager::~EntityManager()
{ }

// Called before render is available
bool EntityManager::awake(pugi::xml_node &node)
{
	return true;
}

// Called before the first frame
bool EntityManager::start()
{
	next_ID = 0;
    building_tile = app->tex->loadTexture("maps/Path_Tiles.png");

	return true;
}

Entity* const EntityManager::addEntity(iPoint &pos, SPECIALIZATION type)
{
	Entity *e = NULL;

	switch (type)
	{
	case(MARINE) :
		LOG("Creating Marine");
		e = new Marine(pos);
		break;
	case(COMMANDCENTER) :
		LOG("Creating Command Center");
		e = new CommandCenter(pos);
		break;
	case(BUNKER) :
		LOG("Creating Bunker");
		e = new Bunker(pos);
		building_to_place = (Building*)e;
		building_mode = true;
		create_bunker = false;
		break;
	case(ZERGLING) :
		LOG("Creating Zergling");
		e = new Zergling(pos);
		break;
	case(SCV) :
		LOG("Creating SCV");
		e = new Scv(pos);
		break;
	}

	if (e != NULL && building_mode != true)
	{
		e->id = ++next_ID;
		active_entities.insert(pair<uint, Entity*>(e->id, e));

		// Command center creation, special treatment
		if (e->specialization == COMMANDCENTER)
		{
			app->map->changeLogic(e->coll->rect, NO_WALKABLE);
			recalculatePaths();
		}
	}

	return e;
}

//AleixBV Research
Entity* const EntityManager::addEntity(Entity* e)
{
	if (e != NULL && building_mode != true)
	{
		e->id = ++next_ID;
		active_entities.insert(pair<uint, Entity*>(e->id, e));

		// Command center creation, special treatment
		if (e->specialization == COMMANDCENTER)
		{
			app->map->changeLogic(e->coll->rect, NO_WALKABLE);
			recalculatePaths();
		}
	}

	return e;
}

Entity* const EntityManager::createUnit(iPoint &pos, SPECIALIZATION type)
{
	Entity *e = NULL;

	switch (type)
	{
	case(MARINE) :
		LOG("Creating Marine");
		e = new Marine(pos);
		break;
	case(ZERGLING) :
		LOG("Creating Zergling");
		e = new Zergling(pos);
		break;
	case(SCV) :
		LOG("Creating SCV");
		e = new Scv(pos);
		create_SCV = false;
		break;
	}

	return e;
}
//AleixBV /Research

/*Method that makes the enemyWave attack the commandCenter*/
void EntityManager::SetEnemyToAttackCommandCenter(Entity* e)
{
	if (e->type == UNIT)
	{
		iPoint p = COMMANDCENTERPOSITION;
		p = app->map->worldToMap(app->map->data.back(), p.x, p.y - 100); // With -100, we avoid a NoWalkable tile

		Unit* unit = (Unit*)e;
		if (app->path->createPath(e->tile_pos, p) != -1)
		{
			unit->path = app->path->getLastPath();
			unit->has_target = true;
			unit->state = MOVE_ALERT;
		}
	}
}

// Called each loop iteration
bool EntityManager::preUpdate()
{

	// We delete the entities marked with to_delete
	map<uint, Entity*>::iterator it = active_entities.begin();
	for (; it != active_entities.end();)
	{
		if (it->second->to_delete)
		{
			// Maybe, the entity removed is someone's entity_to_attack. Now, it's not.  CRZ
			for (map<uint, Entity*>::iterator it2 = active_entities.begin(); it2 != active_entities.end(); ++it2)
			{
				if (it->second == it2->second->target_to_attack)
					it2->second->target_to_attack = NULL;
			}


			if (it->second->specialization == BUNKER)
			{
				app->map->changeLogic(it->second->coll->rect, LOW_GROUND);
				app->entity_manager->recalculatePaths();
			}

			if (it->second->specialization == COMMANDCENTER)
			{
				app->map->changeLogic(it->second->coll->rect, LOW_GROUND);
				app->entity_manager->recalculatePaths();
			}
			
			selection.erase(it->first);
			if(!it->second->inside_bunker)
				RELEASE(it->second);

			it = active_entities.erase(it);
		}
		else
			++it;
	}
	
	handleSelection();

	iPoint position;

	if (app->input->getKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		app->input->getMousePosition(position);
		position = app->render->screenToWorld(position.x, position.y);
		addEntity(position, MARINE);		
	}

	if (app->input->getKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		app->input->getMousePosition(position);
		position = app->render->screenToWorld(position.x, position.y);
		addEntity(position, SCV);
	}


	if (app->input->getKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		app->input->getMousePosition(position);
		position = app->render->screenToWorld(position.x, position.y);
		addEntity(position, COMMANDCENTER);
	}

	if (app->input->getKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		app->input->getMousePosition(position);
		position = app->render->screenToWorld(position.x, position.y);
		addEntity(position, ZERGLING);
	}

	return true;
}


// Called each loop iteration
bool EntityManager::update(float dt)
{
	// Debug ---
	if (app->input->getKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	map<uint, Entity*>::iterator it = active_entities.begin();
	for (; it != active_entities.end(); ++it)
			it->second->update(dt);

	return true;
}

// Called each loop iteration
bool EntityManager::postUpdate()
{	
	/*Resetting bool for Game Manager*/
	enemyJustDied = false;

	// Entities Drawing
	// Look on Gui postUpdate()!

	// Drawing selector (green SDL_Rect)
	if (selector_init && selector.w > 1 && selector.h > 1) app->render->DrawQuad(selector, 35, 114, 48, 255, false, true);

	if (building_mode)
		choosePlaceForBuilding();

	return true;
}

// Called before quitting
bool EntityManager::cleanUp()
{
	map<uint, Entity*>::iterator it = active_entities.begin();
	for (; it != active_entities.end(); it++)
		delete it->second;

	active_entities.clear();
	selection.clear();

	return true;
}

// Return ID for the corresponding entity
Entity *EntityManager::getEntity(uint id)
{
	map<uint, Entity*>::iterator it = active_entities.find(id);
	return (it != active_entities.end() ? it->second : NULL);
}

// WhichEntityOnMouse: Returns an entity under the mouse cursor
Entity *EntityManager::whichEntityOnMouse()
{
	iPoint p; app->input->getMousePosition(p);
	p = app->render->screenToWorld(p.x, p.y);

	map<uint, Entity*>::reverse_iterator rit = active_entities.rbegin();
	for (; rit != active_entities.rend(); ++rit)
	{
		if (p.x >= rit->second->coll->rect.x &&
			p.x <= rit->second->coll->rect.x + rit->second->coll->rect.w &&
			p.y >= rit->second->coll->rect.y &&
			p.y <= rit->second->coll->rect.y + rit->second->coll->rect.h)
			return rit->second;
	}
	return NULL;
}

// CalculateSelector: Method that computes the dimensions of the green SDL_Rect selector.
void EntityManager::calculateSelector()
{
	// If dimensions of the selector are less than 1/1 (for width/height), it sets 1. Eliminates 0 width/height to proper alone selection. 
	int selector_width = abs(final_selector_pos.x - initial_selector_pos.x) > 1 ? abs(final_selector_pos.x - initial_selector_pos.x) : 1;
	int selector_height = abs(final_selector_pos.y - initial_selector_pos.y) > 1 ? abs(final_selector_pos.y - initial_selector_pos.y) : 1;
	int selector_pos_x = (initial_selector_pos.x < final_selector_pos.x ? initial_selector_pos.x : final_selector_pos.x);
	int selector_pos_y = (initial_selector_pos.y < final_selector_pos.y ? initial_selector_pos.y : final_selector_pos.y);
	selector = { selector_pos_x, selector_pos_y, selector_width, selector_height };
}

void EntityManager::handleSelection()
{
	// Clicking and holding left button, starts a selection
	if (!building_mode && app->input->getMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		selection.clear();
		app->input->getMousePosition(initial_selector_pos);
		initial_selector_pos = app->render->screenToWorld(initial_selector_pos.x, initial_selector_pos.y);

		//Click and select unit	
		selector_init = true;
		Entity *e = whichEntityOnMouse();

		if (e != NULL)
			selector = { e->coll->rect.x, e->coll->rect.y, 1, 1 };
		else
			selector = { 0, 0, 0, 0 };
	}

	// Holding left button, updates selector dimensions with calculateSelector
	if (selector_init && app->input->getMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		app->input->getMousePosition(final_selector_pos);
		final_selector_pos = app->render->screenToWorld(final_selector_pos.x, final_selector_pos.y);

		calculateSelector();
	}

	// Once released left button, the selection is computed. This selection doesn't take buildings and units
	// there are both present. Or buildings (only one) or units (all of them)
	if (app->input->getMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		selector_init = false;
		bool units_only = false;    // If only one unit is on selection, buildings will be excluded
		map<uint, Entity*>::iterator it = active_entities.begin();

		// First, we need to know if any unit has been selected. 
		for (; it != active_entities.end(); ++it)
		{
			if (it->second->coll->checkCollision(selector))
			{
				if (it->second->type == UNIT)
					units_only = true;
			}
		}

		// Now, we include the entities according to the only_units boolean variable.
		for (it = active_entities.begin(); it != active_entities.end(); ++it)
		{
			if (it->second->coll->checkCollision(selector) && it->second->faction == PLAYER)
			{
				if (it->second->type == UNIT)
				{
					Unit *u = (Unit*)it->second;
					u->distance_to_center_selector = u->tile_pos - app->map->worldToMap(app->map->data.back(), selector.x + (selector.w / 2), selector.y + (selector.h / 2));
					selection.insert(pair<uint, Entity*>(it->first, it->second));
				}

				if (!units_only && it->second->type == BUILDING)
				{
					selection.insert(pair<uint, Entity*>(it->first, it->second));
					break;
				}

			}
		}
	}

	// Now, we give destination for the entities englobed by selection.
	if (!selection.empty() && app->input->getMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		// Target position is where the player has clicked to move his units.
		iPoint target_position;
		app->input->getMousePosition(target_position);
		target_position = app->render->screenToWorld(target_position.x, target_position.y);
		target_position = app->map->worldToMap(app->map->data.back(), target_position.x, target_position.y);

		Entity* e = whichEntityOnMouse();

		map<uint, Entity*>::iterator it = selection.begin();
		for (; it != selection.end(); ++it)
		{
			if (it->second->type == UNIT)
			{
				Unit *unit = (Unit*)it->second;
				if (selection.size() == 1)
				{
					if (app->path->createPath(unit->tile_pos, target_position) != -1)
					{
						unit->path = app->path->getLastPath();
						unit->has_target = true;
						unit->state = MOVE;
					}
				}
				else
				{
					iPoint target = target_position + unit->distance_to_center_selector;
					if (app->path->createPath(unit->tile_pos, target) != -1)
					{
						unit->path = app->path->getLastPath();
						unit->has_target = true;
						unit->state = MOVE;
					}
				}
				
			}
		}
	}

	// When we are in building mode, clicking left mouse button put the building on the clicked position.
	// The logic is modified to not walk over building.
	if (building_mode && app->input->getMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (app->map->isAreaWalkable(building_to_place->coll->rect))
		{
			building_to_place->id = ++next_ID;
			building_to_place->tile_pos = app->map->worldToMap(app->map->data.back(), building_to_place->center.x, building_to_place->center.y);
			active_entities.insert(pair<uint, Entity*>(next_ID, building_to_place));
			building_mode = false;

			app->map->changeLogic(building_to_place->coll->rect, NO_WALKABLE);
			recalculatePaths();
		}
	}
}

/*------------------WAVE RELATED METHODS--------------------------*/
void EntityManager::createWave(uint sizex, uint sizey, iPoint position)
{
	for (int i = 0; i < sizex; i++)
	{
		for (int j = 0; j < sizey; j++)
		{
			int posx = position.x + (sizex * i * 2);
			int posy = position.y + (sizey * j * 2);

			iPoint position = {posx, posy};

			createZergling(position);
		}
	}
}

bool EntityManager::searchNearEntity(Entity* e)
{
	bool ret = false;

	e->target_to_attack = NULL;
	e->target_to_repair = NULL;
	float value = e->range_of_vision;
	map<uint, Entity*>::iterator it = active_entities.begin();
	uint previousMaxHP = 10000;

	for (; it != active_entities.end(); ++it) //First and foremost the unit looks for the closest and weakest enemy
	{
		if (e->specialization != SCV && it->second != e && e->faction != it->second->faction && it->second->to_delete == false)
		{
			float d = abs(e->center.x - it->second->center.x) + abs(e->center.y - it->second->center.y);
			uint maxHP = it->second->current_hp;
			
			d -= ((e->coll->rect.w / 2 + e->coll->rect.h / 2) / 2 + (it->second->coll->rect.w / 2 + it->second->coll->rect.h / 2) / 2);

			//If the a unit is low on health it attacks it :). It is possible to kite zerglings now. However too dumb yet :D!
			if (e->target_to_attack == NULL && d <= value && maxHP <= previousMaxHP)
			{
				(e->target_to_attack) = &(*it->second);
				LOG("BUG NET");
				value = d;
				previousMaxHP = maxHP;
				ret = true;
			}
			else if (e->target_to_attack != NULL)
			{
				if ((e->target_to_attack->type == it->second->type && d <= value && maxHP <= previousMaxHP) || 
					(e->target_to_attack->type == BUILDING && it->second->type == UNIT && d <= e->range_of_vision && maxHP <= previousMaxHP))
				{
					(e->target_to_attack) = &(*it->second);
					LOG("BUG NET");
					value = d;
					previousMaxHP = maxHP;
					ret = true;
				}
			}
		}
		// We check if we are a SCV, the objective is a building and needs to be repared
		else if (e->specialization == SCV && it->second != NULL && it->second->type == BUILDING && (it->second->current_hp < it->second->max_hp))
		{
			float d = abs(e->center.x - it->second->center.x) + abs(e->center.y - it->second->center.y);
			d -= ((e->coll->rect.w / 2 + e->coll->rect.h / 2) / 2 + (it->second->coll->rect.w / 2 + it->second->coll->rect.h / 2) / 2);
			if (d <= value)
			{
				(e->target_to_repair) = &(*it->second);
				value = d;
				ret = true;
			}
		}
	}

	if (e->target_to_attack != NULL) // Second it does the calculus and changes the IA states
	{
		if (e->type == UNIT)
		{
			Unit* unit = (Unit*)e;
			unit->has_target = false;
			if (value <= e->range_to_attack) //If the entity isn't in the range of attack it changes the direction and state
			{
				e->state = ATTACK;
			}
			else
			{
				if (e->type == UNIT && app->path->createPath(e->tile_pos, e->target_to_attack->tile_pos) != -1) //the path to the selected entity is constructed
				{
					unit->has_target = true;
					unit->path = app->path->getLastPath();
					e->state = MOVE_ALERT;
				}
			}
		}
		else
		{
			if (value <= e->range_to_attack)
				e->state = ATTACK;

			else
				e->state = IDLE;
		}
	}
	else if (e->target_to_repair != NULL)// if we have a building that needs to be repaired
	{
		Unit* unit = (Unit*)e;
		unit->has_target = false;
		if (value <= e->range_to_attack)
		{
			e->state = REPAIR;
		}

	}

	return ret;
}

void EntityManager::createZergling(iPoint position)
{
	addEntity(position, ZERGLING);
}

//Deletes all units SELECTED
void EntityManager::deleteEntity(map<uint, Entity*> selection)
{
	vector<Entity* const> unitsto_delete;
	map<uint, Entity*>::iterator it;

	vector <Entity* const>::iterator itdel;
	bool must_delete = true;

	for (auto it = selection.cbegin(); it != selection.cend() /* not hoisted */; it++)
	{
		/*Stores entities in a vector that is going to be erased later
		(add into the .h to become accessible to erase them at another
		time*/

		std::map<uint, Entity*>::iterator itdel;
		itdel = active_entities.find(it->first);
		active_entities.erase(itdel);
	}

	selection.clear();
}

/*Method that deletes an entity*/
void EntityManager::deleteEntityKilled(Entity* e)
{
	vector <Entity* const>::iterator itdel;

	e->coll->to_delete = true;
	active_entities.erase(e->id);
	enemyJustDied = true;
	if (e->faction == COMPUTER)
		LOG("ZERGLING KILLED! Enemies remaining in the wave: ");
}

void EntityManager::KillEntity(map<uint, Entity*> selection)
{
	deleteEntity(selection);
}

void EntityManager::KillEntity(Entity* e)
{
	deleteEntityKilled(e);
}

void EntityManager::GetInsideBunker(Entity* e)
{
	Bunker* bunker = (Bunker*)e->target_to_reach;
	if (bunker->max_capacity != 0)
	{
		if (e->specialization == MARINE)
		{
			bunker->units_inside.insert(pair<uint, Entity*>(e->id, e));
			e->inside_bunker = true;
			e->to_delete = true;
			selection.erase(e->id);
			--bunker->max_capacity;
		}
	}
	else if(bunker->max_capacity == 0)
	{
		for (map<uint, Entity*>::iterator it2 = active_entities.begin(); it2 != active_entities.end(); ++it2)
		{
			if (bunker == it2->second->target_to_reach)
				it2->second->target_to_reach = nullptr;
		}
	}
}


//Deletes all the units in the screen (DEBUG PURPOSES ONLY)
void EntityManager::deleteAllEntities()
{
	map<uint, Entity*>::iterator it = active_entities.begin();
	for (; it != active_entities.end(); it++)
	{
		it->second->coll->to_delete = true;
		delete it->second;
	}	

	active_entities.clear();
	selection.clear();
}

void EntityManager::choosePlaceForBuilding()
{
	iPoint p; app->input->getMousePosition(p);
	iPoint pos = app->render->screenToWorld( p.x - building_to_place->tex_width / 2, p.y - building_to_place->tex_height / 2 );
	building_to_place->pos = { (float)pos.x, (float)pos.y };
	
	iPoint center = app->render->screenToWorld(p.x, p.y);
	building_to_place->center = { (float)center.x, (float)center.y };
	
	building_to_place->coll->setPos(building_to_place->pos.x + building_to_place->collider_offset.x, building_to_place->pos.y + building_to_place->collider_offset.y);

	iPoint first_tile = app->map->worldToMap(app->map->data.back(), building_to_place->coll->rect.x, building_to_place->coll->rect.y);
	iPoint last_tile = app->map->worldToMap(app->map->data.back(), building_to_place->coll->rect.x + building_to_place->coll->rect.w, building_to_place->coll->rect.y + building_to_place->coll->rect.h);
	iPoint point_to_draw;

	for (int y = first_tile.y; y < last_tile.y; ++y)
	{
		for (int x = first_tile.x; x < last_tile.x; ++x)
		{
			point_to_draw = app->map->mapToWorld(app->map->data.back(), x, y);
			if (app->path->isWalkable({ x, y }))
			{
				SDL_Rect r = { 1, 1, 8, 8 };
				app->render->blit(building_tile, point_to_draw.x, point_to_draw.y, &r);
			}
			else
			{
				SDL_Rect r = { 10, 1, 8, 8 };
				app->render->blit(building_tile, point_to_draw.x, point_to_draw.y, &r);
			}
		}
	}
	app->render->blit(building_to_place->tex, building_to_place->pos.x, building_to_place->pos.y, &building_to_place->current_animation->getCurrentFrame());
}

void EntityManager::recalculatePaths()
{
	map<uint, Entity*>::iterator it = active_entities.begin();
	for (; it != active_entities.end(); ++it)
	{
		if (it->second->type == UNIT)
		{
			Unit *unit = (Unit*)it->second;
			if (unit->path.size() > 0 && app->path->createPath(it->second->tile_pos, unit->path.back()) != -1)
				unit->path = app->path->getLastPath();
		}
	}
}

void EntityManager::onCollision(Collider* c1, Collider* c2)
{
	Entity *e1, *e2;
	e1 = e2 = NULL;

	map<uint, Entity*>::iterator it = active_entities.begin();
	for (; it != active_entities.end(); ++it)
	{
		if (it->second->coll == c1) e1 = it->second;
		else if (it->second->coll == c2) e2 = it->second;
	}
	
	if (e1 != NULL && e2 != NULL && e1->type == UNIT && e1->state == IDLE && e2->type == UNIT && e2->state == IDLE)
	{
		if (app->path->createPathToAdjacent(e2->tile_pos, 2) != -1)
		{
			Unit *unit = (Unit*)e2;
			unit->path = app->path->getLastPath();
			unit->has_target = true;
			unit->state = MOVE;
		}
	}
}