#ifndef CHARACTER_LOADED
#define CHARACTER_LOADED

#include "essentials.h"

#include "Camera.h"
#include "Actor.h"

extern int statCap;

enum STATNAME {STR, DEX, CON, INT, WIS, CHA};

struct Resource {
   string name;
   int base;
};

class Character: public Actor {
   public:
      Character();
      Character(string charName, string techName, int level, int maxHP,
         int moveSpeed, int numAnims, int numTextures, bool collides = true);

      void LoadDefaultArt(string name);
      int GetStat(STATNAME stat);
      bool SetStat(STATNAME stat, int newVal, bool ignoreStatCap = false);
      bool SetStats(int STR, int DEX, int CON, int INT, int WIS, int CHA);
      void SetHPAndResourceToFull();

      void SetSpawnPoint(SDL_Rect r);
      void SetPosition(int newX, int newY);
      void MoveTowards(int destX, int destY);
      void SetXVelocity(int multiplier);
      void SetYVelocity(int multiplier);

      void Render(int screenFrame);

      void Free();

      ~Character();

      // Attributes
      string charName;
      string techName;

      int charLevel;

      bool collisionEnabled;

      int moveSpeed;

      int currentHP;
      int maxHP;
      double HPbonus;
      double HPRegenRate;

      int currentResource;
      int maxResource;
      double resourceBonus;
      double resourceRegenRate;

      float pixelVelocity;
      float pixelsPerFrame;

      // An array of ints to represent character stats
      int* stats;
};

#endif
