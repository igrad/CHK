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

SDL_Rect* Collider::GetHitBox() {
   return &hitBox;
}

SDL_Rect* Collider::GetDrawBox() {
   return &drawBox;
}

void Collider::GetFoot(double* x, double* y) {
   *x = xPos + hitBoxXOffset + hitBox.w/2;
   *y = yPos + hitBoxYOffset + hitBox.h;
}

void Collider::GetFoot(int* x, int* y) {
   *x = (int)(xPos + hitBoxXOffset + hitBox.w/2);
   *y = (int)(yPos + hitBoxYOffset + hitBox.h);
}

void Collider::GetCoM(double* x, double* y) {
   *x = xPos + max(hitBox.w, drawBox.w)/2;
   *y = yPos + max(hitBox.h, drawBox.h)/2;
}

void Collider::GetCoM(int* x, int* y) {
   *x = (int)(xPos + max(hitBox.w, drawBox.w)/2);
   *y = (int)(yPos + max(hitBox.h, drawBox.h)/2);
}

void Collider::SetXPos(double x) {
   if (x != -999) { xPos = x; }

   hitBox.x = (int)xPos + hitBoxXOffset;
}

void Collider::SetYPos(double y) {
   if (y != -999) { yPos = y; }

   hitBox.y = (int)yPos + hitBoxYOffset;
}

void Collider::SetPos(double x, double y) {
   SetXPos(x);
   SetYPos(y);
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

void Collider::SetHitBoxOffsets(int x, int y) {
   hitBoxXOffset = x;
   hitBoxYOffset = y;
}

Collider::~Collider() {

}
