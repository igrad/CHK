#include "..\include\NPCBlueprint.h"
#include "..\include\NPC.h"

NPCBlueprint::NPCBlueprint() {
   anims = NULL;
   animBPs = NULL;
   textures = NULL;
}

NPCBlueprint::NPCBlueprint(NPCROLE role, Faction* faction, int level,
   string charName, string techName, SDL_Rect drawBox, SDL_Rect hitBox,
   int maxHP, double HPbonus, double HPRegenRate, int maxResource,
   double resourceBonus, double resourceRegenRate, int moveSpeed,
   int numAnims, int numTextures, float density, int packSize, bool collides) {
   this->role = role;
   this->faction = faction;
   this->level = level;
   this->drawBox = {
      int(drawBox.x * GZOOM),
      int(drawBox.y * GZOOM),
      int(drawBox.w * GZOOM),
      int(drawBox.h * GZOOM)
   };
   this->hitBox = {
      int(hitBox.x * GZOOM),
      int(hitBox.y * GZOOM),
      int(hitBox.w * GZOOM),
      int(hitBox.h * GZOOM)
   };
   this->charName = charName;
   this->techName = techName;
   this->maxHP = maxHP;
   this->HPbonus = HPbonus;
   this->HPRegenRate = HPRegenRate;
   this->maxResource = maxResource;
   this->resourceBonus = resourceBonus;
   this->resourceRegenRate = resourceRegenRate;
   this->moveSpeed = moveSpeed;
   this->numAnims = numAnims;
   this->numTextures = numTextures;
   this->density = density;
   this->packSize = packSize;
   this->collides = collides;

   animBPs = new NPCAnim[numAnims];
   anims = new Animation[numAnims];
   textures = new LTexture[numTextures];

   faction->NPCBPs.push_back(this);
}

NPCBlueprint::~NPCBlueprint() {
   if (animBPs != NULL) delete[] animBPs;
   if (anims != NULL) delete[] anims;
   if (textures != NULL) delete[] textures;
}

void NPCBlueprint::SetAnimBPDefaults(int frames, float duration,
   int frameW, int frameH) {
   for (int iter = 0; iter < numAnims; iter++) {
      animBPs[iter].frames = frames;
      animBPs[iter].duration = duration;
      animBPs[iter].frameW = frameW;
      animBPs[iter].frameH = frameH;
   }
}

void NPCBlueprint::GenerateAnimsAndTextures() {
   string prefix = "media\\images\\Characters\\" + techName + "_";

   for (int iter = 0; iter < numAnims; iter++) {
      NPCAnim animBP;
      if (animBPs != NULL) animBP = animBPs[iter];

      anims[iter].LoadFromFile(prefix + to_string(iter) + ".png",
         animBP.frames, animBP.duration, animBP.frameW, animBP.frameH,
         animBP.reversed);
   }

   for (int iter = 0; iter < numTextures; iter++) {
      textures[iter].LoadFromFile(prefix + to_string(iter) + ".png");
   }
}

void NPCBlueprint::Create(vector<NPC>* NPCs) {
   NPCs->push_back(NPC(this));
}

// Factions
Faction F_goblins {1, "Goblins"};

// NPC Blueprints
SDL_Rect drawBox = {0, 0, 26, 40};
SDL_Rect hitBox = {8, 0, 10, 6};
NPCBlueprint BP_goblinGuard(
   GUARD, &F_goblins,   // Role and faction
   1,                   // Level
   "Goblin Guard",      // On-screen name
   "goblinGuard",       // Technical name
   drawBox, hitBox,     // drawBox, hitBox
   10, 5.00, 0.40,      // HP, HP bonus, and HP regen
   0, 0.00, 0.00,       // Resource, resource bonus, and resource regen
   25,                  // Movespeed
   8, 1,                // # animations, # textures
   0.03, 3              // Frequency (% chance to spawn), pack size
);

NPCBlueprint BP_goblinShaman(GUARD, &F_goblins, 1,
   "Goblin Shaman", "goblinShaman", drawBox, hitBox, 8, 4.00, 0.40, 6, 6.00,
   0.60, 25, 8, 1, 0.01, 1);
