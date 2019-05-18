#ifndef CAMERA_LOADED
#define CAMERA_LOADED

#include "essentials.h"
#include "Actor.h"

enum FOCUSMODE {
   FOCUS_PLAYER,
   FOCUS_AREA
};

class Camera {
   public:
      static void SetFocusOnRect(SDL_Rect r);
      static void SetFocusOnPoint(double x, double y);
      static void SetFocusOnActor(Actor* a);

      static FOCUSMODE focusMode;
      static double x;
      static double y;
      static float zoom;
};

#endif
