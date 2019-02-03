#include "..\include\Collider.h"

Collider::Collider() {
   hitBox = {0, 0, 0, 0};
   drawBox = {0, 0, 0, 0};
}



void Collider::SetPos(int x, int y) {
   if (x != -1) { xPos = x; }
   if (y != -1) { yPos = y; }
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



void HandleCollisions() {
   
}



Collider::~Collider() {

}
