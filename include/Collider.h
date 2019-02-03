#ifndef COLLIDER_LOADED
#define COLLIDER_LOADED

#include "essentials.h"

/**
* Class: Collider
* Inherits: none
* Authors: Ian Gradert
* Description: Basic collision box used for inheritance in other classes.
*/
class Collider {
   public:
      Collider();
      ~Collider();

      void SetPos(int x = -1, int y = -1);

      void SetHitBox(int x = -1, int y = -1, int w = -1, int h = -1);

      void SetDrawBox(int x = -1, int y = -1, int w = -1, int h = -1);

      void HandleCollisions();

      // Origin and point of reference for the hitBox and drawBox
      int xPos;
      int yPos;

      // Because the collider's hitbox may not necessary correspond with their origin on screen, we separate the two values.
      SDL_Rect hitBox;
      SDL_Rect drawBox;
};

#endif