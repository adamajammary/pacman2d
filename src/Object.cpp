#include "Object.h"
#include "WindowHandler.h"

Object::Object(SDL_Texture* tx, glm::vec2 pos, glm::vec2 sz) : ScreenObject(tx, pos, sz) {
	init();
};

Object::Object() {
	init();
};

void Object::init() {
	direction             = nextDirection = glm::vec2(0, 0);
	lives                 = -1;
	spawnPosition         = position;
	score                 = 0;
	pickUps[TOTAL_SOUNDS] = { NULL };
	type                  = OBJECT_UNKNOWN;
	velocity              = glm::vec2(0, 0);
}

void Object::animateSpriteChangeDirection() {
	switch (type) {
		case OBJECT_GHOST:
		case OBJECT_PACMAN:
			// ANIMATE LEFT
			if (direction.x < 0) {
				spriteIndex.y = 2;
			// ANIMATE RIGHT
			} else if (direction.x > 0) {
				spriteIndex.y = 3;
			//ANIMATE  UP
			} else if (direction.y < 0) {
				spriteIndex.y = 1;
			// ANIMATE DOWN
			} else if (direction.y > 0) {
				spriteIndex.y = 0;
			}
			break;
	}
}

void Object::animateSpriteIndex(float indexMin, float indexMax, float animationSpeed, float deltaTime) {
	if ((indexMin >= 0) && (indexMax >= 0)) {
		// ANIMATE BY SKIPPING TO THE NEXT TILE INDEX
		if (spriteIndex.x >= indexMax) {
			spriteIndex.x = indexMin;
		} else {
			spriteIndex.x += (animationSpeed * deltaTime);
		}

		// UPDATE THE SOURCE RECT TO USE THE NEW TILE INDEX
		setSourceRect(fullSize, nrOfTiles, spriteIndex);
	}
}

ObjectType Object::getMapItem(glm::vec2 mapIndex, const std::vector<std::vector<int>> &map) {
	int x = static_cast<int>(mapIndex.x);
	int y = static_cast<int>(mapIndex.y);

	// INVALID INDEX (ROW, CELL) IN MAP
	if ((y < 0) || (x < 0) || (y >= static_cast<int>(map.size())) || (x >= static_cast<int>(map[y].size()))) {
		return OBJECT_UNKNOWN;
	}

	return (ObjectType)map[y][x];
}

std::vector<Object*> Object::getMapObjectType(ObjectType objType, const std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &items) {
	Object*              object;
	std::vector<Object*> objects;

	// FIND AND RETURN THE OBECTS OF THE SPECIFIED TYPE
	for (auto item : items) {
		object = static_cast<Object*>(item);

		if (object->getType() == objType) {
			objects.push_back(object);
		}
	}

	return objects;
}

Object* Object::getMapObjectLocation(glm::vec2 mapIndex, const std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &items) {
	Object* object;

	// FIND AND RETURN THE OBECT IN THE SPECIFIED MAP LOCATION
	for (auto item : items) {
		object = static_cast<Object*>(item);

		if (object->getMapLocation() == mapIndex) {
			return object;
		}
	}

	return nullptr;
}

glm::vec2 Object::getMapLocation() {
	return glm::vec2(std::roundf(position.x / size.x), std::roundf(position.y / size.y));
}

int Object::getScore() {
	return score;
}

int Object::getLives() {
	return lives;
}

ObjectType Object::getType() {
	return type;
}

bool Object::isCollision(glm::vec2 objectDirection, const std::vector<std::vector<int>> &map) {
	int mapTileX = static_cast<int>(std::roundf(position.x / size.x));
	int mapTileY = static_cast<int>(std::roundf(position.y / size.y));

	// CHECK COLLISION LEFT
	if ((objectDirection.x < 0) && (getMapItem(glm::vec2(mapTileX - 1, mapTileY), map) != OBJECT_WALL_NON_WALKABLE)) {
		return false;
	// CHECK COLLISION RIGHT
	} else if ((objectDirection.x > 0) && (getMapItem(glm::vec2(mapTileX + 1, mapTileY), map) != OBJECT_WALL_NON_WALKABLE)) {
		return false;
	// CHECK COLLISION UP
	} else if ((objectDirection.y < 0) && (getMapItem(glm::vec2(mapTileX, mapTileY - 1), map) != OBJECT_WALL_NON_WALKABLE)) {
		return false;
	// CHECK COLLISION DOWN
	} else if ((objectDirection.y > 0) && (getMapItem(glm::vec2(mapTileX, mapTileY + 1), map) != OBJECT_WALL_NON_WALKABLE)) {
		return false;
	}

	return true;
}

// Returns true if the object could move successfully without colliding, otherwise false.
bool Object::move(const std::vector<std::vector<int>> &map, float deltaTime) {
	// KEEP MOVING AS LONG AS THERE ARE NO COLLISIONS IN THE CURRENT DIRECTION, 
	if (!isCollision(direction, map) ||
		// ALSO MOVE UNTIL COMPLETING A TILE SIZE (AVOID MOVING IN THE MIDDLE OF THE TILE)
		(((direction.x != 0) && ((int)position.x % (int)size.x > ALLOWED_PIXEL_OFFSET)) ||
		((direction.y  != 0) && ((int)position.y % (int)size.y > ALLOWED_PIXEL_OFFSET))))
	{
		position.x += (direction.x * (velocity.x * deltaTime));
		position.y += (direction.y * (velocity.y * deltaTime));

		return true;
	}

	return false;
}

void Object::moveGhost(const std::vector<std::vector<int>> &map, float deltaTime) {
	float randomDirection;
	int   randdomIterations;

	// TRY MOVING IN THE CURRENT DIRECTION UNTIL A COLLISION OCCURS
	if (!move(map, deltaTime)) {
		for (int j = 0; j < 4; j++) {
			randomDirection   = -1.0f;
			randdomIterations = (rand() % 2);

			// GET A NEW RANDOM DIRECTION (1 OR -1)
			for (int i = 0; i < randdomIterations; i++) {
				randomDirection *= randomDirection;
			}

			// TRY MOVING IN THE CURRENT DIRECTION UNTIL A COLLISION OCCURS
			if (!move(map, deltaTime)) {
				// CURRENTLY MOVING VERTICALLY - TRY MOVING HORIZONTALLY
				if (direction.y != 0) {
					direction = glm::vec2(randomDirection, 0);
				// CURRENTLY MOVING HORIZONTALLY - TRY MOVING VERTICALLY
				} else if (direction.x != 0) {
					direction = glm::vec2(0, randomDirection);
				}
			} else {
				// CHANGE DIRECTION SPRITE FOR ANIMATION
				animateSpriteChangeDirection();
				
				// NO COLLISION - EXIT THE LOOP
				break;
			}
		}
	}
}

void Object::movePacman(const std::vector<std::vector<int>> &map, float deltaTime) {
	// CHANGE DIRECTION IF NO COLLISION WILL OCCUR
	if (!isCollision(nextDirection, map) && ((nextDirection.x != 0) || (nextDirection.y != 0))) {
		// ONLY CHANGE DIRECTION EVERY COMPLETE TILE SIZE (AVOID CHANGE IN THE MIDDLE OF THE TILE)
		if (((direction.x != 0) && ((int)position.x % (int)size.x <= ALLOWED_PIXEL_OFFSET)) ||
			((direction.y != 0) && ((int)position.y % (int)size.y <= ALLOWED_PIXEL_OFFSET)))
		{
			direction     = nextDirection;
			nextDirection = glm::vec2(0, 0);

			// CHANGE DIRECTION SPRITE FOR ANIMATION
			animateSpriteChangeDirection();
		}
	}

	// KEEP MOVING AS LONG AS THERE ARE NO COLLISIONS IN THE CURRENT DIRECTION, 
	// ALSO MOVE UNTIL COMPLETING A TILE SIZE (AVOID MOVING IN THE MIDDLE OF THE TILE)
	if (move(map, deltaTime)) {
		// RESUME PLAYING MUSIC WHEN MOVING
		if (Mix_PausedMusic()) {
			Mix_ResumeMusic();
		}
	} else {
		// PAUSE MUSIC WHEN NOT MOVING
		if (!Mix_PausedMusic()) {
			Mix_PauseMusic();
		}
	}
}

void Object::pickup(glm::vec2 mapIndex, std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &walls, const std::vector<ScreenObject*> &objects) {
	std::vector<ScreenObject*> items;
	Object*                    mapObject;
	std::vector<Object*>       orbs;
	int                        x = static_cast<int>(mapIndex.x);
	int                        y = static_cast<int>(mapIndex.y);

	// HANDLE COLLISION WITH ORBS AND FRUITS
	switch (map[y][x]) {
		case OBJECT_ORB: case OBJECT_CHERRY: case OBJECT_STRAWBERRY: case OBJECT_ORANGE: case OBJECT_APPLE: case OBJECT_PINEAPPLE:
			switch (map[y][x]) {
				case OBJECT_ORB:        score += SCORE_ORB;        items = walls;                                break;
				case OBJECT_CHERRY:     score += SCORE_CHERRY;     items = objects; playChunk(PACMAN_EAT_FRUIT); break;
				case OBJECT_STRAWBERRY: score += SCORE_STRAWBERRY; items = objects; playChunk(PACMAN_EAT_FRUIT); break;
				case OBJECT_ORANGE:     score += SCORE_ORANGE;     items = objects; playChunk(PACMAN_EAT_FRUIT); break;
				case OBJECT_APPLE:      score += SCORE_APPLE;      items = objects; playChunk(PACMAN_EAT_FRUIT); break;
				case OBJECT_PINEAPPLE:  score += SCORE_PINEAPPLE;  items = objects; playChunk(PACMAN_EAT_FRUIT); break;
			}

			// FIND THE OBJECT IN THE SAME LOCATION AS PACMAN
			mapObject = getMapObjectLocation(mapIndex, map, items);

			// PICK UP THE OBJECT AND RESET THE MAP ITEM TYPE
			if (mapObject != nullptr) {
				mapObject->resetSprite();
				map[y][x] = OBJECT_WALL_WALKABLE;
			}

			break;
	}
}

void Object::resetSprite() {
	// RESET AND UPDATE THE SPRITE
	setSpriteInfo(OBJECT_WALL_WALKABLE, fullSize, nrOfTiles, glm::vec2(0, spriteIndex.y));
	setSourceRect(fullSize, nrOfTiles, spriteIndex);
}

void Object::respawn() {
	// RESET POSITION AND SPRITE
	position = spawnPosition;
	setSpriteInfo(type, fullSize, nrOfTiles, spriteSpawnIndex);
	setSourceRect(fullSize, nrOfTiles, spriteIndex);
}

void Object::setSpriteInfo(ObjectType objType, glm::vec2 size, glm::vec2 noTiles, glm::vec2 idx) {
	float randomDirection;
	int   randdomIterations;

	type      = objType;
	fullSize  = size;
	nrOfTiles = noTiles;
	spriteIndex = spriteSpawnIndex = idx;

	switch (type) {
		case OBJECT_PACMAN:
			direction = nextDirection = glm::vec2(1.0f, 0);
			velocity  = glm::vec2(100.0f, 100.0f);
			lives     = (lives == -1 ? 3 : lives);	// SETS LIVES FIRST TIME, BUT LEAVES CURRENT VALUE WHEN RESPAWN.

			break;
		case OBJECT_GHOST:
			randomDirection   = -1.0f;
			randdomIterations = (rand() % 2);

			// GET A NEW RANDOM DIRECTION (1 OR -1)
			for (int i = 0; i < randdomIterations; i++) {
				randomDirection *= randomDirection;
			}

			direction = nextDirection = glm::vec2(randomDirection, 0);
			velocity  = glm::vec2((rand() % 50 + 50), (rand() % 50 + 50));

			break;
	}
}

void Object::update(float deltaTime, std::vector<std::vector<int>> &map, const std::vector<ScreenObject*> &walls, const std::vector<ScreenObject*> &objects) {
	Object*   ghost;
	glm::vec2 mapLocation;

	// ANIMATE THE OBJECTS
	switch (type) {
		case OBJECT_GHOST:  animateSpriteIndex(4.0f, 5.4f, 5.0f, deltaTime); break;
		case OBJECT_PACMAN: animateSpriteIndex(0,    3.4f, 5.0f, deltaTime); break;
	}

	// HANDLE MOVEMENT AND COLLISION
	switch (type) {
	case OBJECT_GHOST:
		moveGhost(map, deltaTime);
		wrap(); // Added wrap
		break;
	case OBJECT_PACMAN:
		movePacman(map, deltaTime);

		// GET THE CURRENT MAP LOCATION OF PACMAN
		mapLocation = getMapLocation();

		// PICK UP ORBS AND FRUITS
		pickup(mapLocation, map, walls, objects);

		// RESPAWN PACKMAN AND THE GHOST IF THEY ARE STANDING ON THE SAME TILE
		ghost = getMapObjectLocation(mapLocation, map, objects);

		if ((ghost != nullptr) && (ghost->getType() == OBJECT_GHOST)) {
			playChunk(PACMAN_EAT_GHOST);
			ghost->respawn();
			respawn();
			lives--;
		}

		// WRAP PACMAN TO THE OTHER SIDE OF THE SCREEN
		wrap();

		break;
	}
}

void Object::update(float deltaTime, glm::vec2 objectDirection) {
	// SAVE THE REQUESTED DIRECTION CHANGE
	switch (type) {
	case OBJECT_PACMAN:
		nextDirection = objectDirection;
		break;
	}
}

void Object::wrap() {
	glm::vec2 screenSize = WindowHandler::getInstance().getScreenSize();

	// WRAP RIGHT SIDE
	if ((position.x + size.x) > screenSize.x) {
		position.x = 0;
	// WRAP LEFT SIDE
	} else if (position.x < 0) {
		position.x = (screenSize.x - size.x);
	// WRAP BOTTOM SIDE
	} else if ((position.y + size.y) > screenSize.y) {
		position.y = 0;
	// WRAP TOP SIDE
	} else if (position.y < 0) {
		position.y = (screenSize.y - size.y);
	}
}

void Object::loadSound() {
	// load chunks
	pickUps[PACMAN_START] = Mix_LoadWAV("audio/pacman_beginning.wav");
	if (pickUps[PACMAN_START] == NULL) {
		printf("Unable to load PACMAN_START sound");
	}

	pickUps[PACMAN_DEATH] = Mix_LoadWAV("audio/pacman_death.wav");
	if (pickUps[PACMAN_DEATH] == NULL) {
		printf("Unable to load PACMAN_DEATH sound");
	}

	pickUps[PACMAN_EAT_FRUIT] = Mix_LoadWAV("audio/pacman_eatfruit.wav");
	if (pickUps[PACMAN_EAT_FRUIT] == NULL) {
		printf("Unable to load PACMAN_EAT_FRUIT sound");
	}

	pickUps[PACMAN_EAT_GHOST] = Mix_LoadWAV("audio/pacman_eatghost.wav");
	if (pickUps[PACMAN_EAT_GHOST] == NULL) {
		printf("Unable to load PACMAN_EAT_GHOST sound");
	}

	pickUps[PACMAN_INTERMISSION] = Mix_LoadWAV("audio/pacman_intermission.wav");
	if (pickUps[PACMAN_INTERMISSION] == NULL) {
		printf("Unable to load PACMAN_INTERMISSION sound");
	}
}

void Object::playChunk(ObjectSounds sounds) {
	if (Mix_PlayChannel(-1, pickUps[sounds], 0) == -1) {
		printf("Failed to play sound chunk ID: %d\n", sounds);
	}	
}
