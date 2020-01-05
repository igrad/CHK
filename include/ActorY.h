#ifndef ACTORY_DEFINED
#define ACTORY_DEFINED

#include "essentials.h"

class Actor;
class Character;
class Door;
class NPC;

// ActorY is used for collision, lighting, rendering... everything that
// relies on the actor's Y coordinate in the map
class ActorY {
public:
   ActorY();

   void operator=(ActorY a);

   void NewActor(Actor* a);
   void NewCharacter(Character* c);
   void NewDoor(Door* d);

   void UpdateY();

   int GetYBase();

   void Render(int screenFrame);

   int y;
	Actor* actor;
   Character* character;
   Door* door;

   // Static components
   static vector<ActorY> allActors;

   static void PushActor(ActorY actor);
   static void PopActor(Actor* actor);
   static void PopActor(int index);
   static void UpdateActor(Actor* actor);
   static void UpdateActor(int index);
};



#endif