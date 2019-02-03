#ifndef ACTOR_LOADED
#define ACTOR_LOADED

#include "essentials.h"

#include "LTexture.h"
#include "Collider.h"
#include "Animation.h"


// PHASE_ANIMS_OPENS and PHASE_TEXT_OPENS are used for doors, gates, windows, chests... anything that opens and closes and has static textures displayed while in an open/close state
// Count: 2
enum PHASE_ANIMS_OPENS {
   ANIM_OPEN,
   ANIM_CLOSE
};

enum PHASE_TEXT_OPENS {
   TEXT_CLOSE,
   TEXT_OPEN
};

// Actor with melee combat capabilities and some special abilities
// Count: 16
enum PHASE_CHARACTER {
   ANIM_IDLE_I,
   ANIM_IDLE_II,
   ANIM_IDLE_III,
   ANIM_IDLE_IV,
   ANIM_WALK_I,
   ANIM_WALK_II,
   ANIM_WALK_III,
   ANIM_WALK_IV,
   ANIM_JUMP,
   ANIM_KICK,
   ANIM_ATTACK_1,
   ANIM_ATTACK_2,
   ANIM_ATTACK_3,
   ANIM_ABILITY_1,
   ANIM_ABILITY_2,
   ANIM_ABILITY_3
};



/**
* Class: Actor
* Inherits: Collider
* Authors: Ian Gradert
* Description: Any entity in the game that moves or displays a dynamic image
* on-screen. This class essentially just allows for easier maintenance of an
* Animation object being rendered.
*/
class Actor: public Collider {
   public:
      Actor(int numAnims, int numTextures);

      void SetHitBoxSize(int x = 0, int y = 0, int w = 0, int h = 0);

      void SetDrawBoxSize(int x = 0, int y = 0, int w = 0, int h = 0);

      void SetZoom(float newZoom);

      bool LoadAnimation(int phase, string path, int frames, float duration, int frameW, int frameH);

      void SetActiveAnim(int anim, bool adoptDefaultDrawBox = false);

      bool LoadTexture(int phase, string path);

      void SetActiveTexture(int phase, bool adoptDefaultDrawBox = false);

      void Render(int screenFrame);

      void Free();

      ~Actor();

      bool changingRender;

      float zoom;

      bool usingAnims;

      bool hasAnimations;
      bool hasTextures;

      int numAnims;
      Animation* anims;
      int activeAnim;

      int numTextures;
      LTexture* textures;
      int activeTexture;
};

#endif