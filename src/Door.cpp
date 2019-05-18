#include "..\include\Door.h"

Door::Door(int gridX, int gridY, int gridW, int gridH, int direction,
   Locale* locale):
   Actor(6, 2) {
   char buf[6];
   snprintf(buf, 6, "%i%i", gridX, gridY);
   id = buf;

   int tileW = PIXELSPERFEET * 5 * GZOOM;
   xPos = gridX * tileW;
   yPos = gridY * tileW;

   SDL_Rect r = {(int)xPos, (int)yPos, gridW * tileW, gridH * tileW};
   drawBox = r;
   hitBox = r;

   this->direction = direction;
   this->locale = locale;

   isOpen = false;
   isLocked = false;
   keyID = "";
   difficulty = 0;
}



void Door::LoadArt() {
   switch (direction) {
      case 0:
         // Door textures
         LoadTexture(0, &locale->doorN_open);
         LoadTexture(1, &locale->doorN_shut);

         // Door animations
         LoadAnimation(0, &locale->doorN_opening_slow, 8, 1.5, 40, 40);
         LoadAnimation(1, &locale->doorN_opening_fast, 8, 0.5, 40, 40);
         LoadAnimation(2, &locale->doorN_closing_slow, 8, 1.5, 40, 40);
         LoadAnimation(3, &locale->doorN_closing_fast, 8, 0.5, 40, 40);
         LoadAnimation(4, &locale->doorN_break_S, 8, 1.0, 40, 40);
         LoadAnimation(5, &locale->doorN_break_N, 8, 1.0, 40, 40);
   }
}



void Door::SetLocked(string key) {
   keyID = key;
   isLocked = true;
}



void Door::AttemptUnlock(string key) {
   if (key == keyID) Unlock();
}



void Door::Open(int speed) {
   if (!isOpen) {
      isOpen = true;
      isLocked = false;

      hitBox.w = 0;
      hitBox.h = 0;
      // Queue animation of door opening
   }
}



void Door::Close(int speed) {
   if (isOpen) {
      isOpen = false;
      hitBox = drawBox;
      // Queue animation of door closing
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



Door::~Door() {
   delete locale;
}
