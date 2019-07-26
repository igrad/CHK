#ifndef DOOR_LOADED
#define DOOR_LOADED

#include "essentials.h"
#include "Camera.h"
#include "Actor.h"
#include "Locale.h"
#include "Decal.h"
#include "CollisionDetection.h"
#include "MouseEvents.h"

class Door: public Actor, public ClickRegion {
public:
   Door(int gridX, int gridY, int gridW, int gridH, int direction, bool hand,
      int room, Locale* locale);

   void SetHitbox(SDL_Rect* p1, SDL_Rect* p2, SDL_Rect* d);

   // CB functions
   void SetLocked(string key);
   void AttemptUnlock(string key);
   void Open(bool fast);
   void Close(bool fast);
   void Break();
   void Lock();
   void Unlock();
   void Examine();

   void CreateDropMenuOptions();

   void Render();

   ~Door();

   string id;

   int attachedRoom;
   int direction;
   bool hand;
   Locale* locale;

   int phase;
   bool newPhase;
   int frame;

   Collider part1;
   Collider part2;
   Collider door;

   bool isOpen;
   bool hasLock;
   bool isLocked;
   bool isBroken;
   string keyID;
   int difficulty;
};

#endif
