#pragma	once

#include <vector>

#include "Level.h"
#include "ScreenObject.h"

extern const char gWINDOW_CONFIGURATION_FILE[];
extern const char gLEVELS_FILE[];

extern int   gLevel;
extern bool  gRunning;
extern float gFpsGoal;

extern std::vector<Level*> gLevels;

Uint32 color32(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 alpha);