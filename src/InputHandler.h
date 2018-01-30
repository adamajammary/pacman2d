#pragma	once

#include <SDL2/SDL.h>
#include <map>
#include <queue>
#include <glm/glm.hpp>
#include <chrono>

#include "GameEvent.h"

#define INACTIVE -1

class InputHandler {
public:
	// This ensures that there is only ever one instance of the InputHandler.
	static InputHandler& getInstance() {
		static InputHandler instance;
		return instance;
	}
	
	void      init();
	glm::vec2 getMousePosition();
	void      readInput(std::queue<GameEvent>& events); //Reads input events from SDL

	// This ensures that there is only ever one instance of the InputHandler.
	InputHandler(InputHandler const& copy) = delete;
	void operator=(InputHandler const& copy) = delete;
private:
	SDL_Event event;
	glm::vec2 mousePosition;

	std::map<ActionEnum, int> eventRepeat; //Map of action types and if they are reapeating.(Button is being held down.)
	std::map<ActionEnum, int> eventRepeatRate; //The rate at which an action is repeated.

	InputHandler() {};

	void handleKeys(std::queue<GameEvent>& events);
	void handleMouse(std::queue<GameEvent>& events);
};