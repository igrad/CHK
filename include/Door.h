#ifndef DOOR_LOADED
#define DOOR_LOADED

#include "essentials.h"
#include "Camera.h"
#include "Actor.h"
#include "Locale.h"
#include "Decal.h"
#include "CollisionDetection.h"

class Door: public Actor {
public:
   Door(int gridX, int gridY, int gridW, int gridH, int direction,
      Locale* locale);

   ~Door();

   void LoadArt();

   void SetLocked(string key);
   void AttemptUnlock(string key);

   void Open(int speed);
   void Close(int speed);
   void Lock();
   void Unlock();

   string id;

   int direction;
   Locale* locale;

   bool isOpen;
   bool isLocked;
   string keyID;
   int difficulty;
};

#endif
