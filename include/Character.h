#ifndef CHARACTER_LOADED
#define CHARACTER_LOADED

#include "essentials.h"

#include "Camera.h"
#include "Actor.h"
#include "Level.h"

extern int statCap;

enum STATNAME {STR, DEX, CON, INT, WIS, CHA};

struct Resource {
   string name;
   int base;
};

class Character: public Actor, virtual public RenderedActor {
   public:
      Character();
      Character(string charName, string techName, int level, int maxHP,
         int moveSpeed, int numAnims, int numTextures, bool collides = true, 
         bool unique = true);

      void LoadDefaultArt(string name);
      int GetStat(STATNAME stat);
      bool SetStat(STATNAME stat, int newVal, bool ignoreStatCap = false);
      bool SetStats(int STR, int DEX, int CON, int INT, int WIS, int CHA);
      void SetHPAndResourceToFull();

      void SetSpawnPoint(SDL_Rect r);
      void SetPosition(double newX, double newY);

      void Render(int screenFrame) override;

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

      // Movement pathing data
      bool pathing;
      vector<pair<int, int>> pathPoints;
};

#endif
