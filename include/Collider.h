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

      Collider* operator=(const Collider* o);

      SDL_Rect* GetHitBox();
      SDL_Rect* GetDrawBox();

      void GetFoot(double* x, double* y);
      void GetFoot(int* x, int* y);
      void GetCoM(double* x, double* y);
      void GetCoM(int* x, int* y);

      void SetXPos(double x = -999);
      void SetYPos(double y = -999);
      void SetPos(double x = -999, double y = -999);
      void SetHitBox(int x = -1, int y = -1, int w = -1, int h = -1);
      void SetDrawBox(int x = -1, int y = -1, int w = -1, int h = -1);
      void SetHitBoxOffsets(int x = 0, int y = 0);

      ~Collider();

      bool queueCollisions;

      // Origin and point of reference for the hitBox and drawBox
      double xPos;
      double yPos;

      // The point at which, if something's y value is greater than this value,
      // the other object is "ahead of" (rendered after) this collider
      int drawCutoff;

      // Because the collider's hitbox may not necessary correspond with their origin on screen, we separate the two values.
      SDL_Rect hitBox;
      SDL_Rect drawBox;
      int hitBoxXOffset;
      int hitBoxYOffset;
};

#endif
