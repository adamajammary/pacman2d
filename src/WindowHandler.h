#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "ScreenObject.h"

class WindowHandler {
public:
	//This ensures that there can only be one WindowHandler object at any given time.
	static WindowHandler& getInstance() {
		static WindowHandler instance;
		return instance;
	}

	bool init();
	void clear();//Clears the back buffer, making it ready for a new frame.
	//Loops through a list of ScreenObjects and calls the draw function on them.
	//This is used to prevent exessive calling of getInstance()
	void drawList(std::vector<ScreenObject*>* objects); 
	void draw(ScreenObject* object); //This should draw a ScreenObject* object or a child class there of.
	void update(); //Switches the front and back buffer
	void close();

	void showMessage(std::string message);

	SDL_Renderer* getRenderer();
	SDL_Surface*  getSurface();
	glm::vec2     getScreenSize();

	//This ensures that there can only be one WindowHandler object at any given time.
	WindowHandler(WindowHandler const& copy)  = delete;
	void operator=(WindowHandler const& copy) = delete;

private:
	SDL_Window*   window        = nullptr;
	SDL_Surface*  screenSurface = nullptr;
	SDL_Renderer* renderer      = nullptr;
	char windowName[100];
	int windowXSize;
	int windowYSize;

	WindowHandler() {};
	bool initSDL();
	void loadConfig();
};