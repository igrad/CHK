#include "..\include\Character.h"

int statCap = 8;

Character::Character() {
}

Character::Character(string charName, string techName, int level, int HP,
   int moveSpeed, int numAnims, int numTextures, bool collides, bool unique):
   Actor(numAnims, numTextures, true, unique) {
   charLevel = level;
   collisionEnabled = collides;
   this->moveSpeed = moveSpeed;
   this->maxHP = HP;

   // The number of feet times the pixels per feet yeids the pixels moved per second
   pixelVelocity = moveSpeed * PIXELSPERFEET * GZOOM;
   pixelsPerFrame = pixelVelocity / SCREEN_FPS;
   slowVelocity = pixelsPerFrame/2;
   avgVelocity = pixelsPerFrame;
   maxVelocity = pixelsPerFrame * 2;

   stats = new int[6];
}

void Character::LoadDefaultArt(string name) {
   string str = "media\\images\\Characters\\" + name + "_";

   LoadAnimation(ANIM_IDLE_I, str + "Idle_I.png", 8, 2.0, 20, 40);
	LoadAnimation(ANIM_IDLE_II, str + "Idle_II.png", 8, 2.0, 20, 40);
	LoadAnimation(ANIM_IDLE_III, str + "Idle_III.png", 8, 2.0, 20, 40);
	LoadAnimation(ANIM_IDLE_IV, str + "Idle_IV.png", 8, 2.0, 20, 40);
	LoadAnimation(ANIM_WALK_I, str + "Walking_I.png", 8, 1.2, 20, 40);
	LoadAnimation(ANIM_WALK_II, str + "Walking_II.png", 8, 1.2, 20, 40);
	LoadAnimation(ANIM_WALK_III, str + "Walking_III.png", 8, 1.2, 20, 40);
	LoadAnimation(ANIM_WALK_IV, str + "Walking_IV.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_JUMP, str + "ANIM_JUMP.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_KICK, str + "ANIM_KICK.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_ATTACK_1, str + "ANIM_ATTACK_1.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_ATTACK_2, str + "ANIM_ATTACK_2.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_ATTACK_3, str + "ANIM_ATTACK_3.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_ABILITY_1, str + "ANIM_ABILITY_1.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_ABILITY_2, str + "ANIM_ABILITY_2.png", 8, 1.2, 20, 40);
	// LoadAnimation(ANIM_ABILITY_3, str + "ANIM_ABILITY_3.png", 8, 1.2, 20, 40);
}

int Character::GetStat(STATNAME stat) {
   return stats[stat];
}

bool Character::SetStat(STATNAME stat, int newVal, bool ignoreStatCap) {
   if ((newVal > statCap) && !ignoreStatCap) {
      Warn("Could not increase stat above the statCap!");
      return false;
   } else if (newVal < 0) {
      Warn("Could not decrease stat below 0!");
      return false;
   } else {
      stats[stat] = newVal;
      return true;
   }
}

bool Character::SetStats(int strength, int dexterity, int constitution,
   int intellect, int wisdom, int charisma) {
   bool success = true;

   success = (SetStat(STR, strength) && success) ? true : false;
   success = (SetStat(DEX, dexterity) && success) ? true : false;
   success = (SetStat(CON, constitution) && success) ? true : false;
   success = (SetStat(INT, intellect) && success) ? true : false;
   success = (SetStat(WIS, wisdom) && success) ? true : false;
   success = (SetStat(CHA, charisma) && success) ? true : false;

   return success;
}

void Character::SetHPAndResourceToFull() {
   currentHP = maxHP;
   currentResource = maxResource;
}

void Character::SetSpawnPoint(SDL_Rect r) {
   int x = r.x - (drawBox.w/2);
   int y = r.y - drawBox.h + (PIXELSPERFEET * 2.5);
   SetPosition(x, y);
}

void Character::SetPosition(double newX, double newY) {
   xPos = newX;
   yPos = newY;
}

void Character::Render(int screenFrame) {
   // Player animations
   PHASE_CHARACTER phase = ANIM_IDLE_I;

   if (xDirection) {
      if (walkingPath) {
         if (!yDirection) phase = ANIM_WALK_I;
         else phase = ANIM_WALK_IV;
      } else {
         if (!yDirection) phase = ANIM_IDLE_I;
         else phase = ANIM_IDLE_IV;
      }
   } else {
      if (walkingPath) {
         if (!yDirection) phase = ANIM_WALK_II;
         else phase = ANIM_WALK_III;
      } else {
         if (!yDirection) phase = ANIM_IDLE_II;
         else phase = ANIM_IDLE_III;
      }
   }

   if (activeAnim != phase) SetActiveAnim(phase, true);

   Actor::Render(screenFrame);

   // Render the path being walked
   // if (walkingPath) {
   //    for (int point = 0; point < pathNodes.size(); point++) {
   //       const SDL_Rect r = {
   //          (int)(pathNodes[point].first - Camera::x) - 3,
   //          (int)(pathNodes[point].second - Camera::y) - 3,
   //          7,
   //          7};
   //       SDL_RenderFillRect(gRenderer, &r);
   //    }
   // }
}

void Character::Free() {
   delete[] stats;
}

Character::~Character() {
   Free();
}
