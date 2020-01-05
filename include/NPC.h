#ifndef NPC_LOADED
#define NPC_LOADED

#include "essentials.h"
#include "Character.h"
#include "Camera.h"
#include "UITheme.h"
#include "CollisionDetection.h"
#include "MouseEvents.h"
#include "NPCBlueprint.h"
#include "ActorY.h"

class NPC : public Character {
public:
   NPC();
   NPC(NPCBlueprint* bp);

   void CreatePath();

   ~NPC();

   // NPC's faction
   Faction* faction;

   // Role of the NPC
   NPCROLE role;

   // Patrol path, if applicable
   Patrol patrol;
   PatrolNode* currentPatrolNode;
};

#endif
