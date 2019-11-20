#ifndef NPCBLUEPRINT_LOADED
#define NPCBLUEPRINT_LOADED

#include "essentials.h"
#include "Animation.h"

class NPC;
class NPCBlueprint;

struct Faction {
   int id;
   string name;
   vector<NPCBlueprint*> NPCBPs;

   vector<Faction*> alliances;
};

struct PatrolNode {
   double holdDuration;
   PatrolNode* next;
};

struct Patrol {
   PatrolNode head;
   PatrolNode tail;
};

enum NPCROLE {GUARD, PATROL, SCOUT};

enum NPCType {
   GOBLIN_GUARD, GOBLIN_SHAMAN
};

struct NPCAnim {
   int frames;
   float duration;
   int frameW;
   int frameH;
   bool reversed;
};

class NPCBlueprint {
public:
   NPCBlueprint();
   NPCBlueprint(NPCROLE role, Faction* faction, int level, string charName,
   string techName, SDL_Rect drawBox, SDL_Rect hitBox,  int maxHP,
   double HPbonus, double HPRegenRate, int maxResource,
   double resourceBonus, double resourceRegenRate, int moveSpeed,
   int numAnims, int numTextures, float density, int packSize,
   bool collides = true);

   void SetAnimBPDefaults(int frames, float duration, int frameW, int frameH);
   void GenerateAnimsAndTextures();

   void Create(vector<NPC>* NPCs);

   ~NPCBlueprint();

   NPCROLE role;
   Faction* faction;

   int level;
   string charName, techName;

   SDL_Rect drawBox, hitBox;

   int maxHP;
   double HPbonus, HPRegenRate;

   int maxResource;
   double resourceBonus, resourceRegenRate;

   int moveSpeed;

   int numAnims, numTextures;

   float density;
   int packSize;

   bool collides;

   NPCAnim* animBPs;

   Animation* anims;
   LTexture* textures;
};

extern NPCBlueprint BP_goblinGuard;
extern NPCBlueprint BP_goblinShaman;

#endif
