#include "..\include\Door.h"

Door::Door(int gridX, int gridY, int gridW, int gridH, int direction, bool hand,
   int room, Locale* locale): Actor(6, 4) {
   char buf[6];
   snprintf(buf, 6, "%2i%2i", gridX, gridY);
   id = buf;

   this->direction = direction;
   this->hand = hand;
   attachedRoom = room;
   this->locale = locale;


   int tileW = PIXELSPERFEET * 5 * GZOOM;
   xPos = gridX * tileW;
   yPos = gridY * tileW;
   if (direction != NORTH) yPos -= (locale->wallHeight * GZOOM);
   else yPos -= ((locale->wallHeight * GZOOM) - tileW);

   int drawW = gridW * tileW;
   int drawH = (gridH * tileW) + (locale->wallHeight * GZOOM);
   drawBox = {(int)xPos, (int)yPos, drawW, drawH};
   drawCutoff = gridY * tileW;
   if (direction == EAST || direction == WEST) drawCutoff += tileW;

   isOpen = false;
   isLocked = false;
   keyID = "";
   difficulty = 0;

   phase = TEXT_CLOSED;
   newPhase = false;
   frame = 0;
   crType = CR_RELATIVE;

   // Load the art assets and collision rects for this door
   switch (direction) {
      case NORTH:
         if (hand) { // Right-handed door
            // Load the colliders
            part1.hitBox = locale->doorNS_right_inside_part1Clip;
            part2.hitBox = locale->doorNS_right_inside_part2Clip;
            door.hitBox = locale->doorNS_right_inside_doorClip;

            // Door textures
            LoadTexture(0, &locale->doorNS_right_inside_closed);
            LoadTexture(1, &locale->doorNS_right_inside_open);

            // Door animations
            // Open slowly
            LoadAnimation(0, &locale->doorNS_right_inside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH);
            // Open fast
            LoadAnimation(1, &locale->doorNS_right_inside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            // Close slowly
            LoadAnimation(2, &locale->doorNS_right_inside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH, true);
               // Reversed animation because it's closing
            // Close fast
            LoadAnimation(3, &locale->doorNS_right_inside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH, true);
            // Break outward
            LoadAnimation(4, &locale->doorNS_right_inside_break_out_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            // Break inward
            LoadAnimation(5, &locale->doorNS_right_inside_break_in_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
         } else { // Left-handed door
            part1.hitBox = locale->doorNS_left_inside_part1Clip;
            part2.hitBox = locale->doorNS_left_inside_part2Clip;
            door.hitBox = locale->doorNS_left_inside_doorClip;

            LoadTexture(0, &locale->doorNS_left_inside_open);
            LoadTexture(1, &locale->doorNS_left_inside_closed);

            LoadAnimation(0, &locale->doorNS_left_inside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorNS_left_inside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorNS_left_inside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorNS_left_inside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorNS_left_inside_break_out_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorNS_left_inside_break_in_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
         }
         break;
      case EAST:
         if (hand) {
            part1.hitBox = locale->doorE_right_part1Clip;
            part2.hitBox = locale->doorE_right_part2Clip;
            door.hitBox = locale->doorE_right_doorClip;

            LoadTexture(0, &locale->doorE_right_open);
            LoadTexture(1, &locale->doorE_right_closed);

            LoadAnimation(0, &locale->doorE_right_open_anim,
               8, locale->doorEChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorE_right_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorE_right_open_anim,
               8, locale->doorEChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorE_right_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorE_right_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorE_right_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH);
         } else {
            part1.hitBox = locale->doorE_left_part1Clip;
            part2.hitBox = locale->doorE_left_part2Clip;
            door.hitBox = locale->doorE_left_doorClip;

            LoadTexture(0, &locale->doorE_left_open);
            LoadTexture(1, &locale->doorE_left_closed);

            LoadAnimation(0, &locale->doorE_left_open_anim,
               8, locale->doorEChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorE_left_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorE_left_open_anim,
               8, locale->doorEChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorE_left_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorE_left_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorE_left_open_anim,
               8, locale->doorEChangeRate_fast, drawW, drawH);
         }
         break;
      case SOUTH:
         if (hand) {
            part1.hitBox = locale->doorNS_right_outside_part1Clip;
            part2.hitBox = locale->doorNS_right_outside_part2Clip;
            door.hitBox = locale->doorNS_right_outside_doorClip;

            LoadTexture(0, &locale->doorNS_right_outside_open);
            LoadTexture(1, &locale->doorNS_right_outside_closed);

            LoadAnimation(0, &locale->doorNS_right_outside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorNS_right_outside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorNS_right_outside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorNS_right_outside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorNS_right_outside_break_out_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorNS_right_outside_break_in_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
         } else {
            part1.hitBox = locale->doorNS_left_outside_part1Clip;
            part2.hitBox = locale->doorNS_left_outside_part2Clip;
            door.hitBox = locale->doorNS_left_outside_doorClip;

            LoadTexture(0, &locale->doorNS_left_outside_open);
            LoadTexture(1, &locale->doorNS_left_outside_closed);

            LoadAnimation(0, &locale->doorNS_left_outside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorNS_left_outside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorNS_left_outside_open_anim,
               8, locale->doorNSChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorNS_left_outside_open_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorNS_left_outside_break_out_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorNS_left_outside_break_in_anim,
               8, locale->doorNSChangeRate_fast, drawW, drawH);
         }
         break;
      case WEST:
         if (hand) {
            part1.hitBox = locale->doorW_right_part1Clip;
            part2.hitBox = locale->doorW_right_part2Clip;
            door.hitBox = locale->doorW_right_doorClip;

            LoadTexture(0, &locale->doorW_right_open);
            LoadTexture(1, &locale->doorW_right_closed);

            LoadAnimation(0, &locale->doorW_right_open_anim,
               8, locale->doorWChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorW_right_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorW_right_open_anim,
               8, locale->doorWChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorW_right_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorW_right_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorW_right_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH);
         } else {
            part1.hitBox = locale->doorW_left_part1Clip;
            part2.hitBox = locale->doorW_left_part2Clip;
            door.hitBox = locale->doorW_left_doorClip;

            LoadTexture(0, &locale->doorW_left_open);
            LoadTexture(1, &locale->doorW_left_closed);

            LoadAnimation(0, &locale->doorW_left_open_anim,
               8, locale->doorWChangeRate_slow, drawW, drawH);
            LoadAnimation(1, &locale->doorW_left_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH);
            LoadAnimation(2, &locale->doorW_left_open_anim,
               8, locale->doorWChangeRate_slow, drawW, drawH, true);
            LoadAnimation(3, &locale->doorW_left_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH, true);
            LoadAnimation(4, &locale->doorW_left_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH);
            LoadAnimation(5, &locale->doorW_left_open_anim,
               8, locale->doorWChangeRate_fast, drawW, drawH);
         }
         break;
   }

   part1.hitBox.x = part1.hitBox.x * GZOOM;
   part1.hitBox.y = part1.hitBox.y * GZOOM;
   part1.hitBox.w = part1.hitBox.w * GZOOM;
   part1.hitBox.h = part1.hitBox.h * GZOOM;
   part2.hitBox.x = part2.hitBox.x * GZOOM;
   part2.hitBox.y = part2.hitBox.y * GZOOM;
   part2.hitBox.w = part2.hitBox.w * GZOOM;
   part2.hitBox.h = part2.hitBox.h * GZOOM;
   door.hitBox.x = door.hitBox.x * GZOOM;
   door.hitBox.y = door.hitBox.y * GZOOM;
   door.hitBox.w = door.hitBox.w * GZOOM;
   door.hitBox.h = door.hitBox.h * GZOOM;

   part1.hitBox.x += (gridX * tileW);
   part1.hitBox.y += (gridY * tileW);
   part2.hitBox.x += (gridX * tileW);
   part2.hitBox.y += (gridY * tileW);
   door.hitBox.x += (gridX * tileW);
   door.hitBox.y += (gridY * tileW);

   // Set the mouseHandler parameters
   clickRect = {
      gridX * tileW,
      gridY * tileW,
      tileW,
      tileW
   };

   dropmenu = new DropMenu(0, 0, 1, 5, CR_ABSOLUTE, &UI.doorDM);
   linkedDM = dropmenu;
}



void Door::SetHitbox(SDL_Rect* p1, SDL_Rect* p2, SDL_Rect* d) {
   // We'll use this in the future for creating doors that don't perfectly fit
   // our default door structure, such as boss door entrances
   part1.SetHitBox(p1->x, p1->y, p1->w, p1->h);
   part2.SetHitBox(p2->x, p2->y, p2->w, p2->h);
   door.SetHitBox(d->x, d->y, d->w, d->h);
}



void Door::SetLocked(string key) {
   keyID = key;
   isLocked = true;
}



void Door::AttemptUnlock(string key) {
   if (key == keyID) Unlock();
}



void Door::AttemptOpen(bool fast) {
   //if (isLocked) return; // Inform the player that the door is locked!
   //else
   Log("Attempting to open");
   this->Open(fast);
}



void Door::Open(bool fast) {
   Log("Opening!");
   if (!isOpen) {
      isOpen = true;
      isLocked = false;

      hitBox.w = 0;
      hitBox.h = 0;

      // Queue animation of door opening
      newPhase = true;
      if (phase == ANIM_OPEN_FAST || phase == ANIM_OPEN_SLOW) {
         newPhase = false;
         frame = anims[phase].frameCount - frame;
      }

      if (fast) phase = ANIM_CLOSE_FAST;
      else phase = ANIM_CLOSE_SLOW;
   }
}



void Door::Close(bool fast) {
   if (isOpen) {
      isOpen = false;
      hitBox = drawBox;

      // Queue animation of door closing
      newPhase = true;
      if (phase == ANIM_CLOSE_FAST || phase == ANIM_CLOSE_SLOW) {
         newPhase = false;
         frame = anims[phase].frameCount - frame;
      }

      if (fast) phase = ANIM_OPEN_FAST;
      else phase = ANIM_OPEN_SLOW;
   }
}



void Door::Break() {
   // Do the breaking thing
}



void Door::Lock() {
   if (!isLocked) {
      isLocked = true;
   }
}



void Door::Unlock() {
   if (isLocked) {
      isLocked = false;
   }
}



void Door::Examine() {
   // Describe if the door is broken, unlocked, has a lock, etc.
}



void Door::OpenDropMenu() {
   Log("Opening door drop menu");

   dropmenu->ClearButtons();
   bool canBreak = !isOpen && !isBroken;

   string op = isOpen ? "Open " : "Close ";

   dropmenu->AddButton(op + "slowly", LEFTCLICK, [this](int a, int b, int c){
      this->AttemptOpen(false);
      return false;
   });

   dropmenu->AddButton(op + "fast", LEFTCLICK, [this](int a, int b, int c){
      this->AttemptOpen(true);
      return false;
   });

   if (canBreak) {
      dropmenu->AddButton("Break down", LEFTCLICK, [this](int a, int b, int c){
         //this->AttemptBreak(true);
         return false;
      });
   }
   if (hasLock) {
      if (isLocked) {
         dropmenu->AddButton("Unlock", LEFTCLICK,
         [this](int a, int b, int c){
            // We technically need to show the unlock attempt UI here
            //this->AttemptUnlock(true);
            return false;
         });
      }
      else {
         dropmenu->AddButton("Lock", LEFTCLICK, [this](int a, int b, int c){
            //this->Lock(true);
            return false;
         });
      }
   }
   dropmenu->AddButton("Examine", LEFTCLICK, [this](int a, int b, int c){
      // this->AttemptOpen(true);
      return false;
   });

   dropmenu->Open();
}



void Door::Render() {
   if (newPhase) {
      frame = 0;
      newPhase = false;
   }

   Actor::Render(frame, Camera::x, Camera::y);
   if (frame >= anims[activeAnim].frameCount) {
      if (activeAnim == ANIM_OPEN_SLOW || activeAnim == ANIM_OPEN_FAST) {
         activeAnim = TEXT_OPEN;
         newPhase = true;
      } else if (activeAnim == ANIM_CLOSE_SLOW ||
         activeAnim == ANIM_CLOSE_FAST) {
         activeAnim = TEXT_CLOSE;
         newPhase = true;
      }
   }
}



Door::~Door() {
   delete locale;
}
