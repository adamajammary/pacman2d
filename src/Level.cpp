#include <time.h>
#include <SDL2/SDL_mixer.h>

#include "globals.h"
#include "Level.h"
#include "Text.h"
#include "WindowHandler.h"

Level::Level(std::string filepath, int nr) {
	level  = nr;
	player = nullptr;

	loadMap(filepath);
	loadMusic("audio/WakaWaka.wav");
	loadTextures();
	createWalls();
	createObjects(OBJECT_CHERRY);
	createObjects(OBJECT_STRAWBERRY);
	createObjects(OBJECT_ORANGE);
	createObjects(OBJECT_APPLE);
	createObjects(OBJECT_PINEAPPLE);
	createObjects(OBJECT_GHOST);
	createObjects(OBJECT_PACMAN);

	// Seed the random number generator
	std::srand(static_cast<unsigned int>(time(nullptr)));
}

Level::~Level() {
	for (auto texture : textures) {
		SDL_DestroyTexture(texture); texture = nullptr;
	}

	for (auto text : textLabels) {
		delete text; text = nullptr;
	}

	for (auto wall : walls) {
		delete wall; wall = nullptr;
	}

	for (auto object : objects) {
		delete object; object = nullptr;
	}

	textLabels.clear();
	walls.clear();
	objects.clear();
	map.clear();
};

void Level::loadTextures() {
	SDL_Surface* surface = nullptr;

	for (int i = TEXTURE_PACMAN; i < TOTAL_TEXTURES; i++) {
		switch (i) {
		case TEXTURE_PACMAN:
			surface = IMG_Load(std::string("assets/pacman.png").c_str());
			break;
		default:
			surface = IMG_Load(std::string("assets/mapTiles").append(std::to_string(i)).append(".png").c_str());
			break;
		}

		if (surface != nullptr) {
			textures[i]    = SDL_CreateTextureFromSurface(WindowHandler::getInstance().getRenderer(), surface);
			textureSize[i] = glm::vec2(surface->w, surface->h);

			SDL_FreeSurface(surface);
		}
	}
}

//Uses the map data to create ScreenObjects that represent the walls of the map.
void Level::createWalls() {
	// TODO
	glm::vec2    spriteIndex, screenPosition, spriteSize;
	int          rows       = map.size();
	int          cells      = 0;
	glm::vec2    windowSize = WindowHandler::getInstance().getScreenSize();

	if (textures[level] == nullptr)
	{
		printf("Unable to load image for level walls! SDL_image Error: %s\n", IMG_GetError());
	}
	else
	{
		for (int i = 0; i < rows; i++) {
			cells = map[i].size();

			for (int j = 0; j < cells; j++) {
				// CALCULATE TILE SIZE, AND POSITION ON THE MAP (WINDOW)
				spriteSize.x      = (windowSize.x / cells);
				spriteSize.y      = (windowSize.y / rows);
				screenPosition.x  = (spriteSize.x * j);
				screenPosition.y  = (spriteSize.y * i);

				Object* wall = new Object(textures[level], screenPosition, spriteSize);

				switch (map[i][j]) {
					case OBJECT_WALL_NON_WALKABLE: spriteIndex = glm::vec2(OBJECT_WALL_NON_WALKABLE, 0); break;
					case OBJECT_ORB:               spriteIndex = glm::vec2(OBJECT_ORB, 0);               break;
					default:                       spriteIndex = glm::vec2(OBJECT_WALL_WALKABLE, 0);     break;
				}

				wall->setSpriteInfo(static_cast<ObjectType>(map[i][j]), textureSize[level], glm::vec2(8, 1), spriteIndex);
				wall->setSourceRect(textureSize[level], glm::vec2(8, 1), spriteIndex);

				walls.push_back(static_cast<ScreenObject*>(wall));
			}
		}
	}
}

void Level::createObjects(ObjectType type) {
	glm::vec2    totalSprites, spriteIndex, screenPosition, spriteSize;
	int          rows       = map.size();
	int          cells      = 0;
	SDL_Texture* texture    = nullptr;
	glm::vec2    texSize;
	glm::vec2    windowSize = WindowHandler::getInstance().getScreenSize();
	
	// SELECT CORRECT IMAGE FILE
	switch (type) {
		case OBJECT_PACMAN: case OBJECT_GHOST:
			texture = textures[TEXTURE_PACMAN];
			texSize = textureSize[TEXTURE_PACMAN];
			break;
		case OBJECT_CHERRY: case OBJECT_STRAWBERRY: case OBJECT_ORANGE: case OBJECT_APPLE: case OBJECT_PINEAPPLE:
			texture = textures[level];
			texSize = textureSize[level];
			break;
	}

	if (texture == nullptr)
	{
		printf("Unable to load image for level objects! SDL_image Error: %s\n", IMG_GetError());
	}
	else
	{
		// ITERATE MAP ITEMS
		for (int row = 0; row < rows; row++) {
			cells = map[row].size();

			for (int cell = 0; cell < cells; cell++) {
				// ONLY CREATE SELECTED OBJECT TYPE - OTHERWISE SKIP MAP ITEM
				if (map[row][cell] != type) {
					continue;
				}

				// SELECT SPRITE INDEX (CELL, ROW) IN THE LARGER IMAGE
				switch (map[row][cell]) {
					case OBJECT_PACMAN:
						spriteIndex  = glm::vec2(0, 3);
						totalSprites = glm::vec2(6, 4);
						break;
					case OBJECT_GHOST:
						spriteIndex  = glm::vec2(4, 2);
						totalSprites = glm::vec2(6, 4);
						break;
					case OBJECT_CHERRY: case OBJECT_STRAWBERRY: case OBJECT_ORANGE: case OBJECT_APPLE: case OBJECT_PINEAPPLE:
						spriteIndex  = glm::vec2(map[row][cell], 0);
						totalSprites = glm::vec2(8, 1);
						break;
					default:
						continue; // INVALID OBJECT TYPE
				}

				// CALCULATE TILE SIZE, AND POSITION ON THE MAP (WINDOW)
				spriteSize.x      = (windowSize.x / cells);
				spriteSize.y      = (windowSize.y / rows);
				screenPosition.x  = (spriteSize.x * cell);
				screenPosition.y  = (spriteSize.y * row);

				// CREATE THE OBJECT AND IT TO THE RENDER LIST
				Object* object = new Object(texture, screenPosition, spriteSize);

				object->setSpriteInfo(static_cast<ObjectType>(map[row][cell]), texSize, totalSprites, spriteIndex);
				object->setSourceRect(texSize, totalSprites, spriteIndex);

				objects.push_back(static_cast<ScreenObject*>(object));

				if (map[row][cell] == OBJECT_PACMAN) {
					player = object;
					player->loadSound();	// AUDIO
				}
			}
		}
	}
}

std::vector<ScreenObject*>* Level::getObjects() {
	return &objects;
}

std::vector<ScreenObject*>* Level::getTextLabels() {
	return &textLabels;
}


std::vector<ScreenObject*>* Level::getWalls() {
	return &walls;
}

/*
Reads a text file containing map data in the following format:
5x5
1 1 1 1 1
1 0 0 0 1
2 0 1 0 0
1 0 0 0 1
1 1 1 1 1
The first line defines the dimensions of the map
The following matrix has 1s representing walls and 0s representing open space.
2 represents Pac Mans start position.
*/
void Level::loadMap(std::string filepath) {
	int x, y, temp;
	FILE* file = fopen(filepath.c_str(), "r");
	
	fscanf(file, "%dx%d", &x, &y);

	for (int i = 0; i < y; i++) {
		std::vector<int> row;

		for (int j = 0; j < x; j++) {
			fscanf(file, " %d", &temp);
			row.push_back(temp);
		}

		map.push_back(row);
	}

	fclose(file);
	file = nullptr;
}

void Level::loadMusic(std::string filepath) {
	music = Mix_LoadMUS("audio/WakaWaka.wav");

	if (music == nullptr) {
		printf("Unable to load %s\n", "audio/WakaWaka.wav");
	}
}

void Level::playMusic()
{
	if ((music != nullptr) && !Mix_PlayingMusic()) {
		if (Mix_PlayMusic(music, -1) == -1) {
			printf("Unable to play '%s': %s\n", "audio/WakaWaka.wav", Mix_GetError());
		} else {
			Mix_VolumeMusic(20);	// MIX_MAX_VOLUME = 128
		}
	}
}

void Level::stopMusic() {
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}
}

void Level::pauseGame() {
	if (Mix_PlayingMusic()) {
		Mix_PauseMusic();
	}

	WindowHandler::getInstance().showMessage("GAME IS PAUSED - PRESS OK TO RESUME");

	if (Mix_PausedMusic()) {
		playMusic();
	}
}

void Level::stopGame() {
	stopMusic();
	gLevel = -1;
}

void Level::setHUD() {
	ScreenObject* object;
	SDL_Surface*  surface;
	Text*         text;

	// RELEASE THE MEMORY ALLOCATED FOR PREVIOUS TEXT LABELS
	if (!textLabels.empty()) {
		for (auto text : textLabels) {
			delete text;
		}

		textLabels.clear();
	}

	// CREATE A TEXT LABEL FOR PLAYER SCORE, AND ADD IT TO THE DRAW LIST.
	//text    = Text(std::string("SCORE: ").append(std::to_string(player->getScore())), false, false);	// NORMAL - NO KERNING
	//text    = Text(std::string("SCORE: ").append(std::to_string(player->getScore())), false, true);		// NORMAL - KERNING
	//text    = Text(std::string("SCORE: ").append(std::to_string(player->getScore())), true,  false);	// BOLD   - NO KERNING
	text    = new Text(std::string(" SCORE: ").append(std::to_string(player->getScore())), true,  true);	// BOLD   - KERNING
	surface = text->getSurface();

	if (surface != nullptr) {
		object = new ScreenObject(
			SDL_CreateTextureFromSurface(WindowHandler::getInstance().getRenderer(), surface),
			glm::vec2(10, 10), glm::vec2(surface->w, surface->h)
		);
		textLabels.push_back(object);
	}

	delete text;

	// CREATE A TEXT LABEL FOR PLAYER LIVES, AND ADD IT TO THE DRAW LIST.
	text = new Text(std::string(" LIVES: ").append(std::to_string(player->getLives())), true, true);
	surface = text->getSurface();

	if (surface != nullptr) {
		object = new ScreenObject(
			SDL_CreateTextureFromSurface(WindowHandler::getInstance().getRenderer(), surface),
			glm::vec2(270, 10), glm::vec2(surface->w, surface->h)
		);
		textLabels.push_back(object);
	}

	delete text;

	// CREATE A TEXT LABEL FOR PLAYER LIVES, AND ADD IT TO THE DRAW LIST.
	text = new Text(std::string(" LEVEL: ").append(std::to_string(level)), true, true);
	surface = text->getSurface();

	if (surface != nullptr) {
		object = new ScreenObject(
			SDL_CreateTextureFromSurface(WindowHandler::getInstance().getRenderer(), surface),
			glm::vec2(500, 10), glm::vec2(surface->w, surface->h)
		);
		textLabels.push_back(object);
	}

	delete text;
}

void Level::update(float deltaTime) {
	Object* object;

	for (auto screenObject : objects) {
		object = static_cast<Object*>(screenObject);

		if (object != nullptr) {
			setHUD();
			object->update(deltaTime, map, walls, objects);
		}
	}

	if ((gLevel > 0) && !Mix_PlayingMusic()) {
		playMusic();
	}

	// CHECK HOW MANY LIVES ARE LEFT
	if (player->getLives() <= 0) {

		// TODO: EXIT LEVEL - GO TO MENU

		stopMusic();
		player->playChunk(PACMAN_DEATH);

		WindowHandler::getInstance().showMessage("GAME OVER - YOU DIED !!!");

		gLevel = -1;

	} else {
		// CHECK HOW MANY ORBS ARE LEFT
		if (player->getMapObjectType(OBJECT_ORB, map, walls).empty()) {
			
			// TODO: EXIT LEVEL - GO TO MENU

			stopMusic();
			player->playChunk(PACMAN_INTERMISSION);

			WindowHandler::getInstance().showMessage("CONGRATULATIONS - YOU WON !!!");

			gLevel++;

			if (gLevel >= static_cast<int>(gLevels.size())) {
				gLevel = -1;
			}

		}
	}
}

void Level::update(float deltaTime, glm::vec2 direction) {
	for (auto object : objects) {
		static_cast<Object*>(object)->update(deltaTime, direction);
	}
}
