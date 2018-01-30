#pragma	once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class ScreenObject {
public:
	ScreenObject(SDL_Texture* tx, glm::vec2 pos, glm::vec2 sz);
	ScreenObject();
	~ScreenObject();

	glm::vec2    getPosition();
	glm::vec2    getSize();
	SDL_Rect     getSourceRect();
	SDL_Rect     getDestRect();
	SDL_Texture* getTexture();
	void         setSourceRect(glm::vec2 fullSize, glm::vec2 nrOfTiles, glm::vec2 rowColumnIndex);

protected:
	glm::vec2    position;
	glm::vec2    size;
	SDL_Texture* texture;
	SDL_Rect 	 sourceR;			

	void         setSourceRect();
};