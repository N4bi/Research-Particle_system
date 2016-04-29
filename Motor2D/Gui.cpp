#include "App.h"
#include "p2Log.h"
#include "Render.h"
#include "PathFinding.h"
#include "Input.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Gui.h"
#include "Map.h"
#include "Window.h"
#include "Textures.h"

#include "GuiImage.h"
#include "GuiCursor.h"
#include "GuiLabel.h"

#include "Bunker.h"

using namespace std;

Gui::Gui() : Module()
{
	name.assign("gui");
}

// Destructor
Gui::~Gui()
{}

// Called before render is available
bool Gui::awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name.assign(conf.child("atlas").attribute("file").as_string());

	return ret;
}

// Called before the first frame
bool Gui::start()
{
	atlas = app->tex->loadTexture(atlas_file_name.data());

	// CURSOR-----------------------------------------------------------------
	SDL_ShowCursor(SDL_DISABLE);
	cursor = app->gui->createCursor(app->tex->loadTexture("Cursor/StarCraftCursors.png"));
	uint w, h; app->win->getWindowSize(w, h);
	cursor_offset.x = (w * 0.01f); // 10% of map width
	cursor_offset.y = (h * 0.01f); // 10% of map height
	scroll_speed = 1.0f;

	map_limits = { app->map->data.front().width * app->map->data.front().tile_width,
		app->map->data.front().height * app->map->data.front().tile_height };

	// CIRCLES OF SELECTIOM----------------------------------------------------
	circles_of_selection = app->tex->loadTexture("Selection/Selection_circles.png");
	lifes_tex = app->tex->loadTexture("Cursor/StarCraftCursors.png");

	green_life = { 486, 3, 4, 5 };
	yellow_life = { 496, 3, 4, 5 };
	red_life = { 501, 3, 4, 5 };
	white_life = { 491, 3, 4, 5 };	

	// To show walkability on building mode-------------------------
	path_walkability = app->tex->loadTexture("maps/Path_tiles.png");

	return true;
}

void Gui::onGui(GuiElements* ui, GUI_EVENTS event)
{
	
}

void Gui::drawHudSelection(SPECIALIZATION  selection)
{
	
	
}

// Update all guis
bool Gui::preUpdate()
{
	return true;
}

bool Gui::update(float dt)
{

	if (app->input->getKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	// Cursor -> check for camera displacement.
	iPoint pos = cursor->getLocalPos();
	cursor->current_animation = &cursor->idle;
	// Checking displacement for X axis.
	if (pos.x < cursor_offset.x) // Left
	{
		app->render->camera.x += (app->render->camera.x + (scroll_speed * dt) <= 0 ? (scroll_speed * dt) : -app->render->camera.x);
		cursor->current_animation = &cursor->left_disp;
	}
	else if (pos.x > (app->render->camera.w - cursor_offset.x)) // Right
	{
		app->render->camera.x -= (app->render->camera.x - (scroll_speed * dt) >= app->render->camera.w - map_limits.x ? (scroll_speed * dt) : map_limits.x - app->render->camera.w + app->render->camera.x);
		cursor->current_animation = &cursor->right_disp;
	}
	// Checking displacement for Y axis.
	if (pos.y < cursor_offset.y) // Up
	{
		app->render->camera.y += (app->render->camera.y + (scroll_speed * dt) <= 0 ? (scroll_speed * dt) : -app->render->camera.y);
		cursor->current_animation = &cursor->up_disp;
	}
	else if (pos.y > (app->render->camera.h - cursor_offset.y)) // Down
	{
		app->render->camera.y -= (app->render->camera.y - (scroll_speed * dt) >= app->render->camera.h - map_limits.y ? (scroll_speed * dt) : map_limits.y - app->render->camera.h + app->render->camera.y);
		cursor->current_animation = &cursor->down_disp;
	}		

	const GuiElements* mouse_hover = findMouseHover();
	if (mouse_hover &&
		mouse_hover->can_focus == true &&
		app->input->getMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		focus = mouse_hover;	
	
	// Now the iteration for input and update
	for (list<GuiElements*>::iterator node = elements.begin(); node != elements.end(); node++)
	{
		GuiElements* gui_test = *node;
		if (gui_test->interactive == true)
		    gui_test->checkInput(mouse_hover, focus);
	}
		

	for (list<GuiElements*>::iterator node = elements.begin(); node != elements.end(); node++)
	{
		GuiElements* gui_test = *node;
		gui_test->update(mouse_hover, focus);
		if (gui_test->getType() == CURSOR)
		{
			gui_test->update();
		}
	}

	return true;
}

// Called after all Updates
bool Gui::postUpdate()
{
	//Check if we have entities selected
	if (app->entity_manager->selection.size() == 0)
		drawHudSelection(SPECIALIZATION::NOTYPE);

	multimap<float, Entity*> to_draw_selection;
	for (map<uint, Entity*>::iterator it = app->entity_manager->selection.begin(); it != app->entity_manager->selection.end(); ++it)
		to_draw_selection.insert(pair<float, Entity*>(it->second->center.y, it->second));

	// Blit of Selection Circles and Lifes
	for (multimap<float, Entity*>::iterator itm = to_draw_selection.begin(); itm != to_draw_selection.end(); ++itm)
	{
		// Circle selection blitting
		Entity *e = itm->second;
		app->render->blit(circles_of_selection, e->center.x - e->selection_type.w / 2, e->center.y - e->circle_selection_offset.y, &e->selection_type);

		// Life selection blitting
		if (e->current_hp > 0)
		{
			uint bars_to_draw = ceil((e->max_hp_bars * ((float)e->current_hp / (float)e->max_hp)) + 1);
			uint bar = 1;
			for (; bar < bars_to_draw; ++bar)
				app->render->blit(lifes_tex, e->center.x + e->offset_life.x + (bar * green_life.w), e->center.y + e->offset_life.y, &green_life);
			for (; bar <= e->max_hp_bars; ++bar)
				app->render->blit(lifes_tex, e->center.x + e->offset_life.x + (bar * white_life.w), e->center.y + e->offset_life.y, &white_life);
		}

		if (itm->second->specialization == SPECIALIZATION::COMMANDCENTER)
			drawHudSelection(COMMANDCENTER);
		if (itm->second->specialization == SPECIALIZATION::BUNKER)
			drawHudSelection(BUNKER);
	}
	
	// CRZ -> possibilty of blitting according to Y value.
	// Entity drawing
	multimap<float, Entity*> to_draw;
	for (map<uint, Entity*>::iterator it = app->entity_manager->active_entities.begin(); it != app->entity_manager->active_entities.end(); ++it)
		to_draw.insert(pair<float, Entity*>(it->second->center.y, it->second));	

	for (multimap<float, Entity*>::iterator itm = to_draw.begin(); itm != to_draw.end(); ++itm)
		itm->second->draw();

	for (list<GuiElements*>::iterator item = elements.begin(); item != elements.end(); item++)
	{
		GuiElements* gui = *item;
		if (gui->draw_element == true)
		{
			if (gui->getType() != CURSOR)
				gui->draw();
		}

		if (debug == true && gui->draw_element == true)
			gui->debugDraw();
	}

	cursor->draw();

	if (debug)
	{
		iPoint to_draw; app->input->getMousePosition(to_draw);
		to_draw = app->render->screenToWorld(to_draw.x, to_draw.y);

		if (app->path->isWalkable(app->map->worldToMap(app->map->data.back(), to_draw.x, to_draw.y)))
		{
			SDL_Rect r = { 1, 1, 8, 8 };
			app->render->blit(path_walkability, to_draw.x, to_draw.y, &r);
		}
		else
		{
			SDL_Rect r = { 10, 1, 8, 8 };
			app->render->blit(path_walkability, to_draw.x, to_draw.y, &r);
		}
	}	
	
	return true;
}

// Called before quitting
bool Gui::cleanUp()
{
	LOG("Freeing GUI");

	list<GuiElements*>::iterator item;

	for (item = elements.begin(); item != elements.end(); item++)
		RELEASE(*item);

	elements.clear();

	return true;
}

// Create a simple image
GuiImage* Gui::createImage(const char* filename)
{
	GuiImage* ret = NULL;
	SDL_Texture* texture = app->tex->loadTexture(filename);

	if (texture != NULL)
	{
		ret = new GuiImage(texture);
		elements.push_back(ret);
	}

	return ret;
}

// Create a simple image
GuiImage* Gui::createImage(const SDL_Texture* texture, const SDL_Rect& section)
{
	GuiImage* ret = NULL;
	if (texture == NULL)
	{
		texture = atlas;
	}
	ret = new GuiImage(texture, section);
	elements.push_back(ret);

	return ret;
}

GuiLabel* Gui::createLabel(const char* text, int kind_of_font)
{
	GuiLabel* ret = NULL;

	if (text != NULL)
	{
		ret = new GuiLabel(text,kind_of_font);
		elements.push_back(ret);
	}

	return ret;
}

//Create cursor
GuiCursor* Gui::createCursor(const SDL_Texture* texture){

	GuiCursor* ret = NULL;

	ret = new GuiCursor(texture);
	elements.push_back(ret);
	
	return ret;
}

//
SDL_Rect Gui::mouseQuad(iPoint init_mouse)
{
	iPoint current_mouse;
	app->input->getMousePosition(current_mouse);
	SDL_Rect rect = { init_mouse.x, init_mouse.y, current_mouse.x - init_mouse.x, current_mouse.y - init_mouse.y };
	app->render->DrawQuad(rect, 65, 105, 225, 100, true, false);
	return rect;
}

// Called after all Updates
const GuiElements* Gui::findMouseHover()
{
	iPoint mouse;
	app->input->getMousePosition(mouse);

	for (list<GuiElements*>::reverse_iterator item = elements.rbegin(); item != elements.rend(); ++item)
	{
		GuiElements* gui_test = *item;
		if (gui_test->interactive == true)
		{
			SDL_Rect r = gui_test->getScreenRect();
			if (mouse.x >= r.x && mouse.x <= r.x + r.w && mouse.y >= r.y && mouse.y <= r.y + r.y)
			{
				//elements.reverse();
                return gui_test;
			}	
		}
	}

	//elements.reverse();
	return nullptr;
}

// const getter for atlas
const SDL_Texture* Gui::getAtlas() const
{
	return atlas;
}




