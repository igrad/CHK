#ifndef ACTOR_LOADED
#define ACTOR_LOADED

#include "essentials.h"

#include "Camera.h"
#include "LTexture.h"
#include "Collider.h"
#include "Animation.h"


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

// Simple doors
// Count: 8
enum PHASE_DOOR_TEXT {
   TEXT_CLOSED,
   TEXT_OPENED,
};

enum PHASE_DOOR_ANIM {
   ANIM_OPEN_SLOW,
   ANIM_OPEN_FAST,
   ANIM_CLOSE_SLOW,
   ANIM_CLOSE_FAST,
   ANIM_BREAK_IN,
   ANIM_BREAK_OUT
};

class Level;



class RenderedActor: public Collider {
public:
   bool CheckRendering();
   virtual void Render(int screenFrame);

   bool isRendering;
};



/**
* Class: Actor
* Inherits: Collider
* Authors: Ian Gradert
* Description: Any entity in the game that moves or displays a dynamic image
* on-screen. This class essentially just allows for easier maintenance of an
* Animation object being rendered.
*/
class Actor: virtual public RenderedActor {
public:
   Actor();
   Actor(int numAnims, int numTextures, bool addToAllActors = true, 
      bool unique = true);

   void SetHitBoxSize(int x = 0, int y = 0, int w = 0, int h = 0);
   void SetDrawBoxSize(int x = 0, int y = 0, int w = 0, int h = 0);
   void SetZoom(float newZoom);

   bool LoadAnimation(int phase, string path, int frames, float duration,
      int frameW, int frameH, bool reversed = false);
   bool LoadAnimation(int phase, LTexture* ref, int frames, float duration,
      int frameW, int frameH, bool reversed = false);
   void SetActiveAnim(int anim, bool loop = false);
   bool LoadTexture(int phase, string path);
   bool LoadTexture(int phase, LTexture* ref);
   void SetActiveTexture(int phase);

   void BufferTexture(int textPhase);
   void BufferAnimation(int animPhase, bool loop = false);

   void HandleMovement(Level* level);
   void MoveTowards(double destX, double destY, int speedType);
   void PlotMovement(Level* level, double destX, double destY);
   void SetCurrentSpeed(int newSpeed);

   virtual void Render(int screenFrame);
   //void Render2(int screenFrame);

   void Free();

   ~Actor();

   int allActorsIndex;
   bool unique;

   float zoom;

   bool xDirection;
   bool yDirection;

   float slowVelocity;
   float avgVelocity;
   float maxVelocity;
   float currentVelocity;
   int currentSpeed;

   bool usingAnims;

   bool hasAnimations;
   bool hasTextures;

   bool animsAllocated;
   bool texturesAllocated;

   int numAnims;
   Animation* anims;
   int activeAnim;

   int numTextures;
   LTexture* textures;
   int activeTexture;

   int bufferedText;
   int bufferedAnim;
   bool loopBufferedAnim;

   int boundingRadius;

   bool walkingPath;
   vector<pair<int,int>> pathNodes;

   // Static
   static void SortRenderingActorVector();
   static void RemoveActor(Actor* actor);
   static vector<RenderedActor*> allActors;
   static vector<RenderedActor*> renderingActors;
};

#endif
