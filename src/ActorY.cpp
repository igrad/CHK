#include "..\include\ActorY.h"

#include "..\include\Actor.h"
#include "..\include\Door.h"
#include "..\include\Character.h"
#include "..\include\NPC.h"

vector<ActorY> ActorY::allActors = {};

ActorY::ActorY() {
   y = 0;

   actor = NULL;
   character = NULL;
   door = NULL;
}

void ActorY::operator=(ActorY a) {
   y = a.y;
   actor = a.actor;
   character = a.character;
   door = a.door;
}

void ActorY::NewActor(Actor* a) {
   y = (int)(a->yPos);

   actor = a;
   character = NULL;
   door = NULL;
}

void ActorY::NewCharacter(Character* c) {
   y = (int)(c->yPos);

   actor = c;
   character = c;
   door = NULL;

   Log("Instantiated new ActorY with Character");
   printf("\nCharacter index: %i", allActors.size());
   printf("\nCharacter address: %p", c);
   printf("\nC and A are equal: %i", (character == actor));
}

void ActorY::NewDoor(Door* d) {
   y = (int)(d->yPos);

   actor = d;
   character = NULL;
   door = d;
}

void ActorY::UpdateY() {
   y = (int)(actor->yPos);
}

int ActorY::GetYBase() {
   return actor->hitBox.y + actor->hitBox.h;
}

void ActorY::Render(int screenFrame) {
   printf("\n\ny: %i\nactor: %p\ncharacter: %p\ndoor: %p", y, actor, character, door);
   if (character != NULL) {
      character->Character::Render(screenFrame);
      Log("Calling render on char");
   }
   else if (door != NULL) door->Render();
   else if (actor != NULL) {
      // Log("Calling render on actor");
      actor->Render(screenFrame);
   }
}

void ActorY::PushActor(ActorY ay) {
   Actor* actor = ay.actor;

   if (allActors.size() == 0) {
      actor->allActorsIndex = allActors.size();
      allActors.push_back(ay);
   } else {
      // Try to insert before an element
      int placement = 0;
      for (int iter = 0; iter < allActors.size(); iter++) {
         if (ay.y < allActors[iter].y) {
            actor->allActorsIndex = iter;
            allActors.insert(allActors.begin() + iter, ay);
            placement = iter;
            break;
         }
      }

      // If it wasn't successfully placed, we just put it at the end
      if (placement == 0) {
         actor->allActorsIndex = allActors.size();
         allActors.push_back(ay);
      } else {
         // Item WAS placed - update the indices of all members
         for (int iter = placement + 1; iter < allActors.size(); iter++) {
            allActors[iter].actor->allActorsIndex = iter;
         }
      }
   }
}

void ActorY::PopActor(Actor* actor) {
   int i = actor->allActorsIndex;
   if (allActors.size() > i) allActors.erase(allActors.begin() + i);

   if (allActors.size() >= i) {
      for (int iter = 0; iter < allActors.size(); iter++) {
         allActors[iter].actor->allActorsIndex = iter;
      }
   }
}

void ActorY::PopActor(int index) {
   if (allActors.size() > index) {
      allActors.erase(allActors.begin() + index);
   }

   if (allActors.size() >= index) {
      for (int iter = 0; iter < allActors.size(); iter++) {
         allActors[iter].actor->allActorsIndex = iter;
      }
   }
}

void ActorY::UpdateActor(Actor* actor) {
   int index = actor->allActorsIndex;
   int oldY = allActors[index].y;
   int newY = actor->yPos;

   // If the actor isn't changing its Y position, we don't need to do anything
   if (oldY == newY) return;

   // Update the y value
   allActors[index].y = newY;

   // If the vector is only 1 element long, we're done
   if (allActors.size() == 1) return;

   // Copy this actor from the vector so that we can overwrite the real item
   // still in the vector
   ActorY thisAY = allActors[index];

   bool sorted = false;
   int nextIndex = index + 1;
   int prevIndex = index - 1;
   while (!sorted) {
      
      // We're trying to sort this vector from small y to large y
      // If the new Y val is larger or equal to the previous index's Y val,
      // then there's a good chance that this item belongs here

      // We use these bools here so that we don't have to access the memory 
      // multiple times while going through these tedious checks
      bool prevGood = (index <= 0) ? false : 
         newY >= allActors[prevIndex].y;
      bool nextGood = (index >= allActors.size() - 1) ? false : 
         newY < allActors[nextIndex].y;

      // printf("\nSorting: index %i prevGood %i nextGood %i", index, prevGood, nextGood);

      if (index == 0 && nextGood ||                   // Belongs at head
          index == allActors.size() - 1 && prevGood ||  // Belongs at tail
          prevGood && nextGood) {                       // Belongs right here
         // Drop the element right here
         allActors[index] = thisAY;
         actor->allActorsIndex = index;
         sorted = true;
      } else if (prevGood && !nextGood) {
         // The previous element is smaller than the current element, which is
         // what we want. However, the next element is also smaller or equal,
         // so we need to move this item toward the back
         // Log("Moving back");
         allActors[nextIndex].actor->allActorsIndex--;
         allActors[index] = allActors[nextIndex];

         index++;
         nextIndex++;
         prevIndex++;
      } else if (nextGood && !prevGood) {
         // The next element is bigger than the current element, which is what
         // we want. However, the previous element is also bigger, so we need to
         // move this item toward the front
         // Log("Moving to front");
         // printf("\n%i %i %i", allActors.size(), prevIndex, index);
         allActors[prevIndex].actor->allActorsIndex++;
         allActors[index] = allActors[prevIndex];

         index--;
         nextIndex--;
         prevIndex--;
      } else {
         // Unhandled case: slap it in the array and see what happens
         allActors[index] = thisAY;
         actor->allActorsIndex = index;
         sorted = true;
      }
   }

   // Log("Exiting UpdateActor");
}

void ActorY::UpdateActor(int index) {
   UpdateActor(allActors[index].actor);
}
