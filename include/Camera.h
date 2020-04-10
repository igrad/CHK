#ifndef CAMERA_LOADED
#define CAMERA_LOADED

#include "essentials.h"
#include "Actor.h"

class Actor;

enum FOCUSMODE {
   FOCUS_PLAYER,
   FOCUS_AREA
};

class Camera {
   public:
      static void Update();
      const static SDL_Rect GetRect();
      static void SetXDirPos();
      static void SetXDirNeg();
      static void SetYDirPos();
      static void SetYDirNeg();
      static void SetFocusOnRect(SDL_Rect r);
      static void SetFocusOnPoint(double x, double y);
      static void SetFocusOnActor(Actor* a);

      static FOCUSMODE focusMode;
      static double x;
      static double y;
      static double xSpeed;
      static double ySpeed;
      static int updateX;
      static int updateY;
      static float zoom;
};

#endif
