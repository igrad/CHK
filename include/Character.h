#ifndef CHARACTER_LOADED
#define CHARACTER_LOADED

#include "essentials.h"

#include "Actor.h"

extern int statCap;

enum STATNAME {STR, DEX, CON, INT, WIS, CHA};

struct Resource {
   string name;
   int base;

};

class Character: public Actor {
   public:
      Character(int level, int HP, int moveSpeed, int numAnims, int numTextures, bool collides = true);

      int GetStat(STATNAME stat);

      bool SetStat(STATNAME stat, int newVal, bool ignoreStatCap = false);

      bool SetStats(int STR, int DEX, int CON, int INT, int WIS, int CHA);

      void SetHPAndResourceToFull();


      void SetSpawnPoint(SDL_Rect r);

      void SetPosition(int newX, int newY);

      void MoveTowards(int destX, int destY);

      void SetXVelocity(int multiplier);

      void SetYVelocity(int multiplier);


      void Render(int screenFrame, int camX, int camY);


      void Free();

      ~Character();

      // Attributes
      string charName;

      int charLevel;

      bool collisionEnabled;

      int moveSpeed;

      bool xDirection;
      bool yDirection;

      int currentHP;
      int maxHP;
      float HPRegenRate;

      int currentResource;
      int maxResource;
      int ResourceBonus;
      float ResourceRegenRate;

      float xVelocity;
      float yVelocity;

      float pixelVelocity;
      float pixelsPerFrame;

      // An array of ints to represent character stats
      int* stats;
};

#endif
