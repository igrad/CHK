#ifndef STATICCOLLIDER_LOADED
#define STATICCOLLIDER_LOADED

#include "essentials.h"

#include "LTexture.h"
#include "Collider.h"

/**
* Class: StaticCollider
* Inherits: Collider
* Authors: Ian Gradert
* Description: Static collision item rendered on screen using a static LTexture.
*/
class StaticCollider: public Collider {
   public:
      StaticCollider();
      ~StaticCollider();

      LTexture* texture;
};

#endif
