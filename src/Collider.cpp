#include "..\include\Collider.h"

Collider::Collider() {
   hitBox = {0, 0, 0, 0};
   drawBox = {0, 0, 0, 0};

   xPos = 0;
   yPos = 0;
}



Collider* Collider::operator=(const Collider* o) {
   if (this != o) {
      xPos = o->xPos;
      yPos = o->yPos;
      hitBox = o->hitBox;
      drawBox = o->drawBox;
   }

   return this;
}



void Collider::SetPos(double x, double y) {
   if (x != -1) { xPos = x; }
   if (y != -1) { yPos = y; }

   drawBox.x = (int) xPos;
   drawBox.y = (int) yPos;
}



void Collider::SetHitBox(int x, int y, int w, int h) {
   if (x != -1) { hitBox.x = x; }
   if (y != -1) { hitBox.y = y; }
   if (w != -1) { hitBox.w = w; }
   if (h != -1) { hitBox.h = h; }
}



void Collider::SetDrawBox(int x, int y, int w, int h) {
   if (x != -1) { drawBox.x = x; }
   if (y != -1) { drawBox.y = y; }
   if (w != -1) { drawBox.w = w; }
   if (h != -1) { drawBox.h = h; }
}



SDL_Rect* Collider::GetHitBox() {
   return &hitBox;
}

SDL_Rect* Collider::GetDrawBox() {
   return &drawBox;
}



Collider::~Collider() {

}
