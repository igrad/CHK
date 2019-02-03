#ifndef DECAL_LOADED
#define DECAL_LOADED

#include "essentials.h"
#include "LTexture.h"

enum RenderOrder {
   RENDER_BEHIND_ALL,
   RENDER_IN_ORDER,
   RENDER_ON_TOP
};

class Decal: public LTexture {
   public:
      Decal();

      // Fade out the alpha on part of this decal focused on a 2d point. An example of this might be a spell firing towards a target, passing through the "rood" portion of a wall, and fading out the alpha of the area around the projectile as it passes behind the "roof"
      void SetPartialAlphaOnPoint(int x, int y, float magnitude);

      // Same as above, but utilizes an entire rect instead of a single point
      void SetPartialAlphaOnRect(SDL_Rect* rect, float magnitude);

      ~Decal();

      RenderOrder renderOrder;

      bool attachedToEntity;
};

#endif
