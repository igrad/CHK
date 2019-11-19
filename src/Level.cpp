#include "..\include\Level.h"

int NUMROOMCOLLISIONS = 0;
int NUMROOMCOLLISIONSALLOWED = 0;
int MAXCORRIDORS = 0;



Room::Room() {
   x, y, w, h, cX, cY, connections, maxConnections = 0;
   rect = {0, 0, 0, 0};
   connections = 0;
   connectedRooms = new int[20];
   maxConnections = (rand() % 3) + 2;
}



Room::~Room() {
   delete[] connectedRooms;
}



Level::Level() {

}



Level::Level(Locale* locale) {
   this->locale = locale;
   groundSize = locale->mapSize;
   minRoomDim = locale->minRoomDim;
   maxRoomDim = locale->maxRoomDim;
   corridorType = locale->corridorType;
   corridorSize = locale->corridorSize;
   cornerType = locale->cornerType;
   cornerSize = locale->cornerSize;

   srand(time(NULL));

   decals = NULL;

   zoom = 1.0 * GZOOM;

   for (int i = 0; i < maxGroundSize; i++) {
      for (int j = 0; j < maxGroundSize; j++) {
         ground[i][j] = 0;
      }
   }

   // Generate 10 to 16 rooms
   int minRoomCount = locale->minRoomCount;
   int maxRoomCount = locale->maxRoomCount;
   int mod = maxRoomCount - minRoomCount;
   roomCount = (rand() % mod) + minRoomCount;
   rooms = new Room[roomCount];

   desiredRoomTiles = pow(((maxRoomDim + minRoomDim) /2 ), 2) * roomCount * 2/3;
   roomTileCount = 0;
   NUMROOMCOLLISIONSALLOWED = ceil(roomCount / 2);

   MAXCORRIDORS = roomCount;

   renderTargetsCompiled = false;
}



bool Level::IsGround(int x, int y) {
   int g = ground[y][x];
   if (g == 1) return true;
   if (g == 5) return true;
   if (g == 9) return true;
   if (g > 15) return true;
   return false;
}



bool Level::IsWall(int x, int y) {
   int g = ground[y][x];
   if (g == 2) return true;
   if (g == 3) return true;
   if (g == 6) return true;
   if (g == 7) return true;
   if (g == 10) return true;
   if (g == 11) return true;
   return false;
}



bool Level::CheckNewRoom(SDL_Rect* room) {
   // Check that it still fits on the board
   if (room->x + room->w > groundSize - 3) {
      return false;
   } else if (room->y + room->h > groundSize - 3) {
      return false;
   }

   // Setup wall height check rect
   SDL_Rect rhc = {room->x - 1, room->y - 1, room->w + 2, room->h + 2};

   // Check for collisions with other rooms
   bool passingCollision = false;
   for (int i = 0; i < roomCount; i++) {
      SDL_Rect* r = &rooms[i].rect;

      // TODO: This section needs heavy cleaning. We shouldn't have to make 3 calls for collision detection

      // Check for wall heights using the wall height check rect above
      if (IsRectCollision(&rhc, r)) { return false; }

      // If more than 3/4ths of the new room is merged with another room, it fails
      if (IsPartialCollision(room, r) > ((3/4) * room->w * room->h)) {
         return false;
      }

      // If we already have too many collisions, it fails
      if (IsRectCollision(room, r)) {
         if (NUMROOMCOLLISIONS >= NUMROOMCOLLISIONSALLOWED) {
            return false;
         }
         passingCollision = true;
      }
   }

   if (passingCollision) { NUMROOMCOLLISIONS++; }
   return true;
}



void Level::GenerateRandomRoom(int roomNum) {
   bool buildingRoom = true;
   while (buildingRoom) {
      // Use the generated coordinates to check if there is a room at that coordinate already, and if so, if there's any permitted collisions remaining
      int x = (rand() % (groundSize - 2)) + 2;
      int y = (rand() % (groundSize - 2)) + 2;

      // Give it 5 tries to try to make a room out of this chosen coordinate. If it can't create a room after 5 tries, it's probably too tight anyways
      for (int i = 0; i < 5; i++) {
         int w = (rand() % (maxRoomDim - minRoomDim + 1)) + minRoomDim;
         int h = (rand() % (maxRoomDim - minRoomDim + 1)) + minRoomDim;

         // If it passes the check, fill in the ground with this room's dimensions
         SDL_Rect room = {x, y, w, h};
         if (CheckNewRoom(&room)) {
            for (int yi = y; yi < y + h; yi++) {
               for (int xi = x; xi < x + w; xi++) {
                  ground[yi][xi] = 1;
               }
            }

            rooms[roomNum].rect = room;
            rooms[roomNum].x = x;
            rooms[roomNum].y = y;
            rooms[roomNum].w = w;
            rooms[roomNum].h = h;
            rooms[roomNum].area = w * h;
            rooms[roomNum].cX = x + (w/2);
            rooms[roomNum].cY = y + (h/2);

            // Go through all existing rooms and check if this room collides with any. If it does, we need to increase the connection counter
            for (int j = 0; j < roomsBuilt; j++) {
               if (IsRectCollision(&room, &rooms[j].rect)) {
                  rooms[roomNum].connectedRooms[rooms[roomNum].connections] = j;
                  rooms[roomNum].connections++;

                  rooms[j].connectedRooms[rooms[j].connections] = roomNum;
                  rooms[j].connections++;
               }
            }
            buildingRoom = false;
            break;
         }
      }
   }
}



bool Level::CheckWallHeights() {
   // Iterate through the whole map and find if we have created any walls that
   // are only one unit high. We want a wall height of at least two units so
   // that we can create tall walls in our levels
   for (int cY = 2; cY < groundSize - 2; cY++) {
      for (int cX = 2; cX < groundSize - 2; cX++) {
         int g = ground[cY][cX] % 10;
         if (g == 0 || g == 2 || g == 3) {
            if (((ground[cY - 1][cX] % 10) == 1) &&
            ((ground[cY + 1][cX] % 10) == 1)) {
               return false;
            }
         }
      }
   }
   return true;
}



void Level::ResetMod10Changes() {
   for (int cy = 0; cy < groundSize; cy++) {
      for (int cx = 0; cx < groundSize; cx++) {
         ground[cy][cx] = ground[cy][cx] % 10;
      }
   }
}



bool Level::PointInRoom(int x, int y, int room) {
   return PointInRect(x, y, &rooms[room].rect);
}



bool Level::PointInAnyRoom(int x, int y) {
   for (int i = 0; i < roomCount; i++) {
      if (PointInRoom(x, y, i)) return true;
   }
   return false;
}



bool Level::FindPath(int a, int b, int adir) {
   // Locate our starting room, and pick an appropriate starting point on the
   // primary edge
   int ax = rooms[a].rect.x;
   int ay = rooms[a].rect.y;
   int aw = rooms[a].rect.w;
   int ah = rooms[a].rect.h;
   int bx = rooms[b].rect.x;
   int by = rooms[b].rect.y;
   int bw = rooms[b].rect.w;
   int bh = rooms[b].rect.h;
   int bdir = NORTH;

   int x1, y1, x2, y2 = 0;

   // We will try 5 times to find an appropriate set of coordinates, then give
   // up on connecting these rooms
   // First we find the coords for room A
   int findingCoords = 0;
   while (findingCoords < 5) {
      findingCoords++;

      if (adir == NORTH) {
         x1 = (ax + cornerSize) + (rand() % (aw - 2*cornerSize));
         y1 = ay - 1;
      } else if (adir == EAST) {
         x1 = ax + aw;
         y1 = (ay + cornerSize) + (rand() % (ah - 2*cornerSize));
      } else if (adir == SOUTH) {
         x1 = (ax + cornerSize) + (rand() % (aw - 2*cornerSize));
         y1 = ay + ah;
      } else if (adir == WEST) {
         x1 = ax - 1;
         y1 = (ay + cornerSize) + (rand() % (ah - 2*cornerSize));
      }

      // Validate that we have two good points to work with
      // First, we have to check if either point has more than one ground tile
      // adjacent to it
      int groundCtr = 0;
      if (ground[y1 - 1][x1] == 1) groundCtr++;
      if (ground[y1][x1 + 1] == 1) groundCtr++;
      if (ground[y1 + 1][x1] == 1) groundCtr++;
      if (ground[y1][x1 - 1] == 1) groundCtr++;

      // Next, we validate that there aren't three or more wall tiles adjacent
      // to the end points
      int wallCtr = 0;
      if (ground[y1 - 1][x1] == 2) wallCtr++;
      if (ground[y1][x1 + 1] == 2) wallCtr++;
      if (ground[y1 + 1][x1] == 2) wallCtr++;
      if (ground[y1][x1 - 1] == 2) wallCtr++;

      // If our conditions are met, set our attempt counter to 6 so that we
      // don't interpret it as 5 failed tries
      if ((groundCtr < 2) && (wallCtr < 3)) findingCoords = 6;
   }

   // We tried to make this connection work, but it failed 5 times
   if (findingCoords == 5) return false;

   int bNSx = bx + (int)(bw/2);
   int bEWy = by + (int)(bh/2);

   int bN = (int)(FindDistance(bNSx, by - 1, x1, y1));
   int bE = (int)(FindDistance(bx + bw, bEWy, x1, y1));
   int bS = (int)(FindDistance(bNSx, by + bh, x1, y1));
   int bW = (int)(FindDistance(bx - 1, bEWy, x1, y1));

   int m = min(bN, min(bE, min(bS, bW)));

   // Next, we do the same thing to find coords for room b
   findingCoords = 0;
   while (findingCoords < 5) {
      findingCoords++;

      if (m == bN) {
         x2 = (bx + cornerSize) + (rand() % (bw - 2*cornerSize));
         y2 = (by - 1);
         bdir = NORTH;
      } else if (m == bE) {
         x2 = bx + bw;
         y2 = (by + cornerSize) + (rand() % (bh - 2*cornerSize));

         // Compensate for existing connections to prevent 1-high walls
         if (ground[y2 - 1][x2] == 1) y2 = y2 - 1;
         else if (ground[y2 - 2][x2] == 1) y2 = y2 - 2;
         else if (ground[y2 + 1][x2] == 1) y2 = y2 + 1;
         else if (ground[y2 + 2][x2] == 1) y2 = y2 + 2;
         bdir = EAST;
      } else if (m == bS) {
         x2 = (bx + cornerSize) + (rand() % (bw - 2*cornerSize));
         y2 = by + bh;
         bdir = SOUTH;
      } else if (m == bW) {
         x2 = bx - 1;
         y2 = (by + cornerSize) + (rand() % (bh - 2*cornerSize));

         // Compensate for existing connections to prevent 1-high walls
         if (ground[y2 - 1][x2] == 1) y2 = y2 - 1;
         else if (ground[y2 - 2][x2] == 1) y2 = y2 - 2;
         else if (ground[y2 + 1][x2] == 1) y2 = y2 + 1;
         else if (ground[y2 + 2][x2] == 1) y2 = y2 + 2;
         bdir = WEST;
      }

      int groundCtr = 0;
      if (ground[y2 - 1][x2] == 1) groundCtr++;
      if (ground[y2][x2 + 1] == 1) groundCtr++;
      if (ground[y2 + 1][x2] == 1) groundCtr++;
      if (ground[y2][x2 - 1] == 1) groundCtr++;

      int wallCtr = 0;
      if (ground[y2 - 1][x2] == 2) wallCtr++;
      if (ground[y2][x2 + 1] == 2) wallCtr++;
      if (ground[y2 + 1][x2] == 2) wallCtr++;
      if (ground[y2][x2 - 1] == 2) wallCtr++;
      if ((groundCtr < 2) && (wallCtr < 3)) findingCoords = 6;
   }

   if (findingCoords == 5) return false;

   float pointDistance = FindDistance(x2, y2, x1, y1);
   if (pointDistance >= 25.0f) return false;


   // We've selected our start and end points - time to see if we can connect
   // the two
   // We have three values for every point in the queue, in this order:
   // Y coord, x coord, path rank
   uint8_t queue[1300][3];
   int queuectr = 1;
   int ctr = 1;
   bool running = true;
   queue[0][0] = y2;
   queue[0][1] = x2;
   queue[0][2] = 0;

   int list[4][3];
   int score = 0;
   int scoreIndex = 0;
   bool scoreIndexUpdated = true;

   while (running) {
      queuectr = ctr;
      if (!scoreIndexUpdated) running = false;
      else scoreIndexUpdated = false;

      if (score > 24) running = false;

      // If we've looked at 1200 nodes and still haven't found the endpoint,
      // just start a new path
      if (queuectr >= 1200 || !running) {
         // Go through and reset all ground values to their original state
         ResetMod10Changes();
         return false;
      }

      // Let's make sure we only need to look at the leaves of our tree
      for (int i = scoreIndex; i < queuectr; i++) {
         if (!running) break;
         // Index 0 = NORTH
         list[0][0] = queue[i][0] - 1;
         list[0][1] = queue[i][1];

         // Index 1 = EAST
         list[1][0] = queue[i][0];
         list[1][1] = queue[i][1] + 1;

         // Index 2 = SOUTH
         list[2][0] = queue[i][0] + 1;
         list[2][1] = queue[i][1];

         // Index 3 = WEST
         list[3][0] = queue[i][0];
         list[3][1] = queue[i][1] - 1;

         for (int j = 0; j < 4; j++) {
            // Validate that we aren't trying to access ground coordinates
            // at/beyond the edge of the map
            if ((list[j][0] < 2) || (list[j][1] < 2)) continue;
            if ((list[j][0] > groundSize - 2) ||
            (list[j][1] > groundSize - 2)) continue;

            // Validate that we aren't just running straight back into the
            // first room
            if (PointInRoom(list[j][1], list[j][0], b)) continue;

            // Copy the score from the parent
            list[j][2] = queue[i][2] + 1;

            if ((list[j][0] == y1) && (list[j][1] == x1)) {
               running = false;
            }

            // First, check if it's already in the queue
            bool inQ = false;
            for (int k = 0; k < ctr; k++) {
               if ((queue[k][0] == list[j][0]) &&
               (queue[k][1] == list[j][1])) {
                  if (queue[k][2] > list[j][2]) queue[k][2] = list[j][2];
                  inQ = true;
               }
            }

            if (!inQ) {
               // Next, check if it's our end point (which will be a wall tile)
               // Here, we add the tile to the queue if it isn't already in the
               // queue.
               bool pushToQueue = false;
               if ((list[j][0] == y1) && (list[j][1] == x1)) {
                  pushToQueue = true;
               } else if ((ground[list[j][0]][list[j][1]] == 0) ||
                  (ground[list[j][0]][list[j][1]] == 1)) {
                  // Check if it's void space or ground
                  pushToQueue = true;
               } else if (ground[list[j][0]][list[j][1]] == 3) {
                  // Check for invisible walls, but only allow them if it's one
                  // tile away from an end point
                  if ((FindDistance(list[j][1], list[j][0], x1, y1) <= 1.0f) ||
                  (FindDistance(list[j][1], list[j][0], x1, y1) <= 1.0f)) {
                     pushToQueue = true;
                  }
               }

               // If any of the specifications are met, add this tile to the
               // queue
               if (pushToQueue) {
                  queue[ctr][0] = list[j][0];
                  queue[ctr][1] = list[j][1];
                  queue[ctr][2] = list[j][2];
                  if (queue[ctr][2] > score) {
                     scoreIndex = ctr;
                     score = queue[ctr][2];
                     scoreIndexUpdated = true;
                  }
                  ctr++;
               }
            }
         }
      }
   }

   // Now that we've laid out our path candidates, our best path is the one that
   // takes the fewest tiles and the fewest turns. But before we find that path,
   // we need to draw on our map to help find the way.
   // We draw in multiples of 10 so that we don't overwrite any of the existing
   // data, at least not in base 10 we aren't :)
   for (int i = 0; i < ctr; i++) {
      int y = queue[i][0];
      int x = queue[i][1];
      int s = queue[i][2];
      int orig = ground[y][x] % 10;
      int newval = orig + (10 * (s + 1));
      if (newval < 255) {
         //printf("\n\ty: %i, x: %i, s: %i, newval: %i, current: %i", y, x, s, newval, orig);
         ground[y][x] = newval;
      }
   }

   ground[y1][x1] = 252;

   // Set our cursor at the start point
   int cx = x1;
   int cy = y1;
   int cs = 25;

   // Now we can start backtracing through the grid to find our optimal path
   int lastDir = bdir;
   int path[50][2];
   int pathCtr = 0;
   while (!((cx == x2) && (cy == y2))) {
      cs = (ground[cy][cx] - (ground[cy][cx] % 10)) / 10;

      // Index 0 = NORTH
      list[0][0] = cy - 1;
      list[0][1] = cx;

      // Index 1 = EAST
      list[1][0] = cy;
      list[1][1] = cx + 1;

      // Index 2 = SOUTH
      list[2][0] = cy + 1;
      list[2][1] = cx;

      // Index 3 = WEST
      list[3][0] = cy;
      list[3][1] = cx - 1;

      bool candidates[4];
      int minVal = cs;
      int numCandidates = 0;

      for (int i = 0; i < 4; i++) {
         candidates[i] = false;
         int ly = list[i][0];
         int lx = list[i][1];
         int lv = list[i][2];

         int initVal = ground[ly][lx];
         int pathVal = (initVal - (initVal % 10)) / 10;

         if ((ly < 2) || (lx < 2)) {
            continue;
         } else if ((ly > (groundSize - 3)) || (lx > (groundSize - 3))) {
            continue;
         } else if (initVal < 10) {
            continue;
         } else {
            // Make sure our potential path isn't outside the queue entirely,
            // and also ensure that it's actually closer to the end goal than
            // the cursor is currently.
            if (pathVal <= minVal) {
               // If this is the shortest path, then the previous candidates
               // don't really matter to us since they're longer.
               if (pathVal < minVal) {
                  for (int j = 0; j < i; j++) candidates[j] = false;
                  numCandidates = 0;
                  minVal = pathVal;
               }
               candidates[i] = true;
               numCandidates++;
            }
         }
      }

      if (numCandidates == 0) {
         ResetMod10Changes();
         return false;
      }

      // Path candidates have been found, lets find the best match
      int selectedDir = -1;
      if (candidates[lastDir]) selectedDir = lastDir;
      else {
         int candidatePick = rand() % numCandidates;
         int candidateCtr = 0;
         for (int i = 0; i < 4; i++) {
            if (candidates[i]) {
               if (candidateCtr == candidatePick) {
                  // Find the direction of this path and compare it to the last
                  // known direction of the existing path
                  int dir = 0;

                  // We share a Y coord with the cursor
                  if (list[i][0] == cy) {
                     if (list[i][1] < cx) dir = WEST;
                     else dir = EAST;
                  } else {
                     if (list[i][0] < cy) dir = NORTH;
                     else dir = SOUTH;
                  }

                  selectedDir = dir;
                  break;
               } else {
                  candidateCtr++;
               }
            }
         }
      }
      // Update our last direction followed for the next node
      lastDir = selectedDir;

      // We've picked our path - push the current position to the path and
      // move the cursor further down the path.
      path[pathCtr][0] = cy;
      path[pathCtr][1] = cx;
      cy = list[selectedDir][0];
      cx = list[selectedDir][1];
      pathCtr++;
   }
   path[pathCtr][0] = cy;
   path[pathCtr][1] = cx;
   pathCtr++;

   // We've created our path - time to clean up the extras
   for (int py = 0; py < groundSize; py++) {
      for (int px = 0; px < groundSize; px++) {
         bool inPath = false;
         for (int j = 0; j < pathCtr; j++) {
            if ((py == path[j][0]) && (px == path[j][1])) {
               inPath = true;
               break;
            }
         }

         if (!inPath) ground[py][px] = ground[py][px] % 10;
      }
   }

   // Finally, check our wall heights to make sure that we didn't make short
   // walls
   if (!CheckWallHeights()) {
      // Go through and reset all ground values to their original state
      ResetMod10Changes();
      return false;
   }

   // Return our results
   return true;
}



bool Level::DigCorridor(int a, int b) {
   // Set some simple variables for sanity's sake
   int ax = rooms[a].rect.x;
   int ay = rooms[a].rect.y;
   int aw = rooms[a].rect.w;
   int ah = rooms[a].rect.h;
   int acx = ax + (int)(aw/2);
   int acy = ay + (int)(ah/2);
   int bx = rooms[b].rect.x;
   int by = rooms[b].rect.y;
   int bw = rooms[b].rect.w;
   int bh = rooms[b].rect.h;
   int bcx = bx + (int)(bw/2);
   int bcy = by + (int)(bh/2);

   // Determine which walls are going to be connected
   // This is done by finding the distance between each of the edges and using
   // our shortest distance (largest positive distance, mathematically
   // speaking), and employing the primary side and two secondary sides as
   // available launch points for our path finding algorithm.
   // "A North of B", "A East of B", ...
   int aNb = abs(by - (ay + ah));
   int aEb = abs(ax - (bx + bw));
   int aSb = abs(ay - (by + bh));
   int aWb = abs(bx - (ax + aw));

   // Find the primary and secondary edges for room A
   int m = min(aNb, min(aEb, min(aSb, aWb)));
   int aprimary, asecondary;

   if (m == aEb || m == aWb) {
      if (m == aEb) aprimary = WEST;
      else aprimary = EAST;

      if (aNb < aSb) asecondary = SOUTH;
      else asecondary = NORTH;
   } else {
      if (m == aNb) aprimary = SOUTH;
      else aprimary = NORTH;

      if (aWb < aEb) asecondary = EAST;
      else asecondary = WEST;
   }

   // Find our exact connection points and the best path between them
   // This function will draw on the ground map with multiples of 10 what the
   // optimal path will be so that our digging function can just follow that
   // path.
   if (!FindPath(a, b, aprimary)) {
      if (!FindPath(a, b, asecondary)) return false;
   }

   // Now we check both rooms to make sure we didn't create a fake entrance
   // which can only happen if either room already has one or more connections
   // First, examine room A
   if (rooms[a].connections > 1) {
      for (int cy = ay; cy < ay + ah; cy++) {
         if (!IsWall(ax - 1, cy)) {
            if (!IsGround(ax - 2, cy)) ground[cy][ax - 1] = 2;
         }

         if (!IsWall(ax + aw, cy)) {
            if (!IsGround(ax + aw + 1, cy)) ground[cy][ax + aw] = 2;
         }
      }

      for (int cx = ax; cx < ax + aw; cx++) {
         if (!IsWall(cx, ay - 1)) {
            if (!IsGround(cx, ay - 2)) ground[ay - 1][cx] = 2;
         }

         if (!IsWall(cx, ay + ah)) {
            if (!IsGround(cx, ay + ah + 1)) ground[ay + ah][cx] = 2;
         }
      }
   }

   // Next, room B
   if (rooms[b].connections > 1) {
      for (int cy = by; cy < by + bh; cy++) {
         if (!IsWall(bx - 1, cy)) {
            if (!IsGround(bx - 2, cy)) {
               ground[cy][bx - 1] = 2;
               printf("\nping: %2i, %2i, room: %i", cy, bx - 1, b);
            }
         }

         if (!IsWall(bx + bw, cy)) {
            if (!IsGround(bx + bw + 1, cy)) {
               ground[cy][bx + bw] = 2;
               printf("\nping: %2i, %2i, room: %i", cy, bx + bw, b);
            }
         }
      }

      for (int cx = bx; cx < bx + bw; cx++) {
         if (!IsWall(cx, by - 1)) {
            if (!IsGround(cx, by - 2)) ground[by - 1][cx] = 2;
         }

         if (!IsWall(cx, by + bh)) {
            if (!IsGround(cx, by + bh + 1)) ground[by + bh][cx] = 2;
         }
      }
   }

   // We got a path, time to dig it with brute force
   for (int cY = 0; cY < groundSize; cY++) {
      for (int cX = 0; cX < groundSize; cX++) {
         if (ground[cY][cX] >= 10) ground[cY][cX] = 1;
      }
   }

   return true;
}



bool Level::DigShortCorridor(int x1, int y1, int x2, int y2) {
   int xi = min(x1, x2);
   int xf = max(x1, x2);
   int yi = min(y1, y2);
   int yf = max(y1, y2);

   for (int cy = yi; cy <= yf; cy++) {
      for (int cx = xi; cx <= xf; cx++) {
         int initVal = ground[cy][cx];
         ground[cy][cx] = initVal + 10;
      }
   }

   if (!CheckWallHeights()) {
      ResetMod10Changes();
      return false;
   }

   for (int cy = yi; cy <= yf; cy++) {
      for (int cx = xi; cx <= xf; cx++) {
         ground[cy][cx] = 1;
      }
   }
   return true;
}



bool Level::DigNeighbor(int a, int b) {
   // Set some simple variables for sanity's sake
   int ax = rooms[a].rect.x;
   int ay = rooms[a].rect.y;
   int aw = rooms[a].rect.w;
   int ah = rooms[a].rect.h;
   int bx = rooms[b].rect.x;
   int by = rooms[b].rect.y;
   int bw = rooms[b].rect.w;
   int bh = rooms[b].rect.h;

   // We already know that these two are neighbors
   // First, we must determine which wall they share. We can do this by finding
   // the shortest distance between edges, which is the only positive and
   // non-zero distance between their walls
   int aNSb = by - (ay + ah);
   int bNSa = ay - (by + bh);
   int aEWb = bx - (ax + aw);
   int bEWa = ax - (bx + bw);

   int zi = 0;
   int zl = 0;

   int m = max(aNSb, max(bNSa, max(aEWb, bEWa)));

   if (aNSb == m) { // A is North of B
      // Next, find the length of wall that they share
      for (int cx = ax; cx < ax + aw; cx++) {
         if (bx <= cx && cx <= bx + bw) {
            if (zi == 0) zi = cx;
            else zl++;
         }
      }

      // Determine the exact x coordinate, and dig it
      if (zl > 0) {
         int sharedX = (rand() % zl) + zi;
         ground[by - 1][sharedX] = 1;
         ground[by - 2][sharedX] = 1;

         return true;
      }
      return false;
   } else if (bNSa == m) { // B is North of A
      // Next, find the length of wall that they share
      for (int cx = ax; cx < ax + aw; cx++) {
         if (bx <= cx && cx <= bx + bw) {
            if (zi == 0) zi = cx;
            else zl++;
         }
      }

      if (zl > 0) {
         // Determine the exact x coordinate, and dig it
         int sharedX = (rand() % zl) + zi;
         ground[ay - 1][sharedX] = 1;
         ground[ay - 2][sharedX] = 1;

         return true;
      }
      return false;
   } else if (aEWb == m) { // A is West of B
      // Next, find the length of wall that they share
      for (int cy = ay; cy < ay + ah; cy++) {
         if (by <= cy && cy <= by + bh) {
            if (zi == 0) zi = cy;
            else zl++;
         }
      }

      if (zl > 0) {
         // Determine the exact y coordinate, and dig it
         int sharedY = (rand() % zl) + zi;
         ground[sharedY][bx - 1] = 1;
         ground[sharedY][bx - 2] = 1;

         return true;
      }
      return false;
   } else if (bEWa == m) { // B is West of A
      // Next, find the length of wall that they share
      for (int cy = ay; cy < ay + ah; cy++) {
         if (by <= cy && cy <= by + bh) {
            if (zi == 0) zi = cy;
            else zl++;
         }
      }

      if (zl > 0) {
         // Determine the exact y coordinate, and dig it
         int sharedY = (rand() % zl) + zi;
         ground[sharedY][ax - 1] = 1;
         ground[sharedY][ax - 2] = 1;

         return true;
      }
      return false;
   }

   return false;
}



bool Level::GenerateCorridors() {
   int numCorridors = 0;

   // First, we go through and look for adjacent rooms. If we find such an
   // occurrence, we can just dig a small 2-unit corridor between them
   for (int j = 0; j < roomCount - 1; j++) {
      SDL_Rect a1 = {
         rooms[j].x - 1,
         rooms[j].y - 1,
         rooms[j].w + 2,
         rooms[j].h + 2
      };

      for (int i = j + 1; i < roomCount; i++) {
         // First, we check if these two rooms are already connected to one
         // another. If they are, then we don't need to do anything!
         bool connected = false;
         for (int k = 0; k < rooms[j].connections; k++) {
            if (i == rooms[j].connectedRooms[k]) {
               connected = true;
            }
         }

         if (!connected) {
            SDL_Rect b1 = {
               rooms[i].x - 1,
               rooms[i].y - 1,
               rooms[i].w + 2,
               rooms[i].h + 2
            };

            // First we "swell" the rooms to expand in each direction by one
            // unit. Then, we check for collisions between these "swollen" rooms
            if (IsRectCollision(&a1, &b1)) {
               // If there was a collision, then we can dig a corridor
               // between these rooms!
               if (DigNeighbor(i, j)) {
                  numCorridors++;
                  rooms[i].connectedRooms[rooms[i].connections] = j;
                  rooms[j].connectedRooms[rooms[j].connections] = i;
                  rooms[i].connections++;
                  rooms[j].connections++;
               }
            }
         }
      }
   }

   // For every room, we attempt to make a corridor up to three times. If it
   // still can't make a corridor after three tries, then we come back to it
   // later
   int roomNeighbors[50][3] = {0};
   float shortestDistances[roomCount] = {0.0f};
   float shortestDistancesSorted[roomCount] = {0.0f};
   int roomOrder[roomCount] = {0};
   for (int i = 0; i < roomCount; i++) {
      // Select the destination room by its proximity to the target room
      float neighbors[roomCount];
      float sortedNeighbors[roomCount];
      for (int j = 0; j < roomCount; j++) {
         float d = FindDistance(rooms[i].cX, rooms[i].cY,
            rooms[j].cX, rooms[j].cY);
         if (i == j) d = 99.f;
         neighbors[j] = d;
         sortedNeighbors[j] = d;
      }

      sort(sortedNeighbors, sortedNeighbors + roomCount);

      for (int j = 0; j < roomCount; j++) {
         if (j != i) {
            if (neighbors[j] == sortedNeighbors[0]) roomNeighbors[i][0] = j;
            if (neighbors[j] == sortedNeighbors[1]) roomNeighbors[i][1] = j;
            if (neighbors[j] == sortedNeighbors[2]) roomNeighbors[i][2] = j;
         }
      }

      shortestDistances[i] = roomNeighbors[i][0];
      shortestDistancesSorted[i] = roomNeighbors[i][0];
   }

   sort(shortestDistancesSorted, shortestDistancesSorted + roomCount);

   // Determine our path attempt order by trying to connect the furthest rooms
   // first to give them priority
   for (int i = 0; i < roomCount; i++) {
      for (int j = 0; j < roomCount; j++) {
         if (shortestDistancesSorted[i] == shortestDistances[j]) {
            roomOrder[i] = j;
         }
      }
   }

   // Try to build connections to the 3 nearest rooms
   for (int j = 0; j < 3; j++) {
      for (int i = 0; i < roomCount; i++) {
         int a = roomOrder[i];
         // If our source room is already at max connections, we can skip it
         // and try another one.
         if (rooms[a].connections >= rooms[a].maxConnections) continue;
         else if ((j == 1) && (rooms[a].connections > 0)) continue;
         else {
            // Select the destination room by its proximity to the target room
            int dest = roomNeighbors[a][j];

            if ((j == 3) ||
            (rooms[dest].connections < rooms[dest].maxConnections)) {
               // We have a solid connection ready to be made, potentially
               // First, we check and see if these two rooms are already
               // connected
               bool alreadyConnected = false;
               for (int k = 0; k < rooms[a].connections; k++) {
                  // If our room is already in the list of connections, skip
                  if (dest == rooms[a].connectedRooms[k]) {
                     alreadyConnected = true;
                     break;
                  }

                  // If our room collides with a room that's already connected,
                  // prevent it
                  if (IsRectCollision(&rooms[dest].rect,
                     &rooms[rooms[a].connectedRooms[k]].rect)) {
                     alreadyConnected = true;
                     break;
                  }
               }

               // If they aren't connected already, we can try to build the
               // corridor between the two
               if (!alreadyConnected) {
                  if (DigCorridor(a, dest)) {
                     numCorridors++;
                     rooms[a].connectedRooms[rooms[a].connections] = dest;
                     rooms[dest].connectedRooms[rooms[dest].connections] = a;
                     rooms[a].connections++;
                     rooms[dest].connections++;

                     if (numCorridors == MAXCORRIDORS) {
                        j = 2;
                        i = roomCount;
                     }
                  }
               }
            }
         }
      }
   }

   // Verify that all rooms are reachable from the spawn room
   bool allConnectedToSpawn = false;
   for (int k = 0; k < 10; k++) {
      connectedToSpawn = new bool[roomCount];
      for (int i = 0; i < roomCount; i++) { connectedToSpawn[i] = false; }
      connectedToSpawn[0] = true;
      CheckConnectionsToSpawn(0);

      // If a room isn't connected, then we need to force a connection to a room that is
      bool missingConnection = false;
      bool madeConnection = false;
      for (int i = 0; i < roomCount; i++) {
         if (!connectedToSpawn[i]) {
            missingConnection = true;
            for (int j = 0; j < roomCount; j++) {
               if (!connectedToSpawn[j]) { continue; }
               else if (j == i) { continue; }
               else if (FindDistance(rooms[i].x, rooms[i].y, rooms[j].x, rooms[j].y) <= (groundSize/2)) {
                  if (DigCorridor(i, j)) {
                     madeConnection = true;
                     numCorridors++;
                     rooms[i].connectedRooms[rooms[i].connections] = j;
                     rooms[j].connectedRooms[rooms[j].connections] = i;
                     rooms[i].connections++;
                     rooms[j].connections++;

                     i = roomCount;
                     j = roomCount;
                  }
               }
            }
         }
      }

      if (!missingConnection) {
         allConnectedToSpawn = true;
         break;
      }
      if (madeConnection) k--;
   }

   if (!allConnectedToSpawn) return false;

   //Simple logging to print out the room dimensions and connections to the console
   for (int i = 0; i < roomCount; i++) {
      printf("\n\tRoom %i: \t{%i, %i, %i, %i}:    \t", i, rooms[i].x, rooms[i].y, rooms[i].w, rooms[i].h);
      for (int j = 0; j < rooms[i].connections; j++) {
         printf(" %i,", rooms[i].connectedRooms[j]);
      }
   }

   printf("\nNumber of corridors: %i", numCorridors);
   return true;
}



void Level::CheckConnectionsToSpawn(int iter) {
   for (int i = 0; i < rooms[iter].connections; i++) {
      int dest = rooms[iter].connectedRooms[i];
      if (!connectedToSpawn[dest]) {
         connectedToSpawn[dest] = true;
         CheckConnectionsToSpawn(dest);
      }
   }
}



void Level::GenerateWalls() {
   // Iterates through every tile on the map to find tiles that aren't floor
   // Then it checks if it should be marked as a wall
   for (int cY = 1; cY < groundSize - 1; cY++) {
      for (int cX = 1; cX < groundSize - 1; cX++) {
         if (ground[cY][cX] != 1) {
            bool isWall = false;
            if (IsGround(cX, cY - 1)) { isWall = true; }
            else if (IsGround(cX, cY + 1)) { isWall = true; }
            else if (IsGround(cX - 1, cY)) { isWall = true; }
            else if (IsGround(cX + 1, cY)) { isWall = true; }
            else if (IsGround(cX - 1, cY - 1)) { isWall = true; }
            else if (IsGround(cX - 1, cY + 1)) { isWall = true; }
            else if (IsGround(cX + 1, cY - 1)) { isWall = true; }
            else if (IsGround(cX + 1, cY + 1)) { isWall = true; }

            if (isWall) ground[cY][cX] = 2;
         }
      }
   }

   for (int cY = 1; cY < groundSize - 1; cY++) {
      for (int cX = 1; cX < groundSize - 1; cX++) {
         if (ground[cY][cX] == 0) {
            // If the tile below this is a shown wall and this tile is void,
            // then we want this tile to be an invisible wall
            // Similarly, if the tile above this is a shown wall, this should
            // be an invisible wall to prevent walls that are one unit high
            if (ground[cY + 1][cX] == 2 || ground[cY - 1][cX] == 2) {
               ground[cY][cX] = 3;
            }
         }
      }
   }
}



void Level::FinalizeWalls() {
   int blockSize = PIXELSPERFEET * zoom * 5;

   for (int cY = 1; cY < groundSize - 1; cY++) {
      for (int cX = 1; cX < groundSize - 1; cX++) {
         if (ground[cY][cX] == 2) {
            int key = (cY * 100) + cX;
            int lKey = (cY * 100) + cX - 1;
            int uKey = ((cY - 1) * 100) + cX;

            bool isWallAbove = (walls.find(uKey) != walls.end());
            bool isWallLeft = (walls.find(lKey) != walls.end());

            bool wallEdited = false;
            // First, we check and see if there's a wall above AND a wall to
            // the left of this wall piece. If there is, then it's safe to
            // assume that the two can be combined as long as neither of them
            // are only one unit wide/tall (respectively)
            if (isWallLeft && isWallAbove) {
               SDL_Rect* lC = &walls[lKey].hitBox;
               SDL_Rect* uC = &walls[uKey].hitBox;
               SDL_Rect* lD = &walls[lKey].drawBox;
               SDL_Rect* uD = &walls[uKey].drawBox;
               if (lC->h < blockSize) {
                  Collider dC = walls[uKey];
                  walls.erase(uKey);

                  dC.hitBox.h += blockSize;
                  dC.drawBox.h += blockSize;
                  walls.insert(pair<int, Collider>(key, dC));

                  wallEdited = true;
               } else if (lC->h > blockSize || uC->w > blockSize) {
                  if ((lC->x == uC->x) && ((lC->w + blockSize) == uC->w)) {
                     walls.erase(lKey);

                     uC->h += blockSize;
                     uD->h += blockSize;

                     Collider dC = walls[uKey];
                     walls.erase(uKey);
                     walls.insert(pair<int, Collider>(key, dC));

                     wallEdited = true;
                  } else if ((lC->y == uC->y) && (lC->h == (uC->h + blockSize))) {
                     walls.erase(uKey);

                     lC->w += blockSize;
                     lD->w += blockSize;

                     Collider dC = walls[lKey];
                     walls.erase(lKey);
                     walls.insert(pair<int, Collider>(key, dC));

                     wallEdited = true;
                  }
               }
            }

            if (!wallEdited) {
               // Check if there's an existing wall to the left of this wall
               if (isWallLeft) {
                  // When the height of the wall to the left is more than 1 block tall, and there isn't a wall tile above, then we need to start a new wall instead of continuing the wall to the left
                  Collider dC = walls[lKey];
                  SDL_Rect* lC = &walls[lKey].hitBox;
                  SDL_Rect* uC = &walls[uKey].hitBox;

                  if ((ground[cY-1][cX] == 2) && (lC->h < blockSize)) {
                     // If the tile above is a wall and the collider to the left is a half-wall, we create a new wall here
                     dC = Collider();

                     int y = cY * blockSize;
                     int h = blockSize;

                     dC.SetPos(cX * blockSize, cY * blockSize);
                     dC.SetHitBox(cX * blockSize, y, blockSize, h);
                     dC.SetDrawBox(cX * blockSize, cY * blockSize, blockSize, blockSize);

                     walls.insert(pair<int, Collider>(key, dC));

                     wallEdited = true;
                   } else if ((lC->h >= blockSize) &&
                   (ground[cY-1][cX] == 1)) {
                     // If the wall to the left isn't a half-wall, and the tile above us is ground, we want a brand new block
                     dC = Collider();

                     int y = (cY * blockSize) + (blockSize / 2);
                     int h = blockSize / 2;

                     dC.SetPos(cX * blockSize, cY * blockSize);
                     dC.SetHitBox(cX * blockSize, y, blockSize, h);
                     dC.SetDrawBox(cX * blockSize, cY * blockSize, blockSize, blockSize);

                     walls.insert(pair<int, Collider>(key, dC));

                     wallEdited = true;
                  } else if (!((ground[cY - 1][cX] != 2) &&
                  (dC.hitBox.h > blockSize))) {
                    // If the tile above is a wall or floor and the tile to the left has a height greater than one tile
                    walls.erase(lKey);

                    dC.hitBox.w += blockSize;
                    dC.drawBox.w += blockSize;
                    walls.insert(pair<int, Collider>(key, dC));

                    wallEdited = true;
                 }
               } else if (isWallAbove) {
                  // When the width of the wall above is more than 1 block
                  // wide, and there isn't a wall to the left, we need to start
                  // a new wall instead of continuing the wall above
                  Collider dC = walls[uKey];

                  if (!((ground[cY][cX - 1] != 2) && (dC.hitBox.w > blockSize))) {
                     walls.erase(uKey);

                     dC.hitBox.h += blockSize;
                     dC.drawBox.h += blockSize;
                     walls.insert(pair<int, Collider>(key, dC));

                     wallEdited = true;
                  }
               }
            }

            if (!wallEdited) {
               Collider dC = Collider();

               int y = (cY * blockSize);
               int h = blockSize;
               if (IsGround(cX, cY - 1)) {
                  y += blockSize / 2;
                  h = blockSize / 2;
               }

               dC.SetPos(cX * blockSize, cY * blockSize);
               dC.SetHitBox(cX * blockSize, y, blockSize, h);
               dC.SetDrawBox(cX * blockSize, cY * blockSize, blockSize, blockSize);

               walls.insert(pair<int, Collider>(key, dC));
            }
         }
      }
   }
}



void Level::GenerateDoors() {
   // Lambda function to generate a percentile chance
   auto roll = [](){ return (rand() % 10) > 1; };

   // Coords array is 6 components:
   // 0: Y
   // 1: X
   // 2: width of wall opening
   // 3: direction toward center of room
   // 4: room #
   // 5: hand
   int pcoords[100][6];
   int pCtr = 0;

   // First, we look through each of the rooms to find wall segments that have
   // floor tiles present, or an opening in the wall
   for (int room = 0; room < roomCount; room++) {
      int rx = rooms[room].x;
      int ry = rooms[room].y;
      int rw = rooms[room].w;
      int rh = rooms[room].h;
      int rwHalf = rx + (int)(rw/2);
      int rhHalf = ry + (int)(rh/2);

      int width = 1;
      for (int y = ry; y < ry + rh; y++) {
         if (ground[y][rx + rw] == 1) {
            int hand = (y <= rhHalf) ? 1 : 0;
            if (width > 1) {
               pcoords[pCtr - 1][2] = width;
               width++;
            } else {
               pcoords[pCtr][0] = y;
               pcoords[pCtr][1] = rx + rw;
               pcoords[pCtr][2] = width;
               pcoords[pCtr][3] = WEST;
               pcoords[pCtr][4] = room;
               pcoords[pCtr][5] = hand;
               pCtr++;
            }
            width++;
         } else width = 1;
      }

      width = 1;
      for (int y = ry; y < ry + rh; y++) {
         if (ground[y][rx - 1] == 1) {
            int hand = (y <= rhHalf) ? 1 : 0;
            if (width > 1) {
               pcoords[pCtr - 1][2] = width;
               width++;
            } else {
               pcoords[pCtr][0] = y;
               pcoords[pCtr][1] = rx - 1;
               pcoords[pCtr][2] = width;
               pcoords[pCtr][3] = EAST;
               pcoords[pCtr][4] = room;
               pcoords[pCtr][5] = hand;
               pCtr++;
            }
            width++;
         } else width = 1;
      }

      width = 1;
      for (int x = rx; x < rx + rw; x++) {
         if (ground[ry - 1][x] == 1) {
            int hand = (x <= rwHalf) ? 1 : 0;
            if (width > 1) {
               pcoords[pCtr - 1][2] = width;
               width++;
            } else {
               pcoords[pCtr][0] = ry - 1;
               pcoords[pCtr][1] = x;
               pcoords[pCtr][2] = width;
               pcoords[pCtr][3] = SOUTH;
               pcoords[pCtr][4] = room;
               pcoords[pCtr][5] = hand;
               pCtr++;
            }
            width++;
         } else width = 1;
      }

      width = 1;
      for (int x = rx; x < rx + rw; x++) {
         if (ground[ry + rh][x] == 1) {
            int hand = (x <= rwHalf) ? 1 : 0;
            if (width > 1) {
               pcoords[pCtr - 1][2] = width;
               width++;
            } else {
               pcoords[pCtr][0] = ry + rh;
               pcoords[pCtr][1] = x;
               pcoords[pCtr][2] = width;
               pcoords[pCtr][3] = NORTH;
               pcoords[pCtr][4] = room;
               pcoords[pCtr][5] = hand;
               pCtr++;
            }
            width++;
         } else width = 1;
      }
   }

   // We've found our potential points and their widths, time to root out the
   // ones we dont want
   // 0: Y
   // 1: X
   // 2: direction toward center of room
   // 3: room #
   // 4: hand
   int coords[100][5]; // Y, X, direction toward center of room, room, and hand
   int ctr = 0;
   for (int i = 0; i < pCtr; i++) {
      if (pcoords[i][2] == locale->doorSize) {
         // Make sure we aren't placing two doors immediately next to each other
         bool doorNeighbor = false;
         for (int j = 0; j < ctr; j++) {
            if (coords[j][0] == pcoords[i][0] ||
               coords[j][1] == pcoords[i][1]) {
               if (abs(coords[j][0] - pcoords[i][0]) == 1 ||
                  abs(coords[j][1] - pcoords[i][1]) == 1) {
                  doorNeighbor = true;
                  break;
               }
            }
         }

         if (!doorNeighbor && (rand() % 100) < locale->doorPercentChance) {
            // We want to place a door here
            coords[ctr][0] = pcoords[i][0];
            coords[ctr][1] = pcoords[i][1];
            coords[ctr][2] = pcoords[i][3];
            coords[ctr][3] = pcoords[i][4];
            coords[ctr][4] = pcoords[i][5];
            ctr++;
         }
      }
   }

   // Now we know for sure where we want to place doors at in this level. Time
   // to start creating door objects
   for (int i = 0; i < ctr; i++) {
      int y = coords[i][0];
      int x = coords[i][1];
      int dir = coords[i][2];
      int room = coords[i][3];
      bool hand = coords[i][4]; // false: left-handed, true: right-handed

      doors.push_back(new Door(x, y, locale->doorSize, 1, dir, hand, room,
         locale->defaultDoorFrameW, locale->defaultDoorFrameH, locale));
      ground[y][x] = 16; // Door tiles use 16
      // We reserve a floor tile to make sure the door can be opened
      if (dir == NORTH) ground[y + 1][x] = 5;
      else if (dir == EAST) ground[y][x - 1] = 5;
      else if (dir == SOUTH) ground[y - 1][x] = 5;
      else if (dir == WEST) ground[y][x + 1] = 5;
   }
}



void Level::GenerateLevel() {
   // Start a timer for level generation for optimization
   LTimer timer;
   timer.start();
   floorRender.CreateBlank(PIXELSPERFEET * 5 * (groundSize + 10) * GMAXZOOM,
   PIXELSPERFEET * 5 * groundSize * GMAXZOOM);
   wallRender.CreateBlank(PIXELSPERFEET * 5 * (groundSize + 10) * GMAXZOOM,
   PIXELSPERFEET * 5 * groundSize * GMAXZOOM);

   bool generating = true;
   while (generating) {
      bool levelIsBigEnough = false;
      while (!levelIsBigEnough) {
         NUMROOMCOLLISIONS = 0;
         roomTileCount = 0;
         roomsBuilt = 0;
         delete rooms;
         rooms = new Room[roomCount];
         for (int i = 0; i < groundSize; i++) {
            for (int j = 0; j < groundSize; j++) {
               ground[i][j] = 0;
            }
         }

         while (roomsBuilt < roomCount) {
            GenerateRandomRoom(roomsBuilt);
            roomsBuilt++;
         }

         for (int i = 0; i < roomCount; i++)
            roomTileCount += rooms[i].w * rooms[i].h;

         if (roomTileCount >= desiredRoomTiles) levelIsBigEnough = true;
      }

      spawnRoom = rooms[0].rect;
      exitRoom = rooms[roomCount - 1].rect;
      GenerateWalls();

      // Create corridors between rooms
      if (!GenerateCorridors()) {
         for (int y = 0; y < groundSize; y++) {
            for (int x = 0; x < groundSize; x++) {
               ground[y][x] = 0;
            }
         }
         continue;
      }

      if (!CheckWallHeights()) continue;

      // Create impromptu room entrances from existing halls
      for (int r = 0; r < roomCount; r++) {
         if (rooms[r].connections < rooms[r].maxConnections) {
            int rx = rooms[r].x;
            int ry = rooms[r].y;
            int rw = rooms[r].w;
            int rh = rooms[r].h;

            // Check North and South faces
            bool Nuntouched = true;
            bool Suntouched = true;
            for (int x = rx; x < rx + rw; x++) {
               if (ground[ry - 1][x] == 1) Nuntouched = false;
               if (ground[ry + rh][x] == 1) Suntouched = false;
            }

            // Check East and West faces
            bool Euntouched = true;
            bool Wuntouched = true;
            for (int y = ry; y < ry + rh; y++) {
               if (ground[y][rx - 1] == 1) Wuntouched = false;
               if (ground[y][rx + rw] == 1) Euntouched = false;
            }

            // Go through each side and find if we have any candidates
            bool madeConnection = false;
            int x1, y1, x2, y2 = 0;

            if (Nuntouched) {
               int startx = 0;
               int endx = 0;
               if (ry >= 4) {
                  for (int x = rx; x < rx + rw; x++) {
                     if (ground[ry - 3][x] == 1) {
                        if (!PointInAnyRoom(x, ry - 3)) {
                           if (startx == 0) startx = x;
                           if (endx < x) endx = x;
                        }
                     }
                  }

                  if (startx > 0) {
                     madeConnection = true;
                     y1 = ry;
                     y2 = ry - 3;
                     int mod = (endx == startx) ?
                        0 : (rand() % (endx - startx));
                     x1 = startx + mod;
                     x2 = x1;
                  }
               }
            }

            if (!madeConnection && Euntouched) {
               int starty = 0;
               int endy = 0;
               if (rx + rw + 2 < groundSize) {
                  for (int y = ry; y < ry + rh; y++) {
                     if (ground[y][rx + rw + 2] == 1) {
                        if (!PointInAnyRoom(rx + rw + 2, ry)) {
                           if (starty == 0) starty = y;
                           if (endy < y) endy = y;
                        }
                     }
                  }

                  if (starty > 0) {
                     madeConnection = true;
                     x1 = rx + rw;
                     x2 = rx + rw + 2;
                     int mod = (endy == starty) ?
                        0 : (rand() % (endy - starty));
                     y1 = starty + mod;
                     y2 = y1;
                  }
               }
            }

            if (!madeConnection && Suntouched) {
               int startx = 0;
               int endx = 0;
               if (ry + rh + 3 < groundSize) {
                  for (int x = rx; x < rx + rw; x++) {
                     if (ground[ry + rh + 3][x] == 1) {
                        if (!PointInAnyRoom(x, ry + rh + 3)) {
                           if (startx == 0) startx = x;
                           if (endx < x) endx = x;
                        }
                     }
                  }

                  if (startx > 0) {
                     madeConnection = true;
                     y1 = ry + rh;
                     y2 = ry + rh + 3;
                     int mod = (endx == startx) ?
                        0 : (rand() % (endx - startx));
                     x1 = startx + mod;
                     x2 = x1;
                  }
               }
            }

            if (!madeConnection && Wuntouched) {
               int starty = 0;
               int endy = 0;
               if (rx >= 4) {
                  for (int y = ry; y < ry + rh; y++) {
                     if (ground[y][rx - 2] == 1) {
                        if (!PointInAnyRoom(rx - 2, ry)) {
                           if (starty == 0) starty = y;
                           if (endy < y) endy = y;
                        }
                     }
                  }

                  if (starty > 0) {
                     madeConnection = true;
                     x1 = rx;
                     x2 = rx - 2;
                     int mod = (endy == starty) ?
                        0 : (rand() % (endy - starty));
                     y1 = starty + mod;
                     y2 = y1;
                  }
               }
            }

            if (madeConnection) {
               DigShortCorridor(x1, y1, x2, y2);
            }
         }
      }

      generating = false;
   }
   // Build walls around corridors
   GenerateWalls();

   // Finalize walls
   FinalizeWalls();

   // Plant secrets throughout the level

   // Set secrets in rooms

   // Block off secret corridors

   // Scan the floorplan and build locales

   // Generate doors
   GenerateDoors();
   for (auto d : doors) {
      d->SetFunction(RIGHTCLICK, [d](int a, int b, int c) {
         d->OpenDropMenu(a, b);
         return true;
      });
   }

   // Give large rooms a chance to have big pits in them (no more than 60% of
   // the room). This chance should be tied to the locale of the level

   printf("\nFinal tile count: %i", roomTileCount);
   printf("\nMax collisions: %i, total: %i",
      NUMROOMCOLLISIONSALLOWED, NUMROOMCOLLISIONS);

   printf("\nLevel generation time: %4i ms", timer.getTicks());
}



bool Level::LoadFromFile(string path, int width, int height) {
   return false;
}



void Level::SetZoom(float newZoom) {
   zoom = newZoom * GZOOM;

   renderTargetsCompiled = false;
}



SDL_Rect Level::GetPlayerSpawn() {
   int gx = (rooms[0].x + (rand() % rooms[0].w));
   int gy = (rooms[0].y + (rand() % rooms[0].h));
   int x = gx * PIXELSPERFEET * 5 * GZOOM;
   int y = gy * PIXELSPERFEET * 5 * GZOOM;

   printf("\nSet spawn point: %i %i -> %i %i", gx, gy, x, y);

   SDL_Rect r = {x, y, 0, 0};
   return r;
}



void Level::WriteOutWholeLevel() {
   ofstream myFile;
   myFile.open("output.txt");
   for (int yi = 0; yi < groundSize; yi++) {
      for (int xi = 0; xi < groundSize; xi++) {
         string o = "  ";
         switch (ground[yi][xi]) {
            case 1: o = "::"; break;
            case 2: o = "##"; break;
            default: o = "  ";
         }

         myFile << o;
      }
      myFile << "\n";
   }

   myFile.close();
}



void Level::CompileRenderTargets(int xQ, int yQ) {
   // Accepts args x and y if we want to only render out a specific tile
   int iterYStart = 0;
   int iterXStart = 0;
   int iterYMax = groundSize;
   int iterXMax = groundSize;
   if (xQ > 0 || yQ > 0) {
      iterYStart = yQ;
      iterXStart = xQ;
      iterYMax = yQ + 1;
      iterXMax = xQ + 1;
   }

   int floorTextureSize = locale->floorTextureSize * zoom;
   int voidTextureSize = locale->voidTextureSize * zoom;
   int tileW = PIXELSPERFEET * 5 * zoom;
   int wallHeight = locale->wallHeight * zoom;

   // First, we paint the repeating texture of the floor everywhere
   SDL_SetRenderTarget(gRenderer, floorRender.mTexture);
   for (int yi = 0; yi < ceil(floorRender.GetHeight()/floorTextureSize); yi++) {
      for (int xi = 0; xi < ceil(floorRender.GetWidth()/floorTextureSize);
      xi++) {
         SDL_Rect r = {
            floorTextureSize * xi,
            floorTextureSize * yi,
            min(floorTextureSize,
               abs((floorTextureSize * (xi)) - floorRender.GetWidth())),
            min(floorTextureSize,
               abs((floorTextureSize * (yi)) - floorRender.GetHeight()))
         };
         locale->floorTexture.Render(&r);
      }
   }

   // Next, we initialize the void texture (OOB), and render the void onto the
   // walls layer
   SDL_SetRenderTarget(gRenderer, wallRender.mTexture);
   for (int yi = iterYStart; yi < iterYMax; yi++) {
      for (int xi = iterXStart; xi < iterXMax; xi++) {
         if ((ground[yi][xi] == 0) || (ground[yi][xi] == 2) ||
         (ground[yi][xi] == 3)) {
            SDL_Rect r = {
               tileW * xi,
               tileW * yi,
               tileW,
               tileW
            };

            SDL_Rect c = {
               (int) (((xi * tileW) % voidTextureSize)/zoom),
               (int) (((yi * tileW) % voidTextureSize)/zoom),
               (int) (tileW/zoom),
               (int) (tileW/zoom)
            };

            locale->voidTexture.Render(&r, &c);
         }
      }
   }

   // Blit the repeating textures to the appropriate buffer textures so that we
   // can render one large texture instead of 300 small textures every frame
   for (int yi = iterYStart; yi < iterYMax; yi++) {
      for (int xi = iterXStart; xi < iterXMax; xi++) {
         if (ground[yi][xi] == 2) {
            SDL_Rect r = {
               tileW * xi,
               tileW * yi,
               tileW,
               tileW
            };

            SDL_Rect rF = {
               tileW * xi,
               (tileW * yi) - (wallHeight - tileW),
               tileW,
               wallHeight
            };

            SDL_Rect c = {
               0,
               wallHeight - tileW,
               tileW,
               wallHeight
            };

            // If the tile beneath this tile is floor, then we render this tile
            // as the "face" of the wall
            if (IsGround(xi, yi + 1)) {
               // render the face wall
               int q = (yi + xi) % 3;

               switch (q) {
                  case 0:
                     locale->wallTexture_Face1.Render(&rF, &c);
                     break;
                  case 1:
                     locale->wallTexture_Face2.Render(&rF, &c);
                     break;
                  case 2:
                     locale->wallTexture_Face3.Render(&rF, &c);
                     break;
               }
            } else {
               bool drewLeft = false;
               bool drewRight = false;

               // If the tile to our left is a floor tile, then this tile has
               // the left edge of a wall
               if (IsGround(xi - 1, yi) ||
               (IsWall(xi - 1, yi) && IsGround(xi - 1, yi + 1))) {
                  // Render the left edge
                  locale->wallTexture_LEdge.Render(&r);
                  drewLeft = true;
               }

               // If the tile to our right is a floor tile, then this tile has
               // the right edge of a wall
               if (IsGround(xi + 1, yi) ||
               (IsWall(xi + 1, yi) && IsGround(xi + 1, yi + 1))) {
                  // Render the right edge
                  locale->wallTexture_REdge.Render(&r);
                  drewRight = true;
               }

               // If the tile above this tile is a floor tile, then this tile
               // has the back edge of a wall
               if (IsGround(xi, yi - 1)) {
                  // render the back edge of wall
                  locale->wallTexture_TEdge.Render(&r);
                  drewLeft = true;
                  drewRight = true;
               }



               // If the tile to our diagonal is a floor tile, then we need to
               // provide a corner piece to its surrounding walls
               if (IsGround(xi - 1, yi - 1) && !drewLeft) {
                  // Render top-left corner
                  locale->wallTexture_LCorner.Render(&r);
               }
               if (IsGround(xi + 1, yi - 1) && !drewRight) {
                  // Render top-right corner
                  locale->wallTexture_RCorner.Render(&r);
               }
               if (IsGround(xi + 1, yi + 1) && !drewRight) {
                  // Render bottom-right corner
                  locale->wallTexture_REdge.Render(&r);
               }
               if (IsGround(xi - 1, yi + 1) && !drewLeft) {
                  // Render bottom-left corner
                  locale->wallTexture_LEdge.Render(&r);
               }
            }
         }
      }
   }

   SDL_SetRenderTarget(gRenderer, NULL);

   renderTargetsCompiled = true;
}



void Level::RenderFloor() {
   if (!renderTargetsCompiled) { CompileRenderTargets(); }
   floorRender.Render(-Camera::x, -Camera::y);
}



void Level::RenderWalls(int yO, int yF) {
   // yO is the pixel Y position of our current actor
   // yF, if positive, is the pixel Y position of the next actor
   int tileW = PIXELSPERFEET * 5 * zoom;

   int yOT = (int) floor(yO/tileW);
   int yFT = (yF == -1) ? (groundSize - 1) : ((int) floor(yF/tileW));

   if (yF == -1) {
      yF = groundSize * tileW;
   }

   yO = yOT * tileW;
   yF = yFT * tileW;

   SDL_Rect wallChunk = {
      0,
      yO,
      wallRender.GetWidth(),
      yF - yO
   };

   SDL_Rect drawBox = {
      -(int)Camera::x,
      yO - (int)Camera::y,
      wallRender.GetWidth(),
      yF - yO
   };

   wallRender.Render(&drawBox, &wallChunk);

   // Paint collided walls
   // SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 63);
   // LTexture colliderDraw;
   // colliderDraw.CreateBlank(wallRender.mWidth, wallRender.mHeight);
   // SDL_SetRenderTarget(gRenderer, colliderDraw.mTexture);
   // for (auto i=walls.begin(); i != walls.end(); i++) {
   //    int x = (i->second.hitBox.x) + 5;
   //    int y = (i->second.hitBox.y) + 5;
   //    int w = (i->second.hitBox.w) - 10;
   //    int h = (i->second.hitBox.h) - 10;
   //
   //    SDL_Rect r = {x, y, w, h};
   //    SDL_RenderFillRect(gRenderer, &r);
   // }
   // SDL_SetRenderTarget(gRenderer, wallRender.mTexture);
   // SDL_Rect db = {0, 0, wallRender.mWidth, wallRender.mHeight};
   // colliderDraw.Render(&db);
   //
   // SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
   // SDL_SetRenderTarget(gRenderer, NULL);
}



void Level::RenderDoors(int yO, int yF) {
   int tileW = PIXELSPERFEET * 5 * zoom;

   int yOT = (int) floor(yO/tileW);
   int yFT = (yF == -1) ? (groundSize - 1) : ((int) floor(yF/tileW));

   if (yF == -1) {
      yF = groundSize * tileW;
   }

   yO = yOT * tileW;
   yF = yFT * tileW;

   for (int i = 0; i < doors.size(); i++) {
      int y = (int)doors[i]->drawCutoff;
      if (y >= yO && y <= yF) doors[i]->Render();
   }
}



Level::~Level() {
   for (auto door : doors) delete door;
   delete[] rooms;
   delete[] connectedToSpawn;
}
