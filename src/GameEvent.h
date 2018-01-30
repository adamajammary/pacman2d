#pragma once

//Add new events here and in InputHandler::init()
enum class ActionEnum :int {
    NOACTION = 0,
    MOUSEBUTTONDOWN, // ADDED FOR BUTTON PRESS
    MOUSEMOTION,
    PLAYER_MOVE_UP, PLAYER_MOVE_LEFT, PLAYER_MOVE_DOWN, PLAYER_MOVE_RIGHT,
	PAUSE_GAME, STOP_GAME, QUIT,
    NUMBER_OF_GAME_EVENTS
};

/* This struct deals with an agent creating events.  This has an agent number and the action*/
struct GameEvent {
    int agent;
    ActionEnum action;
};