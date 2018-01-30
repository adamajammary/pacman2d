#include "ScreenObject.h"
#include "WindowHandler.h"

ScreenObject::ScreenObject(SDL_Texture* tx, glm::vec2 pos, glm::vec2 sz) {
	texture  = tx;
	position = pos;
	size     = sz;

	setSourceRect();
}

ScreenObject::ScreenObject() {
	texture  = nullptr;
	position = glm::vec2(0, 0);
	size     = glm::vec2(0, 0);

	setSourceRect();
}

ScreenObject::~ScreenObject() {
	SDL_DestroyTexture(texture);
};

glm::vec2 ScreenObject::getPosition() {
	return position;
}

glm::vec2 ScreenObject::getSize() {
	return size;
}

SDL_Rect ScreenObject::getSourceRect() {
	return sourceR;
}

SDL_Rect ScreenObject::getDestRect() {
	SDL_Rect rect;

	rect.x = static_cast<int>(position.x);
	rect.y = static_cast<int>(position.y);
	rect.w = static_cast<int>(size.x);
	rect.h = static_cast<int>(size.y);

	return rect;
}

SDL_Texture* ScreenObject::getTexture() {
	return texture;
}

// LOAD FULL SPRITE
void ScreenObject::setSourceRect()
{
	sourceR.x = 0;
	sourceR.y = 0;
	sourceR.w = static_cast<int>(size.x);
	sourceR.h = static_cast<int>(size.y);
}

// LOAD INDEXED SPRITE/TILE FROM A LARGER IMAGE
void ScreenObject::setSourceRect(glm::vec2 fullSize, glm::vec2 nrOfTiles, glm::vec2 rowColumnIndex)
{
	sourceR.w = static_cast<int>(fullSize.x / nrOfTiles.x);
	sourceR.h = static_cast<int>(fullSize.y / nrOfTiles.y);
	sourceR.x = static_cast<int>(static_cast<int>(rowColumnIndex.x) * sourceR.w);
	sourceR.y = static_cast<int>(static_cast<int>(rowColumnIndex.y) * sourceR.h);
}