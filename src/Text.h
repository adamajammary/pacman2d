#pragma once

#include <string>
#include <SDL2/SDL.h>

#include "ScreenObject.h"

class Text {
public:
	Text(std::string text, bool bold, bool kerning);
	Text() {};
	~Text();

	SDL_Surface* getSurface();

private:
	SDL_Surface* fontBitmap;
	SDL_Surface* surface;

	void createSurface(std::string text, bool bold, bool kerning);
};

