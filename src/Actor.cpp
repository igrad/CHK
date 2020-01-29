#include "..\include\Actor.h"
#include "..\include\Level.h"

// Actors
// Actors are any entity within a level than can have animations and be
// expected to move. They're essentially a dynamic iteration of Colliders.
// Actors can play animations or static textures. Animations may be walking,
// exploding, or anything animated. Textures are rarely used for actual
// characters (i.e. players or NPCs), but more for dynamic props within the
// environment (like a box).

// Default constructor
Actor::Actor() {

}

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

   ActorY ay;
   ay.NewActor(this);
   if (allActorsIndex == 0) ActorY::PushActor(ay);
}

// Set hitBox dimensions
void Actor::SetHitBoxSize(int x, int y, int w, int h) {
   hitBoxXOffset = x * zoom;
   hitBoxYOffset = y * zoom;

   hitBox.x = (int)xPos + hitBoxXOffset;
   hitBox.y = (int)yPos + hitBoxYOffset;
   hitBox.w = w * zoom;
   hitBox.h = h * zoom;
}

// Set drawBox dimensions and bounding circle's radius
void Actor::SetDrawBoxSize(int x, int y, int w, int h) {
   drawBox.x = x;
   drawBox.y = y;
   drawBox.w = w * zoom;
   drawBox.h = h * zoom;

   boundingRadius = ceilf(FindDistance(
      ceilf(drawBox.w/2), ceilf(drawBox.h/2), 0, 0));
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
   if (!animsAllocated) animsAllocated = true;

   return anims[phase].LoadFromFile(path, frames, duration, frameW, frameH,
      reversed);
}

// Load an animation from a reference to an LTexture
bool Actor::LoadAnimation(int phase, LTexture* ref, int frames,
   float duration, int frameW, int frameH, bool reversed) {
   if (!animsAllocated) animsAllocated = true;

   return anims[phase].LoadFromReference(ref, frames, duration, frameW, frameH,
      reversed);
}

// Set the actor's current animation
void Actor::SetActiveAnim(int phase, bool loop) {
   usingAnims = true;
   activeAnim = phase;
   anims[phase].looping = loop;
   anims[phase].currentFrame = 0;
   anims[phase].animDone = false;

   if (!loop) anims[phase].SetAnimFrameOffset(0);
}

// Load a texture from file
bool Actor::LoadTexture(int phase, string path) {
   if (!texturesAllocated) animsAllocated = true;

   return textures[phase].LoadFromFile(path);
}

// Load a texture from another LTexture's reference
bool Actor::LoadTexture(int phase, LTexture* ref) {
   if (!texturesAllocated) animsAllocated = true;

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

// Handle movement plots
void Actor::HandleMovement(Level* level) {
   // Log("Handling movement");
   hitBox.x = (int)xPos + hitBoxXOffset;
   hitBox.y = (int)yPos + hitBoxYOffset;

   if (walkingPath) {
      MoveTowards(pathNodes.back().first, pathNodes.back().second, currentSpeed);
   }
}

// Move towards a target destination
void Actor::MoveTowards(double destX, double destY, int speedType) {
   // Find the feet position
   int footX, footY;
   GetFoot(&footX, &footY);

   // printf("\nMoving from {%i, %i} to {%i, %i}", 
   //    footX, footY, (int)destX, (int)destY);

   // The speed types are:
   //    0: zero speed
   //    1: slow speed (slowVelocity)
   //    2: average speed (avgVelocity)
   //    3: top speed (maxVelocity)
   
   // Log("Moving towards...");
   float velocity = 0.f;
   switch (speedType) {
      case 0:
         walkingPath = false;
         return;
      case 1:
         velocity = slowVelocity;
         break;
      case 2:
         velocity = avgVelocity;
         break; 
      case 3:
         velocity = maxVelocity;
         break;
   }
   
   // If we're very close to the target, just move the remaining distance
   if (FindDistance(footX, footY, (int)destX, (int)destY) < velocity) {
      // printf("\nSetting position from {%i, %i} to {%i, %i}", (int)footX, (int)footY, (int)(destX - (footX - xPos)), (int)(destY - (footY - yPos)));
      SetPos(destX - (footX - xPos), destY - (footY - yPos));

      pathNodes.pop_back();

      if (pathNodes.size() == 0) {
         walkingPath = false;
         SetCurrentSpeed(0);
      }
   } else {
      float vX = destX - footX;
      float vY = destY - footY;

      float vMag = FindDistance(footX, footY, destX, destY);

      double newX = (vX/vMag) * velocity;
      double newY = (vY/vMag) * velocity;

      // printf("\nSetting position from {%i, %i} to {%i + %i, %i + %i}", (int)footX, (int)footY, (int)footX, (int)newX, (int)footY, (int)newY);

      SetPos(xPos + newX, yPos + newY);
   }
}

// Plot movement from current position to another point on the map by avoiding
// obstacles
void Actor::PlotMovement(Level* level, double destX, double destY) {
   // Determine if the dest is in-bounds (not a wall)
   // If it's OOB, make the destination the closest (revealed) inbounds point
   // that can be reached.
   bool OOB = false;
   int footX, footY;
   GetFoot(&footX, &footY);

   int dX, dY;
   level->NearestVacantGrid((int)destX, (int)destY, &dX, &dY);
   Log("Found nearest vacancy");

   // Find a path to the target destination
   // This is the full path - not the checkpoints that we want.
   // This vector has all of the nodes stored in correct order, 0-n
   vector<pair<int,int>> path;
   path.push_back(make_pair(footX, footY));
   if (!level->FindGroundRoute(footX, footY, dX, dY, &path)) {
      Warn("Failed to find ground path to target");
      return;
   }
   path.push_back(make_pair(dX, dY));

   Log("Here's the raw oath");
   for (int i = 0; i < path.size(); i++) {
      printf("\n\t {%i, %i}", path[i].first, path[i].second);
   }

   Log("Found ground route");
   int tileW = PIXELSPERFEET * GZOOM * 5;
   SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0x50);
   for (int i = 0; i < path.size(); i++) {
      const SDL_Rect r = {
         (int)(path[i].first - (tileW/2) - Camera::x),
         (int)(path[i].second - (tileW/2) - Camera::y),
         tileW,
         tileW
      };
      SDL_RenderFillRect(gRenderer, &r);
   }
   SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderPresent(gRenderer);
   cin.get();

   // From here, we can take our path and trim it down until we get individual
   // path nodes to travel along. We do this by travelling through the vector
   // to find the furthest path point that we can see from all corners of the
   // hitbox.

   bool donePathing = false;
   if (pathNodes.size() > 0) pathNodes.clear();

   // int tileW = PIXELSPERFEET * GZOOM * 5;
   int x1 = hitBox.x;
   int y1 = hitBox.y;
   
   int lastNode = 0;
   Log("Starting LoS pathing");
   while (!donePathing) {
      int furthestLoSPoint = lastNode + 1;
      for (int pathPoint = lastNode + 2; pathPoint < path.size(); pathPoint++) {
         int x2 = path[pathPoint].first;
         int y2 = path[pathPoint].second;
         printf("\n\n\tChecking LoS from {%i, %i} to {%i, %i}", x1, y1, x2, y2);
         if (level->HasLineOfSight(x1, y1, x2, y2)) {
            furthestLoSPoint = pathPoint;
         } else break;
      }

      printf("\nfurthestLoSPoint: %i to %i", lastNode, furthestLoSPoint);
      pathNodes.insert(pathNodes.begin(), path[furthestLoSPoint]);
      Log("Pushed");
      if (furthestLoSPoint == path.size() - 1) donePathing = true;
      else {
         lastNode = furthestLoSPoint;
         x1 = path[lastNode].first;
         y1 = path[lastNode].second;
         Log("Next cycle");
      }
   }

   Log("HERE'S THE PATH");
   printf("\nStarting pos: %i, %i", footX, footY);
   for (int i = 0; i < pathNodes.size(); i++) {
      printf("\n\tPath point %i: %i, %i", i, path[i].first, path[i].second);
   }

   SDL_SetRenderDrawColor(gRenderer, 0, 0, 0xFF, 0xFF);
   for (int i = 0; i < pathNodes.size(); i++) {
      const SDL_Rect r = {
         (int)(pathNodes[i].first - Camera::x) - 4,
         (int)(pathNodes[i].second - Camera::y) - 4,
         9,
         9
      };
      SDL_RenderFillRect(gRenderer, &r);
   }
   SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
   SDL_RenderPresent(gRenderer);
   cin.get();

   // The path nodes have been set
   walkingPath = true;
}

void Actor::SetCurrentSpeed(int newSpeed) {
   if (newSpeed != currentSpeed) {
      currentSpeed = newSpeed;

      switch (currentSpeed) {
         case 0:
            currentVelocity = 0.f;
            break;
         case 1:
            currentVelocity = slowVelocity;
            break;
         case 2:
            currentVelocity = avgVelocity;
            break;
         case 3:
            currentVelocity = maxVelocity;
            break;
      }
   }
}

// Render the actor to the screen
void Actor::Render(int screenFrame) {
   drawBox.x = (xPos - Camera::x);
   drawBox.y = (yPos - Camera::y);

   bool animDone = false;
   if (usingAnims) {
      animDone = anims[activeAnim].animDone;
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
   if (animsAllocated && anims != NULL) {
      delete[] anims;
      animsAllocated = false;
   }

   if (texturesAllocated && textures != NULL) {
      delete[] textures;
      texturesAllocated = false;
   }

   ActorY::PopActor(allActorsIndex);
}

// Deconstructor
Actor::~Actor() {
   Free();
}
