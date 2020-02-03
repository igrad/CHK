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
   int x = hitBox.x;
   int y = hitBox.y;
   if (FindDistance(x, y, (int)destX, (int)destY) < velocity) {
      // printf("\nSetting position from {%i, %i} to {%i, %i}", (int)footX, (int)footY, (int)(destX - (footX - xPos)), (int)(destY - (footY - yPos)));
      SetPos(destX - (x - xPos), destY - (y - yPos));

      pathNodes.pop_back();

      if (pathNodes.size() == 0) {
         walkingPath = false;
         SetCurrentSpeed(0);
      }
   } else {
      float vX = destX - x;
      float vY = destY - y;

      float vMag = FindDistance(x, y, destX, destY);

      double newX = (vX/vMag) * velocity;
      double newY = (vY/vMag) * velocity;

      // printf("\nSetting position from {%i, %i} to {%i + %i, %i + %i}", (int)x, (int)footY, (int)x, (int)newX, (int)footY, (int)newY);

      SetPos(xPos + newX, yPos + newY);
   }
}

// Plot movement from current position to another point on the map by avoiding
// obstacles
void Actor::PlotMovement(Level* level, double destX, double destY) {
   // Determine if the dest is in-bounds (not a wall)
   // If it's OOB, make the destination the closest (revealed) inbounds point
   // that can be reached.
   int TILEW = PIXELSPERFEET * 5 * GZOOM;
   int HALFTILEW = (int)(TILEW/2);
   bool OOB = false;

   int footX, footY;
   GetFoot(&footX, &footY);

   int dX, dY;
   level->NearestVacantGrid((int)destX, (int)destY, &dX, &dY);

   // Find a path to the target destination
   // This is the full path - not the checkpoints that we want.
   // This vector has all of the nodes stored in correct order, 0-n
   vector<pair<int,int>> path;
   if ((int)(footX/TILEW) == (int)(destX/TILEW) &&
      (int)(footY/TILEW) == (int)(destY/TILEW)){
      path.push_back(make_pair((int)(footX/TILEW), (int)(destX/TILEW)));
   } else if (!level->FindGroundRoute(footX, footY, dX, dY, &path)) {
      if (DEBUG_MOVEMENT) Warn("Failed to find ground path to target");
      return;
   }

   if (DEBUG_MOVEMENT) {
      Log("Here's the raw path");
      for (int i = 0; i < path.size(); i++) {
         printf("\n\t {%i, %i}", path[i].first, path[i].second);
      }

      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0x50);
      for (int i = 0; i < path.size(); i++) {
         const SDL_Rect r = {
            (int)(path[i].first - (TILEW/2) - Camera::x),
            (int)(path[i].second - (TILEW/2) - Camera::y),
            TILEW,
            TILEW
         };
         SDL_RenderFillRect(gRenderer, &r);
      }
      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderPresent(gRenderer);
      cin.get();
   }

   // From here, we can take our path and trim it down until we get individual
   // path nodes to travel along. We do this by travelling through the vector
   // to find the furthest path point that we can see from all corners of the
   // hitbox.
   if (DEBUG_MOVEMENT) Log("Starting blocking");
   vector<NavBlock> blocks;

   int x1 = hitBox.x;
   int y1 = hitBox.y;
   int w = hitBox.w;
   int h = hitBox.h;
   blocks.push_back({x1, y1, true});
   
   int lastDir = NORTH;
   if (path[1].first > path[0].first) lastDir = EAST;
   else if (path[1].second > path[0].second) lastDir = SOUTH;
   else if (path[1].first < path[0].first) lastDir = WEST;

   int dir, cX, cY, cNX, cNY, tX, tY, tNX, tNY;
   for (int tile = 1; tile < path.size() - 1; tile++) {
      dir = NORTH;
      cX = (int)(path[tile].first / TILEW);
      cY = (int)(path[tile].second / TILEW);
      cNX = (int)(path[tile+1].first / TILEW);
      cNY = (int)(path[tile+1].second / TILEW);
      tX = path[tile].first;
      tY = path[tile].second;
      tNX = path[tile+1].first;
      tNY = path[tile+1].second;

      if (tNX > tX) dir = EAST;
      else if (tNY > tY) dir = SOUTH;
      else if (tNX < tX) dir = WEST;

      // If we see a change in direction, we need to add a block to the inner
      // corner
      int blockX, blockY;
      if (lastDir != dir) {
         if (dir == NORTH || lastDir == SOUTH) {
            blockY = tY - HALFTILEW + 1;
         } else blockY = tY + HALFTILEW - h - 1;

         if (dir == WEST || lastDir == EAST) {
            blockX = tX - HALFTILEW + 1;
         } else blockX = tX + HALFTILEW - w - 1;

         // Rounding the corner of a half-height wall
         if (((lastDir == EAST && dir == SOUTH) || 
            (lastDir == NORTH && dir == WEST)) && 
            level->IsWall(cX - 1, cY + 1)) {
            blockY += HALFTILEW;
         } else if (((lastDir == NORTH && dir == EAST) || 
            (lastDir == WEST && dir == SOUTH)) && 
            level->IsWall(cX + 1, cY + 1)) {
            blockY += HALFTILEW;
         }

         blocks.push_back({blockX, blockY, false});
      } else if (level->ground[cY][cX] == T_DOOR) {
         // Direction has remained the same, but we're in a doorway
         blockX = tX - (int)(w/2);
         blockY = tY - h;

         blocks.push_back({blockX, blockY, true});
      } else if (level->ground[cNY][cNX] == T_DOOR) {
         // The next tile is a door, so we want to get close to it in case it's
         // closed

         // TODO: Leaving this out for now, can revisit later. Currently, this puts an additional block in the door tile's space, which means it won't be "seen" when searching through LoS pathing. A few minor adjustments would create a point close to the door, but for now we don't want that.
         // switch(dir) {
         // case NORTH:
         //    blockX = tX - (int)(w/2);
         //    blockY = tY - HALFTILEW + h + 1;
         //    break;
         // case EAST:
         //    blockX = tNX + HALFTILEW - w - 1 -
         //       level->locale->doorE_part1Clip.w;
         //    blockY = tY - (int)(h/2);
         //    break;
         // case SOUTH:
         //    blockX = tX - (int)(w/2);
         //    blockY = tNY + HALFTILEW - h - 1 - 
         //       level->locale->doorNS_left_inside_part1Clip.h;
         //    break;
         // case WEST:
         //    blockX = tNX - HALFTILEW + w + 1 +
         //       level->locale->doorE_part1Clip.w;
         //    blockY = tY - (int)(h/2);
         //    break;
         // }

         // blocks.push_back({blockX, blockY, true});
      }

      lastDir = dir;
   }

   blocks.push_back({dX - (int)(w / 2), dY - h, true});

   if (DEBUG_MOVEMENT) {
      printf("\nBlocks: %i", blocks.size());
      for (int b = 0; b < blocks.size(); b++) {
         printf("\n\t%i %i", blocks[b].x, blocks[b].y);
      }

      SDL_SetRenderDrawColor(gRenderer, 0, 0xFF, 0, 0xFF);
      for (int i = 0; i < blocks.size(); i++) {
         const SDL_Rect r = {
            (int)(blocks[i].x - Camera::x),
            (int)(blocks[i].y - Camera::y),
            w,
            h
         };
         SDL_RenderFillRect(gRenderer, &r);
      }
      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderPresent(gRenderer);
      cin.get();
   }

   if (pathNodes.size() > 0) pathNodes.clear();

   int lastBlock = 0;
   if (DEBUG_MOVEMENT) Log("Starting LoS pathing between blocks");
   bool LoSPathing = true;
   while (LoSPathing) {
      int furthestLoSBlock = lastBlock;
      for (int block = lastBlock + 1; block < blocks.size(); block++) {
         int x2 = blocks[block].x;
         int y2 = blocks[block].y;
         if (level->HasLineOfSight(x1, y1, x2, y2) &&
            level->HasLineOfSight(x1+w, y1, x2+w, y2) &&
            level->HasLineOfSight(x1, y1+h, x2, y2+h) &&
            level->HasLineOfSight(x1+w, y1+h, x2+w, y2+h)) {
            furthestLoSBlock = block;
            if (blocks[block].mustUse) break;
         } else break;
      }

      pathNodes.insert(pathNodes.begin(), 
         make_pair(blocks[furthestLoSBlock].x, blocks[furthestLoSBlock].y));
      if (furthestLoSBlock == blocks.size() - 1) LoSPathing = false;
      else if (furthestLoSBlock == lastBlock) LoSPathing = false;
      else {
         lastBlock = furthestLoSBlock;
         x1 = blocks[lastBlock].x;
         y1 = blocks[lastBlock].y;
      }
   }

   if (DEBUG_MOVEMENT) { 
      Log("HERE'S THE PATH");
      printf("\nStarting pos: %i, %i", footX, footY);
      for (int i = 0; i < pathNodes.size(); i++) {
         printf("\n\tPath point %i: %i, %i", i, path[i].first, path[i].second);
      }

      SDL_SetRenderDrawColor(gRenderer, 0, 0, 0xFF, 0xFF);
      for (int i = 0; i < pathNodes.size(); i++) {
         const SDL_Rect r = {
            (int)(pathNodes[i].first - Camera::x),
            (int)(pathNodes[i].second - Camera::y),
            w,
            h
         };
         SDL_RenderFillRect(gRenderer, &r);
      }
      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderPresent(gRenderer);
      cin.get();
   }

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
