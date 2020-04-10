#include "..\include\NPCBlueprint.h"
#include "..\include\NPC.h"

NPC::NPC() {

}

NPC::NPC(NPCBlueprint* bp): Character(bp->charName, bp->techName, bp->level,
   bp->maxHP, bp->moveSpeed, bp->numAnims, bp->numTextures, bp->collides, 
   bp->unique) {
   this->drawBox = bp->drawBox;
   this->hitBox = bp->hitBox;
   this->role = bp->role;
   this->faction = bp->faction;
   this->charName = bp->charName;
   this->techName = bp->techName;
   this->HPbonus = bp->HPbonus;
   this->HPRegenRate = bp->HPRegenRate;
   this->maxResource = bp->maxResource;
   this->resourceBonus = bp->resourceBonus;
   this->resourceRegenRate = bp->resourceRegenRate;

   anims = bp->anims;
   textures = bp->textures;
}

void NPC::CreatePath() {

}

NPC::~NPC() {

}
