#ifndef COLLISIONDETECTION_LOADED
#define COLLISIONDETECTION_LOADED

#include "essentials.h"
#include "Collider.h"
#include "Door.h"
#include "Actor.h"

bool PointInRect(int x, int y, SDL_Rect* b);
int IsPartialCollision(SDL_Rect* a, SDL_Rect* b);
bool IsWholeCollision(SDL_Rect* a, SDL_Rect* b);
bool IsRectCollision(SDL_Rect* a, SDL_Rect* b);
void CheckWallCollisions(Actor* actor, map<int, Collider>* others);
void CheckDoorCollisions(Actor* actor, Collider* other);

#endif
