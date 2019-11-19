#include "..\include\NPCBlueprint.h"
#include "..\include\NPC.h"

NPC::NPC() {

}

NPC::NPC(string charName, string techName, NPCROLE role, Faction* faction,
   int level, int maxHP, int moveSpeed, int numAnims, int numTextures,
   bool collides): Character(charName, techName, level, maxHP, moveSpeed,
      numAnims, numTextures, collides) {
   this->role = role;
   this->faction = faction;
}

NPC::NPC(NPCBlueprint* bp): Character(bp->charName, bp->techName, bp->level,
   bp->maxHP, bp->moveSpeed, bp->numAnims, bp->numTextures, bp->collides) {
   this->role = bp->role;
   this->faction = bp->faction;
   this->charName = bp->charName;
   this->techName = bp->techName;
   this->HPbonus = bp->HPbonus;
   this->HPRegenRate = bp->HPRegenRate;
   this->maxResource = bp->maxResource;
   this->resourceBonus = bp->resourceBonus;
   this->resourceRegenRate = bp->resourceRegenRate;
}

void NPC::CreatePath() {

}

NPC::~NPC() {
    
}
