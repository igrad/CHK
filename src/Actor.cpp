#include "..\include\Actor.h"

Actor::Actor(int numAnims, int numTextures) {
   zoom = GZOOM;

   hasAnimations = (numAnims > 0);
   hasTextures = (numTextures > 0);

   if (hasAnimations) {
      this->numAnims = numAnims;
      anims = new Animation[numAnims];
   } else {
      anims = NULL;
   }

   if (hasTextures) {
      this->numTextures = numTextures;
      textures = new LTexture[numTextures];
   } else {
      textures = NULL;
   }

   activeAnim = 0;
   activeTexture = 0;


   SetDrawBoxSize(0, 0, 40, 80);
}



void Actor::SetXPos(double x) {
   if (x != -0.1) { xPos = x; }
   hitBox.x = xPos + hitBoxXOffset;
}



void Actor::SetYPos(double y) {
   if (y != -0.1) { yPos = y; }
   hitBox.y = yPos + hitBoxYOffset;
}



void Actor::SetPos(double x, double y) {
   SetXPos(x);
   SetYPos(y);
}



void Actor::SetHitBoxSize(int x, int y, int w, int h) {
   hitBox.x = x * zoom;
   hitBox.y = y * zoom;
   hitBox.w = w * zoom;
   hitBox.h = h * zoom;

   hitBoxXOffset = x * zoom;
   hitBoxYOffset = y * zoom;
}



void Actor::SetDrawBoxSize(int x, int y, int w, int h) {
   drawBox.x = x;
   drawBox.y = y;
   drawBox.w = w * zoom;
   drawBox.h = h * zoom;
}



void Actor::SetZoom(float newZoom) {
   zoom = newZoom * GZOOM;
   drawBox.w = drawBox.w * zoom;
   drawBox.h = drawBox.h * zoom;
   hitBox.w = hitBox.w * zoom;
   hitBox.h = hitBox.h * zoom;
}



bool Actor::LoadAnimation(int phase, string path, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   return anims[phase].LoadFromFile(path, frames, duration, frameW, frameH,
      reversed);
}



bool Actor::LoadAnimation(int phase, LTexture* ref, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   return anims[phase].LoadFromReference(ref, frames, duration, frameW, frameH,
      reversed);
}



void Actor::SetActiveAnim(int phase) {
   usingAnims = true;
   anims[phase].currentFrame = 0;
   activeAnim = phase;
}



bool Actor::LoadTexture(int phase, string path) {
   return textures[phase].LoadFromFile(path);
}



bool Actor::LoadTexture(int phase, LTexture* ref) {
   return textures[phase].LoadFromReference(ref);
}



void Actor::SetActiveTexture(int phase) {
   usingAnims = false;
   activeTexture = phase;
}



void Actor::HandleMovement(int camX, int camY) {
   xPos += xVelocity;
   yPos += yVelocity;

   hitBox.x = xPos + hitBoxXOffset;
   hitBox.y = yPos + hitBoxYOffset;

   drawBox.x = (xPos - camX);
   drawBox.y = (yPos - camY);
}



void Actor::Render(int screenFrame, int camX, int camY) {
   hitBox.x = xPos + hitBoxXOffset;
   hitBox.y = yPos + hitBoxYOffset;

   drawBox.x = (xPos - camX);
   drawBox.y = (yPos - camY);

   if (usingAnims) {
      anims[activeAnim].Render(GetDrawBox(), screenFrame);
   } else {
      textures[activeTexture].Render(GetDrawBox());
   }

   queueCollisions = false;
}



void Actor::Free() {
   if (numAnims > 0) {
      for (int i = 0; i < numAnims; i++) {
         anims[i].Free();
      }
   }

   if (numTextures > 0) {
      for (int i = 0; i < numTextures; i++) {
         textures[i].Free();
      }
   }
}



Actor::~Actor() {

}
