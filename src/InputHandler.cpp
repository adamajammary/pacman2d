#include <SDL2/SDL.h>

#include "globals.h"
#include "InputHandler.h"

void InputHandler::init() {
	//Add new events here and in GameEvent.h
	eventRepeatRate[ActionEnum::QUIT] = 0;
	eventRepeat[ActionEnum::QUIT] = INACTIVE;

	eventRepeatRate[ActionEnum::PAUSE_GAME] = 0;
	eventRepeat[ActionEnum::PAUSE_GAME] = INACTIVE;

	eventRepeatRate[ActionEnum::STOP_GAME] = 0;
	eventRepeat[ActionEnum::STOP_GAME] = INACTIVE;

	eventRepeatRate[ActionEnum::MOUSEBUTTONDOWN] = 0;
	eventRepeat[ActionEnum::MOUSEBUTTONDOWN] = INACTIVE;

	eventRepeatRate[ActionEnum::MOUSEMOTION] = 0;
	eventRepeat[ActionEnum::MOUSEMOTION] = INACTIVE;

	eventRepeatRate[ActionEnum::PLAYER_MOVE_UP] = 0;
	eventRepeat[ActionEnum::PLAYER_MOVE_UP] = INACTIVE;

	eventRepeatRate[ActionEnum::PLAYER_MOVE_LEFT] = 0;
	eventRepeat[ActionEnum::PLAYER_MOVE_LEFT] = INACTIVE;

	eventRepeatRate[ActionEnum::PLAYER_MOVE_DOWN] = 0;
	eventRepeat[ActionEnum::PLAYER_MOVE_DOWN] = INACTIVE;

	eventRepeatRate[ActionEnum::PLAYER_MOVE_RIGHT] = 0;
	eventRepeat[ActionEnum::PLAYER_MOVE_RIGHT] = INACTIVE;
}

void InputHandler::readInput(std::queue<GameEvent>& events) {
	//Handle new events on queue
	while (SDL_PollEvent(&event) != 0) {
		//User requests quit
		if (event.type == SDL_QUIT) {
			gRunning = false;
		} else if (	event.type == SDL_KEYDOWN ||
					event.type == SDL_KEYUP)
		{
			handleKeys(events);
		} else if (	event.type == SDL_MOUSEBUTTONDOWN ||
					event.type == SDL_MOUSEBUTTONUP ||
					event.type == SDL_MOUSEMOTION ||
					event.type == SDL_MOUSEWHEEL)
		{
			handleMouse(events);
		}
	}
	
	//This will trigger all events that have finnished cool down
	//This code is currently not connected to deltaTime. Considder doing so.
	for (auto it = eventRepeat.begin(); it != eventRepeat.end(); ++it) {
		if (it->second != INACTIVE) {
			if (it->second == 0) {
				events.push(GameEvent{ 0, it->first }); // raise the event
				it->second = eventRepeatRate[it->first];  // set to cool down
			} else {
				--it->second;  // cool down toward triggering
			}
		}
	}
}

void InputHandler::handleKeys(std::queue<GameEvent>& events) {
	ActionEnum action = ActionEnum::NOACTION;

	switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE: action = ActionEnum::STOP_GAME;         break;
				case SDLK_SPACE:  action = ActionEnum::PAUSE_GAME;        break;
				case SDLK_w:      action = ActionEnum::PLAYER_MOVE_UP;    break;
				case SDLK_a:      action = ActionEnum::PLAYER_MOVE_LEFT;  break;
				case SDLK_s:      action = ActionEnum::PLAYER_MOVE_DOWN;  break;
				case SDLK_d:      action = ActionEnum::PLAYER_MOVE_RIGHT; break;
				default: break;
			}

			//This checks if the event is active and on cool down
			if (eventRepeat[action] < 0) {
				events.push(GameEvent{ 0, action }); //trigger event
				//eventRepeat[action] = eventRepeatRate[action]; // set the repeat cool down
			}

			break;
		default:
			break;
	}
}
void InputHandler::handleMouse(std::queue<GameEvent>& events) {
    ActionEnum action = ActionEnum::NOACTION;
    
    switch (event.type)
    {
    	// ADDED FOR BUTTON PRESS 
        case SDL_MOUSEBUTTONDOWN:
            mousePosition.x = static_cast<float>(event.button.x);
            mousePosition.y = static_cast<float>(event.button.y);
            action          = ActionEnum::MOUSEBUTTONDOWN;
            break;
        /*case SDL_MOUSEBUTTONUP:
            break;
        case SDL_MOUSEWHEEL:
            break;*/
        case SDL_MOUSEMOTION:
            mousePosition.x = static_cast<float>(event.motion.x);
            mousePosition.y = static_cast<float>(event.motion.y);
            action          = ActionEnum::MOUSEMOTION;
            break;
    }

    if (eventRepeat[action] < 0) {
        events.push(GameEvent{ 0, action }); //trigger event
        //eventRepeat[action] = eventRepeatRate[action]; // set the repeat cool down
    }
}

glm::vec2 InputHandler::getMousePosition()
{
	return mousePosition;
}
