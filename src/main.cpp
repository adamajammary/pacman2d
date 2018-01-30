#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

#include "globals.h"
#include "GameEvent.h"
#include "InputHandler.h"
#include "Level.h"
#include "mainMenu.h"
#include "ScreenObject.h"
#include "WindowHandler.h"

//Creates a Level object for each line in gLEVELS_FILE andplace it it the gLevels vector.
//Those lines are paths of files with map data.
//See Level::loadMap for more information.
//Returns a pointer to the first Level object (currentLevel).
//Level* loadLevels() {
void loadLevels() {
	FILE*       file = fopen(gLEVELS_FILE, "r");
	Level*      level;
	int         nrOfFiles;
	std::string tempString;

	fscanf(file, "Number of files: %d", &nrOfFiles);

	for (auto level : gLevels) {
		delete level;
	}

	gLevels.clear();

	gLevels.push_back(nullptr);

	for (int i = 1; i <= nrOfFiles; i++) {
		char tempCString[51];

		fscanf(file, "%50s", &tempCString);
		tempString = tempCString;

		level = new Level(tempString, i);

		gLevels.push_back(level);
	}

	fclose(file);
	file = nullptr;

	gLevel = 0;

	//return gLevels.front();
}

//Initializes the InputHandler and WindowHandler
//The WindowHandler intitilizes SDL
//Loads levels and returns a pointer to the first Level by calling loadLevels()
//Level* init() {
void init() {
	// Initializes SDL_mixer
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)				//AUDIO
	{
		printf("Audio did not initiated!");
		gRunning = false;
		
	}

	InputHandler::getInstance().init();

	if(!WindowHandler::getInstance().init()) {
		gRunning = false;
	}

	//return loadLevels();
}

//While there are events in the eventQueue. Process those events. 
void update(float deltaTime, std::queue<GameEvent>& eventQueue, Level* currentLevel) {
	glm::vec2 direction;
	GameEvent nextEvent;

	while(!eventQueue.empty()) {
		nextEvent = eventQueue.front();
		eventQueue.pop();

		switch (nextEvent.action) {
			case ActionEnum::STOP_GAME:         currentLevel->stopGame();        continue;
			case ActionEnum::PAUSE_GAME:        currentLevel->pauseGame();       continue;
			case ActionEnum::PLAYER_MOVE_UP:    direction = glm::vec2(0, -1.0f); break;
			case ActionEnum::PLAYER_MOVE_LEFT:  direction = glm::vec2(-1.0f, 0); break;
			case ActionEnum::PLAYER_MOVE_DOWN:  direction = glm::vec2(0, 1.0f);  break;
			case ActionEnum::PLAYER_MOVE_RIGHT: direction = glm::vec2(1.0f, 0);  break;
			default: continue;
		}

		// UPDATE CHANGE IN DIRECTION
		if (deltaTime < 1.0f) {
			currentLevel->update(deltaTime, direction);
		}
	}

	// UPDATE ANIMATION AND MOVEMENT (COLLISION DETECTION)
	if (deltaTime < 1.0f) {
		currentLevel->update(deltaTime);
	}
}

//This is the main draw function of the program. All calls to the WindowHandler for drawing purposes should originate here.
void draw(Level* currentLevel) {
	if (currentLevel != nullptr) {
		WindowHandler::getInstance().clear();

		WindowHandler::getInstance().drawList(currentLevel->getWalls());
		//Draw other things here...
		WindowHandler::getInstance().drawList(currentLevel->getTextLabels());
		WindowHandler::getInstance().drawList(currentLevel->getObjects());

		WindowHandler::getInstance().update();
	}
}

// DRAWS MENU
void draw(Menu* menu)
{
	if (menu != nullptr) {
		WindowHandler::getInstance().clear();

		WindowHandler::getInstance().draw(menu->title);

		WindowHandler::getInstance().draw(menu->menuObjects[menu->highLightedButton[Menu::LEVEL_ONE]   ? Menu::LEVEL_ONE_H   : Menu::LEVEL_ONE]);
		WindowHandler::getInstance().draw(menu->menuObjects[menu->highLightedButton[Menu::LEVEL_TWO]   ? Menu::LEVEL_TWO_H   : Menu::LEVEL_TWO]);
		WindowHandler::getInstance().draw(menu->menuObjects[menu->highLightedButton[Menu::LEVEL_THREE] ? Menu::LEVEL_THREE_H : Menu::LEVEL_THREE]);
		WindowHandler::getInstance().draw(menu->menuObjects[menu->highLightedButton[Menu::QUIT_GAME]   ? Menu::QUIT_GAME_H   : Menu::QUIT_GAME]);

		WindowHandler::getInstance().update();
	}
}

//Calls cleanup code on program exit.
void close() {
	WindowHandler::getInstance().close();
}

int main(int argc, char *argv[]) {
	std::queue<GameEvent> eventQueue;	//Main event queue for the program.
	Level* currentLevel   = nullptr;
	float  nextFrame      = 1/gFpsGoal; //Time between frames in seconds
	float  nextFrameTimer = 0.0f;		//Time from last frame in seconds
	float  deltaTime      = 0.0f;		//Time since last pass through of the game loop.
	auto   clockStart     = std::chrono::high_resolution_clock::now(); //Clock used for timing purposes
	auto   clockStop      = clockStart;

	//currentLevel = init();
	init();

	Menu* menu = new Menu();

	while (gRunning) {
		// INIT AND RECREATE LEVELS IF NEEDED
		if (gLevel < 0) {
			loadLevels();
		}

		// QUIT IF AN INVALID LEVEL HAS BEEN SET
		if ((gLevel < 0) || (gLevel >= (int)gLevels.size())) {
			gRunning = false;
			continue;
		}

		// SET THE CURRENT LEVEL
		currentLevel = gLevels[gLevel];

		// QUIT IF LOADING THE LEVEL FAILED
		if ((menu == nullptr) || ((gLevel > 0) && (currentLevel == nullptr))) {
			gRunning = false;
			continue;
		}

		clockStart = std::chrono::high_resolution_clock::now();

		InputHandler::getInstance().readInput(eventQueue);

		// LOAD THE MENU
		if (gLevel == 0) {
			menu->update(eventQueue);
			draw(menu);
		// LOAD THE CURRENT LEVEL
		} else {
			update(deltaTime, eventQueue, currentLevel);

			if(nextFrameTimer >= nextFrame) {
				draw(currentLevel);
				nextFrameTimer = 0.0f;
			}
		}

		clockStop = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(clockStop - clockStart).count();
		//nextFrame += deltaTime;
		nextFrameTimer += deltaTime;
	}

	close();
	return 0;
}