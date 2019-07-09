#include "..\include\Door.h"

Door::Door(int gridX, int gridY, int gridW, int gridH, int direction, bool hand,
   Locale* locale): Actor(6, 4) {
   char buf[6];
   snprintf(buf, 6, "%2i%2i", gridX, gridY);
   id = buf;

   this->direction = direction;
   this->hand = hand;
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

   // Load the art assets and collision rects for this door
   switch (direction) {
      case 0:
         if (hand) { // Right-handed door
            // Load the colliders
            part1.hitBox = locale->doorNS_right_inside_part1Clip;
            part2.hitBox = locale->doorNS_right_inside_part2Clip;
            door.hitBox = locale->doorNS_right_inside_doorClip;

            // Door textures
            LoadTexture(0, &locale->doorNS_right_inside_open);
            LoadTexture(1, &locale->doorNS_right_inside_closed);

            // Door animations
            // Open slowly
            LoadAnimation(0, &locale->doorNS_right_inside_open_anim,
               8, 1.5, drawW, drawH);
            // Open fast
            LoadAnimation(1, &locale->doorNS_right_inside_open_anim,
               8, 0.5, drawW, drawH);
            // Close slowly
            LoadAnimation(2, &locale->doorNS_right_inside_open_anim,
               8, 1.5, drawW, drawH, true);
               // Reversed animation because it's closing
            // Close fast
            LoadAnimation(3, &locale->doorNS_right_inside_open_anim,
               8, 0.5, drawW, drawH, true);
            // Break outward
            LoadAnimation(4, &locale->doorNS_right_inside_break_out_anim,
               8, 1.0, drawW, drawH);
            // Break inward
            LoadAnimation(5, &locale->doorNS_right_inside_break_in_anim,
               8, 1.0, drawW, drawH);
         } else { // Left-handed door
            part1.hitBox = locale->doorNS_left_inside_part1Clip;
            part2.hitBox = locale->doorNS_left_inside_part2Clip;
            door.hitBox = locale->doorNS_left_inside_doorClip;

            LoadTexture(0, &locale->doorNS_left_inside_open);
            LoadTexture(1, &locale->doorNS_left_inside_closed);

            LoadAnimation(0, &locale->doorNS_left_inside_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorNS_left_inside_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorNS_left_inside_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorNS_left_inside_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorNS_left_inside_break_out_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorNS_left_inside_break_in_anim,
               8, 1.0, drawW, drawH);
         }
         break;
      case 1:
         if (hand) {
            part1.hitBox = locale->doorE_right_part1Clip;
            part2.hitBox = locale->doorE_right_part2Clip;
            door.hitBox = locale->doorE_right_doorClip;

            LoadTexture(0, &locale->doorE_right_open);
            LoadTexture(1, &locale->doorE_right_closed);

            LoadAnimation(0, &locale->doorE_right_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorE_right_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorE_right_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorE_right_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorE_right_open_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorE_right_open_anim,
               8, 1.0, drawW, drawH);
         } else {
            part1.hitBox = locale->doorE_left_part1Clip;
            part2.hitBox = locale->doorE_left_part2Clip;
            door.hitBox = locale->doorE_left_doorClip;

            LoadTexture(0, &locale->doorE_left_open);
            LoadTexture(1, &locale->doorE_left_closed);

            LoadAnimation(0, &locale->doorE_left_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorE_left_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorE_left_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorE_left_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorE_left_open_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorE_left_open_anim,
               8, 1.0, drawW, drawH);
         }
         break;
      case 2:
         if (hand) {
            part1.hitBox = locale->doorNS_right_outside_part1Clip;
            part2.hitBox = locale->doorNS_right_outside_part2Clip;
            door.hitBox = locale->doorNS_right_outside_doorClip;

            LoadTexture(0, &locale->doorNS_right_outside_open);
            LoadTexture(1, &locale->doorNS_right_outside_closed);

            LoadAnimation(0, &locale->doorNS_right_outside_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorNS_right_outside_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorNS_right_outside_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorNS_right_outside_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorNS_right_outside_break_out_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorNS_right_outside_break_in_anim,
               8, 1.0, drawW, drawH);
         } else {
            part1.hitBox = locale->doorNS_left_outside_part1Clip;
            part2.hitBox = locale->doorNS_left_outside_part2Clip;
            door.hitBox = locale->doorNS_left_outside_doorClip;

            LoadTexture(0, &locale->doorNS_left_outside_open);
            LoadTexture(1, &locale->doorNS_left_outside_closed);

            LoadAnimation(0, &locale->doorNS_left_outside_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorNS_left_outside_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorNS_left_outside_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorNS_left_outside_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorNS_left_outside_break_out_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorNS_left_outside_break_in_anim,
               8, 1.0, drawW, drawH);
         }
         break;
      case 3:
         if (hand) {
            part1.hitBox = locale->doorW_right_part1Clip;
            part2.hitBox = locale->doorW_right_part2Clip;
            door.hitBox = locale->doorW_right_doorClip;

            LoadTexture(0, &locale->doorW_right_open);
            LoadTexture(1, &locale->doorW_right_closed);

            LoadAnimation(0, &locale->doorW_right_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorW_right_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorW_right_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorW_right_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorW_right_open_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorW_right_open_anim,
               8, 1.0, drawW, drawH);
         } else {
            part1.hitBox = locale->doorW_left_part1Clip;
            part2.hitBox = locale->doorW_left_part2Clip;
            door.hitBox = locale->doorW_left_doorClip;

            LoadTexture(0, &locale->doorW_left_open);
            LoadTexture(1, &locale->doorW_left_closed);

            LoadAnimation(0, &locale->doorW_left_open_anim,
               8, 1.5, drawW, drawH);
            LoadAnimation(1, &locale->doorW_left_open_anim,
               8, 0.5, drawW, drawH);
            LoadAnimation(2, &locale->doorW_left_open_anim,
               8, 1.5, drawW, drawH, true);
            LoadAnimation(3, &locale->doorW_left_open_anim,
               8, 0.5, drawW, drawH, true);
            LoadAnimation(4, &locale->doorW_left_open_anim,
               8, 1.0, drawW, drawH);
            LoadAnimation(5, &locale->doorW_left_open_anim,
               8, 1.0, drawW, drawH);
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

   // TODO: Remove this after testing
   // Temporarily setting all doors to open, as far as colliders are concerned
   door.hitBox.w = 0;
   door.hitBox.h = 0;
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



void Door::Open(bool fast) {
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
