#include "globals.h"

const char gWINDOW_CONFIGURATION_FILE[] = "settings/windowConfig";
const char gLEVELS_FILE[]               = "levels/levelsList";

int   gLevel   = -1;
bool  gRunning = true;
float gFpsGoal = 60.0f;

std::vector<Level*> gLevels;

Uint32 color32(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 alpha)
{
    return (Uint32(alpha) << 24) + (Uint32(r) << 16) + (Uint32(g) << 8) + Uint32(b);
}