#pragma once

#include <SDL2/SDL_image.h>

#include "WindowHandler.h"
#include "InputHandler.h"

class Menu
{
public:
	enum menuItems {
		LEVEL_ONE,   LEVEL_ONE_H,
		LEVEL_TWO,   LEVEL_TWO_H,
		LEVEL_THREE, LEVEL_THREE_H,
		QUIT_GAME,   QUIT_GAME_H,
		TOTAL_ITEMS
	};
	
	Menu();
	~Menu();

	void loadMenuItems();
	void update(std::queue<GameEvent>& eventQueue);

	ScreenObject* title;
	ScreenObject* menuObjects[TOTAL_ITEMS];
	bool          highLightedButton[TOTAL_ITEMS];

	bool isOnButton(glm::vec2 inPos, glm::vec2 buttonPos, glm::vec2 buttonSize);
	
private:


};
