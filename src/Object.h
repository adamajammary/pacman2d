#pragma once

#include <vector>
#include <SDL2/SDL_mixer.h>

#include "ScreenObject.h"

const int ALLOWED_PIXEL_OFFSET = 5;

enum ObjectType {
	OBJECT_UNKNOWN = -1, OBJECT_WALL_WALKABLE, OBJECT_WALL_NON_WALKABLE,
	OBJECT_ORB, OBJECT_CHERRY, OBJECT_STRAWBERRY, OBJECT_ORANGE, OBJECT_APPLE, OBJECT_PINEAPPLE,
	OBJECT_PACMAN, OBJECT_GHOST, TOTAL_OBJECTS
};

enum ObjectSounds {
	PACMAN_START, PACMAN_DEATH, PACMAN_EAT_FRUIT, PACMAN_EAT_GHOST, PACMAN_INTERMISSION, TOTAL_SOUNDS
};

enum Scores {
	SCORE_ORB = 10, SCORE_CHERRY = 100, SCORE_STRAWBERRY = 200, SCORE_ORANGE = 300, SCORE_APPLE = 400, SCORE_PINEAPPLE = 500
};

class Object : public ScreenObject {
public:
	Object(SDL_Texture* tx, glm::vec2 pos, glm::vec2 sz);
	Object();
	~Object() {};

	glm::vec2 spawnPosition;

	std::vector<Object*> getMapObjectType(ObjectType objType, const std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &items);
	int                  getLives();
	int                  getScore();
	ObjectType           getType();
	void	             loadSound();
	void	             playChunk(ObjectSounds sound);
	void                 setSpriteInfo(ObjectType objType, glm::vec2 size, glm::vec2 noTiles, glm::vec2 idx);
	void                 update(float deltaTime, std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &walls, const std::vector<ScreenObject*> &objects);
	void                 update(float deltaTime, glm::vec2 objectDirection);

private:
	glm::vec2  direction;
	glm::vec2  fullSize;
	int        lives;
	glm::vec2  nextDirection;
	glm::vec2  nrOfTiles;
	Mix_Chunk* pickUps[TOTAL_SOUNDS];
	int        score;
	glm::vec2  spriteIndex;
	glm::vec2  spriteSpawnIndex;
	ObjectType type;
	glm::vec2  velocity;

	void       animateSpriteChangeDirection();
	void       animateSpriteIndex(float indexMin, float indexMax, float animationSpeed, float deltaTime);
	ObjectType getMapItem(glm::vec2 mapIndex, const std::vector<std::vector<int>> &map);
	Object*    getMapObjectLocation(glm::vec2 mapIndex, const std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &items);
	glm::vec2  getMapLocation();
	void       init();
	bool       isCollision(glm::vec2 objectDirection, const std::vector<std::vector<int>> &map);
	bool       move(const std::vector<std::vector<int>> &map, float deltaTime);
	void       moveGhost(const std::vector<std::vector<int>> &map, float deltaTime);
	void       movePacman(const std::vector<std::vector<int>> &map, float deltaTime);
	void       pickup(glm::vec2 mapIndex, std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &walls, const std::vector<ScreenObject*> &objects);
	void       resetSprite();
	void       respawn();
	void       wrap();
};
