#include "globals.h"
#include "mainMenu.h"
#include "Object.h"

Menu::Menu() {
	loadMenuItems();
}

Menu::~Menu() {

}

// CREATES MENU
void Menu::loadMenuItems()
{
	const int       offsetY            = 100;
	const int       numberOfButtons    = TOTAL_ITEMS / 2;
	SDL_Surface*	menuTitleSurface   = IMG_Load("assets/title.png");
	SDL_Surface*	menuButtonsSurface = IMG_Load("assets/newbuttons.png");
	SDL_Texture*    menuTitleTexture   = NULL;
	SDL_Texture*    menuButtonsTexture = NULL;
	glm::vec2		windowSize         = WindowHandler::getInstance().getScreenSize();
	glm::vec2		position           = glm::vec2(0, 0);
	glm::vec2		size               = glm::vec2(200, 50);
	Object*	        button;
	Mix_Music*      music;
	
	memset(highLightedButton, false, TOTAL_ITEMS);

	if (menuTitleSurface != NULL)
	{
		menuTitleTexture = SDL_CreateTextureFromSurface(WindowHandler::getInstance().getRenderer(), menuTitleSurface);
	}

	if (menuButtonsSurface != NULL)
	{
		menuButtonsTexture = SDL_CreateTextureFromSurface(WindowHandler::getInstance().getRenderer(), menuButtonsSurface);
	}

	if (menuTitleTexture == NULL || menuButtonsTexture == NULL)
	{
		printf("Unable to load image %s and %s! SDL_image Error: %s\n", "assets/title.png","assets/newbuttons.png",  IMG_GetError());
	}
	else
	{
		title = new ScreenObject(menuTitleTexture, glm::vec2(0,0), windowSize);
		
		for (int j = 0; j < numberOfButtons; j++)	// rows
		{
			for (int i = 0; i < 2; i++)	// cells
			{
				if (i % 2 == 0) {
					position.x = ((windowSize.x - size.x) / 2);
					position.y = (windowSize.y - ((numberOfButtons - 1 - j) * size.y) - offsetY);
				}

				button = new Object(menuButtonsTexture, position, size);
				button->setSourceRect(glm::vec2(menuButtonsSurface->w, menuButtonsSurface->h), glm::vec2(2, numberOfButtons), glm::vec2(i, j)); 

				menuObjects[(j * 2) + i] = button;
			}
		}

		SDL_FreeSurface(menuTitleSurface);
		SDL_FreeSurface(menuButtonsSurface);
	}

	music = Mix_LoadMUS("audio/pacman_beginning.wav");

	if (music == nullptr) {
		printf("Unable to load %s\n", "audio/pacman_beginning.wav");
	} else {
		if (Mix_PlayMusic(music, 0) == -1) {
			printf("Unable to play '%s': %s\n", "audio/pacman_beginning.wav", Mix_GetError());
		} else {
			Mix_VolumeMusic(100);	// MIX_MAX_VOLUME = 128
		}
	}
}

// CHECK IS ON BUTTON
bool Menu::isOnButton(glm::vec2 inPos, glm::vec2 buttonPos, glm::vec2 buttonSize)
{
	return inPos.x >= buttonPos.x && inPos.x <= buttonPos.x + buttonSize.x
			&& inPos.y >= buttonPos.y && inPos.y <= buttonPos.y + buttonSize.y;
}

// HANDLE MOUSE EVENTS
void Menu::update(std::queue<GameEvent>& eventQueue)
{
	GameEvent nextEvent;

	while(!eventQueue.empty())
	{
		nextEvent = eventQueue.front();
		eventQueue.pop();

		switch(nextEvent.action)
		{
			case ActionEnum::STOP_GAME:
				gRunning = false;
				break;
			case ActionEnum::MOUSEMOTION:
				highLightedButton[LEVEL_ONE]   = isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[LEVEL_ONE]->getPosition(),   menuObjects[LEVEL_ONE]->getSize());
				highLightedButton[LEVEL_TWO]   = isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[LEVEL_TWO]->getPosition(),   menuObjects[LEVEL_TWO]->getSize());
				highLightedButton[LEVEL_THREE] = isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[LEVEL_THREE]->getPosition(), menuObjects[LEVEL_THREE]->getSize());
				highLightedButton[QUIT_GAME]   = isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[QUIT_GAME]->getPosition(),   menuObjects[QUIT_GAME]->getSize());
				break;
			case ActionEnum::MOUSEBUTTONDOWN:
				if (isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[LEVEL_ONE]->getPosition(), menuObjects[LEVEL_ONE]->getSize())) {
					gLevel = 1;
					if (Mix_PlayingMusic()) {
						Mix_HaltMusic();
					}
				}

				if (isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[LEVEL_TWO]->getPosition(), menuObjects[LEVEL_TWO]->getSize())) {
					gLevel = 2;
					if (Mix_PlayingMusic()) {
						Mix_HaltMusic();
					}
				}

				if (isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[LEVEL_THREE]->getPosition(), menuObjects[LEVEL_THREE]->getSize())) {
					gLevel = 3;
					if (Mix_PlayingMusic()) {
						Mix_HaltMusic();
					}
				}

				if (isOnButton(InputHandler::getInstance().getMousePosition(), menuObjects[QUIT_GAME]->getPosition(), menuObjects[QUIT_GAME]->getSize())) {
					gRunning = false;
					if (Mix_PlayingMusic()) {
						Mix_HaltMusic();
					}
				}

				break;
			default:
				break;
		}
	}
}
