#include "..\include\Camera.h"
#include "..\include\Actor.h"

FOCUSMODE Camera::focusMode = FOCUS_AREA;
double Camera::x = 0;
double Camera::y = 0;
double Camera::xSpeed = 6;
double Camera::ySpeed = 9;
int Camera::updateX = 0;
int Camera::updateY = 0;
float Camera::zoom = 1.0;

void Camera::Update() {
   Camera::x += xSpeed * updateX;
   Camera::y += ySpeed * updateY;
}

void Camera::SetXDirPos() {
   Camera::x += xSpeed * 1.0;
}

void Camera::SetXDirNeg() {
   Camera::x += xSpeed * -1.0;
}

void Camera::SetYDirPos() {
   Camera::y += xSpeed * 1.0;
}

void Camera::SetYDirNeg() {
   Camera::y += ySpeed * -1.0;
}

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
