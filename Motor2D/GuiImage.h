#ifndef __GUIIMAGE_H__
#define __GUIIMAGE_H__

#include "GuiElements.h"

class GuiImage : public GuiElements
{
public:
	GuiImage(const SDL_Texture* texture);
	GuiImage(const SDL_Texture* texture, const SDL_Rect& section);
	~GuiImage();

	void setSection(const SDL_Rect& section);
	void draw() const;

private:

	SDL_Rect section;
	const SDL_Texture* texture = nullptr;

};

#endif __GUIIMAGE_H__


