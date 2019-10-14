#include "..\include\Character.h"

int statCap = 8;

Character::Character(int level, int HP, int moveSpeed, int numAnims,
   int numTextures, bool collides): Actor(numAnims, numTextures) {
   charLevel = level;
   collisionEnabled = collides;
   this->moveSpeed = moveSpeed;
   this->maxHP = HP;

   // The number of feet times the pixels per feet yeids the pixels moved per second
   pixelVelocity = moveSpeed * PIXELSPERFEET * GZOOM;
   pixelsPerFrame = pixelVelocity / SCREEN_FPS;

   stats = new int[6];
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

void Character::SetPosition(int newX, int newY) {
   xPos = newX;
   yPos = newY;
}

void Character::MoveTowards(int destX, int destY) {
   float vX = destX - xPos;
   float vY = destY - yPos;

   float vMag = FindDistance(xPos, yPos, destX, destY);

   float uX = vX/vMag;
   float uY = vY/vMag;

   int newX = uX * pixelsPerFrame;
   int newY = uY * pixelsPerFrame;

   SetPosition(newX, newY);
}

void Character::SetXVelocity(int multiplier) {
   xVelocity = pixelsPerFrame * multiplier;
   xVelocity = (multiplier < 1) ? ceil(xVelocity) : floor (xVelocity);

   queueCollisions = true;
}

void Character::SetYVelocity(int multiplier) {
   yVelocity = pixelsPerFrame * multiplier;
   yVelocity = (multiplier < 1) ? ceil(yVelocity) : floor (yVelocity);

   queueCollisions = true;
}

void Character::Render(int screenFrame, int camX, int camY) {
   // Player animations
   bool movingUp = (yVelocity < 0);
   bool movingDown = (yVelocity > 0);
   bool movingLeft = (xVelocity < 0);
   bool movingRight = (xVelocity > 0);
   PHASE_CHARACTER phase = ANIM_IDLE_I;
   if (xDirection) {
      if (movingUp) { phase = ANIM_WALK_I; }
      else if (movingDown) { phase = ANIM_WALK_IV; }
      else if (movingRight) {
         if (yDirection) { phase = ANIM_WALK_IV; }
         else { phase = ANIM_WALK_I; }
      } else {
         if (yDirection) { phase = ANIM_IDLE_IV; }
         else { phase = ANIM_IDLE_I; }
      }
   } else {
      if (movingUp) { phase = ANIM_WALK_II; }
      else if (movingDown) { phase = ANIM_WALK_III; }
      else if (movingLeft) {
         if (yDirection) { phase = ANIM_WALK_III; }
         else { phase = ANIM_WALK_II; }
      } else {
         if (yDirection) { phase = ANIM_IDLE_III; }
         else { phase = ANIM_IDLE_II; }
      }
   }

   if (activeAnim != phase) { SetActiveAnim(phase, true); }

   xVelocity = 0;
   yVelocity = 0;

   Actor::Render(screenFrame, camX, camY);
}

void Character::Free() {
   delete stats;
}

Character::~Character() {
   delete[] stats;
}
