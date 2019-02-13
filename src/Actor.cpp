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



void Actor::SetHitBoxSize(int x, int y, int w, int h) {

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



bool Actor::LoadAnimation(int phase, string path, int frames, float duration, int frameW, int frameH) {
   return anims[phase].LoadFromFile(path, frames, duration, frameW, frameH);
}



void Actor::SetActiveAnim(int phase) {
   usingAnims = true;
   // anims[phase].currentFrame = 0;
   activeAnim = phase;
}



bool Actor::LoadTexture(int phase, string path) {
   return textures[phase].LoadFromFile(path);
}



void Actor::SetActiveTexture(int phase) {
   usingAnims = false;
   activeTexture = phase;
}



void Actor::Render(int screenFrame, int camX, int camY) {
   drawBox.x = (xPos - camX);
   drawBox.y = (yPos - camY);

   if (usingAnims) {
      anims[activeAnim].Render(GetDrawBox(), screenFrame);
   } else {
      textures[activeTexture].Render(GetDrawBox());
   }
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
