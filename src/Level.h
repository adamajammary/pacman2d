#pragma	once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "SDL2/SDL_image.h"

#include "Object.h"
#include "ScreenObject.h"

enum Textures {
	TEXTURE_PACMAN, TEXTURE_MAPTILES1, TEXTURE_MAPTILES2, TEXTURE_MAPTILES3, TOTAL_TEXTURES
};

class Level {
public:
	Level(std::string filepath, int nr);
	~Level();

	std::vector<ScreenObject*>* getWalls();
	std::vector<ScreenObject*>* getTextLabels();
	std::vector<ScreenObject*>* getObjects();
	void                        pauseGame();
	void                        stopGame();
	void                        setHUD();
	void                        update(float deltaTime);
	void                        update(float deltaTime, glm::vec2 direction);

private:
	int                           level;
	std::vector<std::vector<int>> map;
	Mix_Music*                    music;
	std::vector<ScreenObject*>    objects;
	Object*                       player;
	std::vector<ScreenObject*>    textLabels;
	SDL_Texture*                  textures[TOTAL_TEXTURES];
	glm::vec2                     textureSize[TOTAL_TEXTURES];
	std::vector<ScreenObject*>    walls;

	void createWalls();						//This should create a ScreenObject* for each wall segment.
	void createObjects(ObjectType type);
	void loadMap(std::string filepath);		//Loads map data from file
	void loadMusic(std::string filepath);
	void loadTextures();
	void playMusic();
	void stopMusic();

};