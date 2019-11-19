#include "..\include\Camera.h"
#include "..\include\Actor.h"

FOCUSMODE Camera::focusMode = FOCUS_PLAYER;
double Camera::x = 0;
double Camera::y = 0;
float Camera::zoom = 1.0;

void Camera::SetFocusOnRect(SDL_Rect r) {
   Camera::x = r.x;
   Camera::y = r.y;
}



void Camera::SetFocusOnPoint(double nx, double ny) {
   Camera::x = nx;
   Camera::y = ny;
}




void Camera::SetFocusOnActor(Actor* a) {
   Camera::x = a->xPos - (SCREEN_WIDTH - a->drawBox.w)/2;
   Camera::y = a->yPos - (SCREEN_HEIGHT - a->drawBox.h)/2;
}
