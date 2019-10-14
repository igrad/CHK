#include "..\include\Actor.h"

// Actors
// Actors are any entity within a level than can have animations and be
// expected to move. They're essentially a dynamic iteration of Colliders.
// Actors can play animations or static textures. Animations may be walking,
// exploding, or anything animated. Textures are rarely used for actual
// characters (i.e. players or NPCs), but more for dynamic props within the
// environment (like a box).

// Default constructor
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

   bufferedText = -1;
   bufferedAnim = -1;

   SetDrawBoxSize(0, 0, 40, 80);
}

// Set X position of actor within the level, with (0.00, 0.00) being the
// top-left corner of the map
void Actor::SetXPos(double x) {
   if (x != -0.1) { xPos = x; }
   hitBox.x = xPos + hitBoxXOffset;
}

// Set Y position of actor within the level
void Actor::SetYPos(double y) {
   if (y != -0.1) { yPos = y; }
   hitBox.y = yPos + hitBoxYOffset;
}

// Set both X and Y position of actor within the level
void Actor::SetPos(double x, double y) {
   SetXPos(x);
   SetYPos(y);
}

// Set hitBox dimensions
void Actor::SetHitBoxSize(int x, int y, int w, int h) {
   hitBox.x = x * zoom;
   hitBox.y = y * zoom;
   hitBox.w = w * zoom;
   hitBox.h = h * zoom;

   hitBoxXOffset = x * zoom;
   hitBoxYOffset = y * zoom;
}

// Set drawBox dimensions
void Actor::SetDrawBoxSize(int x, int y, int w, int h) {
   drawBox.x = x;
   drawBox.y = y;
   drawBox.w = w * zoom;
   drawBox.h = h * zoom;
}

// Set zoom of the actor's artwork
void Actor::SetZoom(float newZoom) {
   zoom = newZoom * GZOOM;
   drawBox.w = drawBox.w * zoom;
   drawBox.h = drawBox.h * zoom;
   hitBox.w = hitBox.w * zoom;
   hitBox.h = hitBox.h * zoom;
}

// Load an animation from file
bool Actor::LoadAnimation(int phase, string path, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   return anims[phase].LoadFromFile(path, frames, duration, frameW, frameH,
      reversed);
}

// Load an animation from a reference to an LTexture
bool Actor::LoadAnimation(int phase, LTexture* ref, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   return anims[phase].LoadFromReference(ref, frames, duration, frameW, frameH,
      reversed);
}

// Set the actor's current animation
void Actor::SetActiveAnim(int phase, bool loop) {
   usingAnims = true;
   activeAnim = phase;
   anims[phase].looping = loop;

   if (!loop) {
      anims[phase].currentFrame = 0;
      anims[phase].SetAnimFrameOffset(0);
   }
}

// Load a texture from file
bool Actor::LoadTexture(int phase, string path) {
   return textures[phase].LoadFromFile(path);
}

// Load a texture from another LTexture's reference
bool Actor::LoadTexture(int phase, LTexture* ref) {
   return textures[phase].LoadFromReference(ref);
}

// Set the active texture of the actor
void Actor::SetActiveTexture(int phase) {
   usingAnims = false;
   activeTexture = phase;
}

// Queue a texture to be displayed after the current animation is completed
void Actor::BufferTexture(int textPhase) {
   bufferedText = textPhase;
}

// Queue another animation to play after the current animation is completed
void Actor::BufferAnimation(int animPhase, bool loop) {
   bufferedAnim = animPhase;
   loopBufferedAnim = loop;
}

// Position the actor on-screen according to the player's current camera
// position
void Actor::HandleMovement(int camX, int camY) {
   xPos += xVelocity;
   yPos += yVelocity;

   hitBox.x = xPos + hitBoxXOffset;
   hitBox.y = yPos + hitBoxYOffset;

   drawBox.x = (xPos - camX);
   drawBox.y = (yPos - camY);
}

// Render the player to the screen
void Actor::Render(int screenFrame, int camX, int camY) {
   hitBox.x = xPos + hitBoxXOffset;
   hitBox.y = yPos + hitBoxYOffset;

   drawBox.x = (xPos - camX);
   drawBox.y = (yPos - camY);

   bool animDone = false;
   if (usingAnims) {
      animDone = anims[activeAnim].animDone;

      // printf("\nActiveAnim: %i, drawbox: %i %i %i %i", activeAnim, drawBox.x, drawBox.y, drawBox.w, drawBox.h);
      anims[activeAnim].Render(GetDrawBox(), screenFrame);
   } else {
      textures[activeTexture].Render(GetDrawBox());
   }

   // Check for buffered animations/textures queued up
   if (usingAnims && animDone) {
      if (bufferedAnim > -1) {
         SetActiveAnim(bufferedAnim, loopBufferedAnim);
         bufferedAnim = -1;
      } else if (bufferedText > -1) {
         SetActiveTexture(bufferedText);
         bufferedText = -1;
      }
   }

   queueCollisions = false;
}

// Destroy the actor and free memory
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

   if (hasAnimations) delete[] anims;
   if (hasTextures) delete[] textures;
}

// Deconstructor
Actor::~Actor() {
   Free();
}
