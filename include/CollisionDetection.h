#ifndef COLLISIONDETECTION_LOADED
#define COLLISIONDETECTION_LOADED

#include "essentials.h"

bool PointInRect(int x, int y, SDL_Rect* b);

int IsPartialCollision(SDL_Rect* a, SDL_Rect* b);

bool IsWholeCollision(SDL_Rect* a, SDL_Rect* b);

bool IsRectCollision(SDL_Rect* a, SDL_Rect* b);

#endif
