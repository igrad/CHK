#include "..\include\Level.h"

enum DIRECTION { NORTH, EAST, SOUTH, WEST };
int NUMROOMCOLLISIONS = 0;
int NUMROOMCOLLISIONSALLOWED = 0;
int MAXCORRIDORS = 0;

Locale::Locale() {
   int mapSize = 100;
   LTexture floorTexture = LTexture();
   LTexture wallTexture = LTexture();
}



Room::Room() {
   x, y, w, h, cX, cY, connections, maxConnections = 0;
   rect = {0, 0, 0, 0};
   connections = 0;
   connectedRooms = new int[20];
   maxConnections = (rand() % 2) + 1;
}



Room::~Room() {
   delete connectedRooms;
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

            printf("\nGenerated Room %i: \t{%i, %i, %i, %i}:    \t", roomNum,
            rooms[roomNum].x, rooms[roomNum].y,
            rooms[roomNum].w, rooms[roomNum].h);

            // Go through all existing rooms and check if this room collides with any. If it does, we need to increase the connection counter
            for (int j = 0; j < roomsBuilt; j++) {
               if (IsRectCollision(&room, &rooms[j].rect)) {
                  //printf("\n[Room collision during generation] Adding connection from %i to %i", roomNum, j);
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
         if (ground[cY][cX] == 0) {
            if ((ground[cY - 1][cX] == 1) && (ground[cY + 1][cX] == 1)) {
               return false;
            }
         }
      }
   }
   return true;
}



bool Level::DigCorridor(int a, int b) {
   // Build an array to store all of the coordinates that we will temporarily
   // change on the map
   int pathCtr = 0;
   int path[50][3];

   // Set some simple variables for sanity's sake
   int ax = rooms[a].rect.x;
   int ay = rooms[a].rect.y;
   int aw = rooms[a].rect.w;
   int ah = rooms[a].rect.h;
   int bx = rooms[b].rect.x;
   int by = rooms[b].rect.y;
   int bw = rooms[b].rect.w;
   int bh = rooms[b].rect.h;

   // Find the X and Y distances between our two chosen rooms
   int dX = abs(rooms[a].cX - rooms[b].cX);
   int dY = abs(rooms[a].cY - rooms[b].cY);

   // Find the delta between room edges
   int aNSb = by - (ay + ah);
   int bNSa = ay - (by + bh);
   int aEWb = bx - (ax + aw);
   int bEWa = ax - (bx + bw);

   // Set the primary direction for our dig
   int dir = 0;

   if (aNSb > 0) { dir = SOUTH; }
   else if (bNSa > 0) { dir = NORTH; }
   else if (aEWb > 0) { dir = WEST; }
   else if (bEWa > 0) { dir = EAST; }

   // Set our exact destination point
   int cAX = 0;
   int cAY = 0;
   int cBX = 0;
   int cBY = 0;

   switch (dir) {
      case 0: {
         cAX = ax + (rand() % (aw - 1));
         cAY = ay;
         cBX = bx + (rand() % (bw - 1));
         cBY = by + bh;
         break;
      }
      case 1: {
         cAX = ax + (rand() % (aw - 1));
         cAY = ay + ah;
         cBX = bx + (rand() % (bw - 1));
         cBY = by;
         break;
      }
      case 2: {
         cAX = ax;
         cAY = ay + (rand() % (ah - 1));
         cBX = bx + bw;
         cBY = by + (rand() % (bh - 1));
         break;
      }
      case 3: {
         cAX = ax + aw;
         cAY = ay + (rand() % (ah - 1));
         cBX = bx + bw;
         cBY = by + (rand() % (bh - 1));
         break;
      }
   }

   // Initialize our dig cursor and the accompanying distance params
   int xDist = abs(cAX - cBX);
   int yDist = abs(cAY - cBY);
   int cX = cAX;
   int cY = cAY;

   bool pathing = true;

   // Time to start digging
   if (corridorType == CORRIDOR_DIRECT) {
      // Not implemented yet
   } else if (corridorType == CORRIDOR_CORNERS) {
      int lastDir = NORTH;
      bool lastDirSet = false;
      while (pathing) {
         xDist = abs(cX - cBX);
         yDist = abs(cY - cBY);
         int remainingDistance = (int) sqrt(pow(xDist, 2) + pow(yDist, 2));

         int segmentLength = (rand() % 9) + 4;
         int dir = NORTH;

         if (xDist > yDist) {
            if (cX > cBX) { dir = WEST; }
            else { dir = EAST; }
         } else {
            if (cY > cBY) { dir = NORTH; }
            else { dir = SOUTH; }
         }

         // If we're a short distance away, let's just get to the finish line
         // and not do anything special or random
         if (remainingDistance <= (2 * segmentLength)) {
            if (cX == cBX) { segmentLength = yDist; }
            else if (cY == cBY) { segmentLength = xDist; }
            else {
               if ((dir == NORTH) || (dir == SOUTH)) { segmentLength = yDist; }
               else { segmentLength = xDist; }
            }
         } else {
            // If we're still far enough away from our end point, let's put a
            // spin on the formula
            if (lastDirSet && ((rand() % 5) == 4)) { dir == lastDir; }
            lastDir = dir;
            lastDirSet = true;
         }

         if (!CheckWallHeights()) {
            for (int i = 0; i < pathCtr; i++) {
               ground[path[i][0]][path[i][1]] = path[i][2];
            }
            return false;
         } else {
            path[pathCtr][0] = cY;
            path[pathCtr][1] = cX;
            path[pathCtr][2] = ground[cY][cX];
            pathCtr++;
            ground[cY][cX] = 1;

            for (int i = 0; i < segmentLength; i++) {
               switch(dir) {
                  case NORTH:
                     if (cY - 1 < 1) { i = segmentLength; }
                     else { cY--; }
                     break;
                  case EAST:
                     if (cX + 1 > groundSize - 2) { i = segmentLength; }
                     else { cX++; }
                     break;
                  case SOUTH:
                     if (cY + 1 > groundSize - 2) { i = segmentLength; }
                     else { cY++; }
                     break;
                  case WEST:
                     if (cX - 1 < 1) { i = segmentLength; }
                     else { cX--; }
                     break;
               }

               // If this block is already true, it's probably in another
               // corridor or in a room. If it's in a room, then it needs to
               // count as a connection.
               if (ground[cY][cX] == 1) {
                  for (int j = 0; j < roomCount; j++) {
                     if (j == a) { continue; }
                     else if (PointInRect(cX, cY, &rooms[j].rect)) {
                        if (j == b) {
                           pathing = false;
                           i == segmentLength;
                           break;
                        } else {
                           bool skipAConnection = false;
                           for (int k = 0; k < rooms[a].connections; k++) {
                              if (rooms[a].connectedRooms[k] == j) {
                                 skipAConnection = true;
                                 break;
                              }
                           }

                           if (!skipAConnection) {
                              rooms[a].connectedRooms[rooms[a].connections] = j;
                              rooms[a].connections++;

                              rooms[j].connectedRooms[rooms[j].connections] = a;
                              rooms[j].connections++;
                           }

                           bool skipBConnection = false;
                           for (int k = 0; k < rooms[b].connections; k++) {
                              if (rooms[b].connectedRooms[k] == j) {
                                 skipBConnection = true;
                                 break;
                              }
                           }

                           if (!skipBConnection) {
                              rooms[b].connectedRooms[rooms[b].connections] = j;
                              rooms[b].connections++;

                              rooms[j].connectedRooms[rooms[j].connections] = b;
                              rooms[j].connections++;
                           }
                        }
                     }
                  }
               } else {
                  ground[cY][cX] = 1;
               }

               if ((cX == cBX) && (cY == cBY)) {
                  pathing = false;
                  break;
               }
            }
         }
      }
   }

   // Check for wall heights
   if (!CheckWallHeights()) {
      for (int i = 0; i < pathCtr; i++) {
         ground[path[i][0]][path[i][1]] = path[i][2];
      }
      return false;
   } else return true;
}



void Level::FindPath(int a, int b, int adir, SDL_Rect* r) {
   Log("Finding path");
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

   if (adir == NORTH) {
      x1 = (ax + cornerSize) + (rand() % (aw - cornerSize));
      y1 = ay - 1;
   } else if (adir == EAST) {
      x1 = ax + aw;
      y1 = (ay + cornerSize) + (rand() % (ah - cornerSize));
   } else if (adir == SOUTH) {
      x1 = (ax + cornerSize) + (rand() % (aw - cornerSize));
      y1 = ay + ah;
   } else if (adir == WEST) {
      x1 = ax - 1;
      y1 = (ay + cornerSize) + (rand() % (ah - cornerSize));
   }

   // Now that we have our start point, we need to find which edge of B is
   // closest to that point to make the shortest path
   int bNSx = bx + (int)(bw/2);
   int bEWy = by + (int)(bh/2);

   int bN = (int)(FindDistance(bNSx, by - 1, x1, y1));
   int bE = (int)(FindDistance(bx + bw, bEWy, x1, y1));
   int bS = (int)(FindDistance(bNSx, by + bh, x1, y1));
   int bW = (int)(FindDistance(bx - 1, bEWy, x1, y1));

   int m = min(bN, min(bE, min(bS, bW)));

   if (m == bN) {
      x2 = (bx + cornerSize) + (rand() % (bw - cornerSize));
      y2 = (by - 1);
      bdir = NORTH;
   } else if (m == bE) {
      x2 = bx + bw;
      y2 = (by + cornerSize) + (rand() % (bh - cornerSize));
      bdir = EAST;
   } else if (m == bS) {
      x2 = (bx + cornerSize) + (rand() % (bw - cornerSize));
      y2 = by + bh;
      bdir = SOUTH;
   } else if (m == bW) {
      x2 = bx - 1;
      y2 = (by + cornerSize) + (rand() % (bh - cornerSize));
      bdir = WEST;
   }

   // Validate that we have two good points to work with
   // First, we have to check if either point has more than one ground tile
   // adjacent to it
   int groundCtr = 0;
   if (ground[y1 - 1][x1] == 1) groundCtr++;
   if (ground[y1][x1 + 1] == 1) groundCtr++;
   if (ground[y1 + 1][x1] == 1) groundCtr++;
   if (ground[y1][x1 - 1] == 1) groundCtr++;
   if (groundCtr != 1) return;

   groundCtr = 0;
   if (ground[y2 - 1][x2] == 1) groundCtr++;
   if (ground[y2][x2 + 1] == 1) groundCtr++;
   if (ground[y2 + 1][x2] == 1) groundCtr++;
   if (ground[y2][x2 - 1] == 1) groundCtr++;
   if (groundCtr != 1) return;

   // Next, we validate that there aren't three or more wall tiles adjacent to
   // the end points
   int wallCtr = 0;
   if (ground[y1 - 1][x1] == 1) wallCtr++;
   if (ground[y1][x1 + 1] == 1) wallCtr++;
   if (ground[y1 + 1][x1] == 1) wallCtr++;
   if (ground[y1][x1 - 1] == 1) wallCtr++;
   if (wallCtr > 2) return;

   wallCtr = 0;
   if (ground[y2 - 1][x2] == 1) wallCtr++;
   if (ground[y2][x2 + 1] == 1) wallCtr++;
   if (ground[y2 + 1][x2] == 1) wallCtr++;
   if (ground[y2][x2 - 1] == 1) wallCtr++;
   if (wallCtr > 2) return;

   // No issues so far means we've successfully selected two points!
   Log("Selected start and end points");

   ofstream myFile;
   myFile.open("p1.txt");
   char buff[10];
   char buf[2];
   snprintf(buff, 10, "x1: %2i\t", x1);
   myFile << buff;
   snprintf(buff, 10, "y1: %2i\n", y1);
   myFile << buff;
   snprintf(buff, 10, "x2: %2i\t", x2);
   myFile << buff;
   snprintf(buff, 10, "y2: %2i\n", y2);
   myFile << buff;

   for (int py = min(y1,y2) - 3; py < max(y1,y2) + 3; py++) {
      for (int px = min(x1,x2) - 3; px < max(x1,x2) + 3; px++) {
         buf[0] = ' ';
         buf[1] = ' ';
         if ((px == x1) && (py == y1)) snprintf(buf, 3, "$$");
         else if ((px == x2) && (py == y2)) snprintf(buf, 3, "@@");
         else {
            if (ground[py][px] == 2) snprintf(buf, 3, "||", ground[py][px]);
            else if (ground[py][px] == 1) snprintf(buf, 3, "::", ground[py][px]);
            else snprintf(buf, 3, "%2i", ground[py][px]);
         }

         myFile << buf;
      }
      myFile << "\n";
   }
   myFile.close();

   // We've selected our start and end points - time to see if we can connect
   // the two
   // Set our cursor at the end point
   // We have three values for every point in the queue, in this order:
   // Y coord, x coord, path rank
   int queue[300][3];
   int queuectr = 1;
   int ctr = 1;
   bool running = true;
   printf("\nSetting queue[0] vals: %i, %i, %i", y1, x1, adir);
   queue[0][0] = y2;
   queue[0][1] = x2;
   queue[0][2] = 0;

   int list[4][3];
   int minPathLength = 99;
   int numPaths = 0;

   Log("Running search");
   while (running) {
      queuectr = ctr;
      // If we've looked at 200 nodes and still haven't found the endpoint,
      // just start a new path
      if (queuectr >= 200) {
         // Go through and reset all ground values to their original state
         for (int cy = 0; cy < groundSize; cy++) {
            for (int cx = 0; cx < groundSize; cx++) {
               ground[cy][cx] = ground[cy][cx] % 10;
            }
         }
         return;
      }

      for (int i = 0; i < queuectr; i++) {
         printf("\nRunning: ctr = %i", ctr);
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

         Log("Set list");
         for (int j = 0; j < 4; j++) {
            // Validate that we aren't trying to access ground coordinates
            // at/beyond the edge of the map
            if ((list[j][0] < 2) || (list[j][1] < 2)) continue;

            // Copy the score from the parent
            list[j][2] = queue[i][2] + 1;

            // First, check if it's already in the queue
            bool inQ = false;
            for (int k = 0; k < ctr; k++) {
               if ((queue[k][0] == list[j][0]) &&
               (queue[k][1] == list[j][1])) {
                  if (queue[k][2] > list[j][2]) queue[k][2] = list[j][2];
                  inQ = true;
               }
            }
            Log("Found inclusion in queue");

            // Next, check if it's our end point (which will be a wall tile)
            // Here we determine if the pathing function is ready to move on to
            // the next phase.
            if ((list[j][0] == y1) && (list[j][1] == x1)) {
               if (list[j][2] < minPathLength) {
                  minPathLength = list[j][2];
               }

               if ((list[j][2] > minPathLength) || (numPaths == 9)) {
                  // If we've reached a number of paths where the path length
                  // is greater than the minimum path length, it's time to
                  // end
                  running = false;
               } else {
                  numPaths++;
               }
            }

            Log("Found shortest path");
            if (!inQ) {
               Log("In queue");
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
                  int d = (int)(ceil(FindDistance(list[j][1], list[j][0], x1, y1)));
                  printf("\nd: %i", d);
                  if ((int)(ceil(
                     FindDistance(list[j][1], list[j][0], x1, y1))) == 1) {
                     pushToQueue = true;
                  } else if ((int)(ceil(
                     FindDistance(list[j][1], list[j][0], x2, y2))) == 1) {
                     pushToQueue = true;
                  }
               }

               Log("Checking push to queue");

               // If any of the specifications are met, add this tile to the
               // queue
               if (pushToQueue) {
                  queue[ctr][0] = list[j][0];
                  queue[ctr][1] = list[j][1];
                  queue[ctr][2] = list[j][2];
                  ctr++;
               }
            }
            else {Log("Not in queue");}
         }
      }
   }

   // Now that we've laid out our path candidates, our best path is the one that
   // takes the fewest tiles and the fewest turns. But before we find that path,
   // we need to draw on our map to help find the way.
   // We draw in multiples of 10 so that we don't overwrite any of the existing
   // data, at least not in base 10 we aren't :)
   Log("setting ground values");
   for (int i = 0; i < ctr; i++) {
      ground[queue[i][0]][queue[i][1]] = ground[queue[i][0]][queue[i][1]] +
      (10 * (queue[i][2] + 1));
   }

   // NOTE: Temporary chunk to print to a file to see what our pathfinding
   // algorithm is doing
   myFile.open("p2.txt");
   for (int py = min(y1,y2) - 3; py < max(y1,y2) + 3; py++) {
      for (int px = min(x1,x2) - 3; px < max(x1,x2) + 3; px++) {
         buf[0] = ' ';
         buf[1] = ' ';
         if ((px == x1) && (py == y1)) snprintf(buf, 3, "$$");
         else if ((px == x2) && (py == y2)) snprintf(buf, 3, "@@");
         else {
            if (ground[py][px] == 2) snprintf(buf, 3, "||", ground[py][px]);
            else if (ground[py][px] == 1) snprintf(buf, 3, "::", ground[py][px]);
            else snprintf(buf, 3, "%2i", ground[py][px]);
         }

         myFile << buf;
      }
      myFile << "\n";
   }
   myFile.close();

   // Set our cursor at the start point
   int cx = x1;
   int cy = y1;
   int cs = 99;

   // Now we can start backtracing through the grid to find our optimal path
   int lastDir = bdir;
   int path[50][2];
   int pathCtr = 0;
   Log("Backtracing");
   while ((cx != x1) && (cy != y1)) {
      cs = (int)((ground[cy][cx] - (ground[cy][cx] % 10)) / 10);

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
      int minVal = 99;
      int numCandidates = 0;

      for (int i = 0; i < 4; i++) {
         int ly = list[i][0];
         int lx = list[i][1];
         int lv = list[i][2];

         // If we have reached the end point, be sure to mark it as our selected
         // candidate and our only candidate.
         if ((ly == y2) && (lx == x2)) {
            candidates[0] = false;
            candidates[1] = false;
            candidates[2] = false;
            candidates[3] = false;

            candidates[i] = true;
            numCandidates = 1;
            break;
         }
         int initVal = ground[ly][lx];
         int pathVal = (int)((initVal - (initVal % 10)) / 10);

         if (initVal < 10) pathVal = 99;

         // Make sure our potential path isn't outside the queue entirely, and
         // also ensure that it's actually closer to the end goal than the
         // cursor is currently.
         if ((pathVal < cs) && (pathVal <= minVal)) {
            // If this is the shortest path, then the previous candidates
            // don't really matter to us since they're longer.
            if (pathVal < minVal) {
               for (int j = 0; j < i; j++) candidates[i] = false;
               numCandidates = 0;
               minVal = pathVal;
            }
            candidates[i] = true;
            numCandidates++;
         }
      }

      // Path candidates have been found, lets find the best match
      Log("Starting candidate selection");
      printf("\nminval: %i, numCandidates: %i", minVal, numCandidates);
      int selectedDir = NORTH;
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
                  break;
               } else {
                  candidateCtr++;
               }
            }
         }
      }

      Log("Set dir");
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

   // We've created our path - time to clean up the extras
   Log("Path created - cleaning up");
   for (int i = 0; i < ctr; i++) {
      int y = queue[i][0];
      int x = queue[i][1];
      int val = queue[i][2];
      bool inPath = false;

      for (int j = 0; j < pathCtr; j++) {
         if ((y == path[j][0]) && (x == path[j][1])) {
            inPath = true;
            break;
         }
      }

      if (!inPath) ground[y][x] = ground[y][x] % 10;
   }

   // Return our results
   Log("Returning results");
   *r = {x1, y1, x2, y2};
}



bool Level::DigCorridor2(int a, int b) {
   // We're just going to completely redo this method because it's a total mess
   // 1. We're only going to dig to a nearby room, not one on the other
   //    side of the map, because that causes nothing but problems, and isn't
   //    very logical (who adds rooms in the middle of a compound after digging
   //    out a super long hallway?).
   //    - Room selection needs to be done in GenerateCorridors
   // 2. We're going to prevent corridors from overwriting long segments of
   //    walls, specifically no more than two wall overrides per corridor. This
   //    should help keep corridors more direct and more logical as well.
   // 3. We're going to properly implement the side selection so that we
   //    don't have corridors that spawn on the wrong side of the room and then
   //    instantly travel through the same room that they spawned at. It's ugly
   //    and useless.
   // 4. We're going to prevent corridors from colliding with other rooms. We
   //    have previously set a limit on how many corridors should be created,
   //    and how many corridors should connect to each room. Leaving in an easy
   //    way to bypass those restrictions makes those restrictions pointless.

   // Set some simple variables for sanity's sake
   int ax = rooms[a].rect.x;
   int ay = rooms[a].rect.y;
   int aw = rooms[a].rect.w;
   int ah = rooms[a].rect.h;
   int bx = rooms[b].rect.x;
   int by = rooms[b].rect.y;
   int bw = rooms[b].rect.w;
   int bh = rooms[b].rect.h;

   // Determine which walls are going to be connected
   // This is done by finding the distance between each of the edges and using
   // our shortest distance (largest positive distance, mathematically
   // speaking), and employ the primary side and two secondary sides as
   // available launch points for our path finding algorithm.
   // "A North of B", "A East of B", ...
   int aNb = by - (ay + ah);
   int aEb = bx - (ax + aw);
   int aSb = ay - (by + bh);
   int aWb = ax - (bx + bw);

   // Find the primary edge and secondary edges
   int m = max(aNb, max(aEb, max(aSb, aWb)));
   int aprimary = SOUTH;
   int asecondary1 = EAST;
   int asecondary2 = WEST;

   if (m == aEb) {
      aprimary = EAST;
      asecondary1 = NORTH;
      asecondary2 = SOUTH;
   } else if (m == aSb) {
      aprimary = NORTH;
      asecondary1 = WEST;
      asecondary2 = EAST;
   } else if (m == aWb) {
      aprimary = WEST;
      asecondary1 = SOUTH;
      asecondary2 = NORTH;
   }

   Log("Found primary and secondary sides");

   // Find our exact connection points and the best path between them
   // This function will return our two path end points, and will draw on the
   // ground map with multiples of 10 what the optimal path will be so that our
   // digging function can just follow that path.
   // We might have to try this method a few times to get a valid result since
   // the paths might start/end in walls, fail to find a suitable route, etc.
   SDL_Rect pathPoints = {0, 0, 0, 0};

   // Try to build a corridor from our three sides
   FindPath(a, b, aprimary, &pathPoints);
   if (pathPoints.x == 0) {
      FindPath(a, b, asecondary1, &pathPoints);
      if (pathPoints.x == 0) {
         FindPath(a, b, asecondary2, &pathPoints);
      }
   }

   // We still failed, return failure
   if (pathPoints.x == 0) {
      Log("Found no suitable connections");
      return false;
   }

   // We got a path, time to dig it with brute force
   for (int cY = 0; cY < groundSize; cY++) {
      for (int cX = 0; cX < groundSize; cX++) {
         if (ground[cY][cX] >= 10) ground[cY][cX] = 1;
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
      }

      return true;
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
      }

      return true;
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
      }

      return true;
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
      }

      return true;
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
                  printf("\nDug neighbor corridor between %i and %i", j, i);

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
   Log("Digging corridors now");
   for (int j = 0; j < 3; j++) {
      for (int i = 0; i < roomCount; i++) {
         // Select the destination room by its proximity to the target room
         Log("Finding min distances");
         float minDistance = 99.f;
         float secondMinDistance = 99.f;
         float thirdMinDistance = 99.f;
         int room1 = 0;
         int room2 = 0;
         int room3 = 0;

         for (int k = 0; k < roomCount; k++) {
            if (k != i) {
               float distance =
               FindDistance(rooms[i].cX, rooms[i].cY, rooms[k].cX, rooms[k].cY);

               if (distance < minDistance) {
                  thirdMinDistance = secondMinDistance;
                  secondMinDistance = minDistance;
                  minDistance = distance;
                  room3 = room2;
                  room2 = room1;
                  room1 = k;
               }
            }
         }
         int dest = room1;
         if (j == 2) dest = room2;
         if (j == 3) dest = room3;
         Log("Looking at rules");
         // Set our rules/restrictions bypass to false
         bool bypass = false;

         // If we're on our last try, and this room has no connections, we
         // override everything so that a connection can be made no matter what.
         if ((j == 2) && (rooms[i].connections == 0)) {
            bypass = true;
         }

         // If we aren't bypassing requirements, and our destination is already
         // at max connections, we can skip it and try another one.
         if (!bypass && (rooms[i].connections >= rooms[i].maxConnections)) {
            continue;
         }
         else {
            Log("Check 1");
            if (bypass ||
               (rooms[dest].connections < rooms[dest].maxConnections)) {
               // We have a solid connection ready to be made, potentially
               // First, we check and see if these two rooms are already
               // connected
               bool alreadyConnected = false;
               for (int k = 0; k < rooms[i].connections; k++) {
                  // If our room is already in the list of connections, skip
                  if (dest == rooms[i].connectedRooms[k]) {
                     alreadyConnected = true;
                     break;
                  }

                  // If our room collides with a room that's already connected,
                  // prevent it
                  if (IsRectCollision(&rooms[dest].rect,
                     &rooms[rooms[i].connectedRooms[k]].rect)) {
                     alreadyConnected = true;
                     break;
                  }
               }

               // If they aren't connected already, we can try to build the
               // corridor between the two
               if (!alreadyConnected) {
                  printf("\nTrying to dig a corridor from %i to %i", i, dest);
                  if (DigCorridor2(i, dest)) {
                     numCorridors++;
                     rooms[i].connectedRooms[rooms[i].connections] = dest;
                     rooms[dest].connectedRooms[rooms[dest].connections] = i;
                     rooms[i].connections++;
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

   bool verifyingConnections = true;
   while(verifyingConnections) {
      // Verify that all rooms are reachable from the spawn room
      connectedToSpawn = new bool[roomCount];
      for (int i = 0; i < roomCount; i++) { connectedToSpawn[i] = false; }
      connectedToSpawn[0] = true;
      CheckConnectionsToSpawn(0);

      // If a room isn't connected, then we need to force a connection to a room that is
      bool madeConnection = false;
      for (int i = 0; i < roomCount; i++) {
         if (!connectedToSpawn[i]) {

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

            if (!madeConnection) { return false; }
         }
      }

      verifyingConnections = madeConnection;
   }

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
   int conns = rooms[iter].connections; // number of connections to iter
   for (int i = 0; i < conns; i++) {
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
            if (ground[cY - 1][cX] == 1) { isWall = true; }
            else if (ground[cY][cX - 1] == 1) { isWall = true; }
            else if (ground[cY - 1][cX - 1] == 1) { isWall = true; }
            else if (ground[cY - 1][cX + 1] == 1) { isWall = true; }
            else if (ground[cY + 1][cX - 1] == 1) { isWall = true; }
            else if (ground[cY + 1][cX] == 1) { isWall = true; }
            else if (ground[cY][cX + 1] == 1) { isWall = true; }
            else if (ground[cY + 1][cX + 1] == 1) { isWall = true; }

            if (isWall) ground[cY][cX] = 2;

            // If the tile below this is a shown wall and this tile is void,
            // then we want this tile to be an invisible wall
            if ((ground[cY][cX] == 0) && (ground[cY + 1][cX] == 2)) {
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
         if ((ground[cY][cX] == 2) || (ground[cY][cX] == 3)) {
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
               if (ground[cY - 1][cX] == 1) {
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
   // First, search for valid locations
   for (int cY = 2; cY < groundSize-2; cY++) {
      for (int cX = 2; cX < groundSize-2; cX++) {
         if (ground[cY][cX] == 1) {
            if (ground[cY+1][cX] == 1 && ground[cY-1][cX] == 1 &&
            ground[cY][cX+1] == 1 && ground[cY][cX-1] == 1) {
               // We've found a + shape of floor tiles. Time to check if we're actually at a door junction

               int TL = ground[cY-1][cX-1];
               int TR = ground[cY-1][cX+1];
               int BL = ground[cY+1][cX-1];
               int BR = ground[cY+1][cX+1];

               // First, we check for simple T intersections, perhaps of halls
               if (TL == TR == BL == BR == 2) {
                  continue;
               } else if (TL == TR == 2 && (BL == 1 || BR == 1)) {
                  // Spot for a North-pointing door
                  if (roll()) {
                     doors.push_back(new Door(cX, cY, 1, 1, 0, locale));
                  }
               } else if (TR == BR == 2 && (TL == 1 || BL == 1)) {
                  // Spot for an East-pointing door
                  if (roll()) {
                     doors.push_back(new Door(cX, cY, 1, 1, 1, locale));
                  }
               } else if (BL == BR == 2 && (TL == 1 || TR == 1)) {
                  // Spot for a South-pointing door
                  if (roll()) {
                     doors.push_back(new Door(cX, cY, 1, 1, 2, locale));
                  }
               } else if (TL == BL == 2 && (TR == 1 || BR == 1)) {
                  // Spot for a West-pointing door
                  if (roll()) {
                     doors.push_back(new Door(cX, cY, 1, 1, 3, locale));
                  }
               }
            }
         }
      }
   }

   // Be sure we tell the doors to load up their textures and animations
   for (int i = 0; i < doors.size(); i++) {
      doors[i]->LoadArt();
   }
}



void Level::GenerateLevel() {
   // Start a timer for level generation for optimization
   LTimer timer;
   timer.start();
   floorRender.CreateBlank(PIXELSPERFEET * 5 * (groundSize + 10) * GMAXZOOM, PIXELSPERFEET * 5 * groundSize * GMAXZOOM);
   wallRender.CreateBlank(PIXELSPERFEET * 5 * (groundSize + 10) * GMAXZOOM, PIXELSPERFEET * 5 * groundSize * GMAXZOOM);

   bool levelIsBigEnough = false;
   while (!levelIsBigEnough) {
      NUMROOMCOLLISIONS = 0;
      while (roomsBuilt < roomCount) {
         GenerateRandomRoom(roomsBuilt);
         roomsBuilt++;
      }

      for (int i = 0; i < groundSize; i++) {
         for (int j = 0; j < groundSize; j++) {
            if (ground[i][j]) {
               roomTileCount++;
            }
         }
      }

      if (roomTileCount >= desiredRoomTiles) {
         levelIsBigEnough = true;
      } else {
         // If the room isn't big enough, start over
         roomTileCount = 0;
         roomsBuilt = 0;
         if (rooms != NULL) { delete rooms; }
         rooms = new Room[roomCount];
         for (int i = 0; i < groundSize; i++) {
            for (int j = 0; j < groundSize; j++) {
               ground[i][j] = 0;
            }
         }
      }
   }

   Log("Setting spawn and exit rooms");

   spawnRoom = rooms[0].rect;
   exitRoom = rooms[roomCount - 1].rect;

   Log("Creating walls for rooms");
   GenerateWalls();

   // Create corridors between rooms
   Log("Generating corridors");
   GenerateCorridors();

   // TODO: Come up with locale struct for dungeons, and later, caves

   // Plant secrets throughout the level

   // Set secrets in rooms

   // Block off secret corridors

   // Scan the floorplan and build locales

   // Build walls
   Log("Generating walls");
   GenerateWalls();

   // Finalize walls
   Log("Finalizing Walls");
   FinalizeWalls();
   // Give large rooms a chance to have big pits in them (no more than 60% of the room). This chance should be tied to the locale of the level

   // Write final tile values to file
   ofstream myFile;
   myFile.open("tiles.txt");
   char buf[2];
   for (int py = 0; py < groundSize; py++) {
      for (int px = 0; px < groundSize; px++) {
         buf[0] = ' ';
         buf[1] = ' ';
         snprintf(buf, 3, "%i", ground[py][px]);
         myFile << buf;
      }
      myFile << "\n";
   }
   myFile.close();


   printf("\nFinal tile count: %i", roomTileCount);
   printf("\nMax collisions: %i, total: %i", NUMROOMCOLLISIONSALLOWED, NUMROOMCOLLISIONS);

   printf("\nLevel generation time: %4.0i ms", timer.getTicks());
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
      for (int xi = 0; xi < ceil(floorRender.GetWidth()/floorTextureSize); xi++) {
         SDL_Rect r = {
            floorTextureSize * xi,
            floorTextureSize * yi,
            min(floorTextureSize, abs((floorTextureSize * (xi)) - floorRender.GetWidth())),
            min(floorTextureSize, abs((floorTextureSize * (yi)) - floorRender.GetHeight()))
         };
         locale->floorTexture.Render(&r);
      }
   }

   // Next, we initialize the void texture (OOB), and render the void onto the walls layer
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

   // Blit the repeating textures to the appropriate buffer textures so that we can render one large texture instead of 300 small textures every frame
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

            // If the tile beneath this tile is floor, then we render this tile as the "face" of the wall
            if (ground[yi + 1][xi] == 1) {
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

               // If the tile to our left is a floor tile, then this tile has the left edge of a wall
               if (ground[yi][xi - 1] == 1 ||
               (ground[yi][xi-1] == 2 && ground[yi+1][xi-1] == 1)) {
                  // Render the left edge
                  locale->wallTexture_LEdge.Render(&r);
                  drewLeft = true;
               }

               // If the tile to our right is a floor tile, then this tile has the right edge of a wall
               if (ground[yi][xi + 1] == 1 ||
               (ground[yi][xi+1] == 2 && ground[yi+1][xi+1] == 1)) {
                  // Render the right edge
                  locale->wallTexture_REdge.Render(&r);
                  drewRight = true;
               }

               // If the tile above this tile is a floor tile, then this tile has the back edge of a wall
               if (ground[yi - 1][xi] == 1) {
                  // render the back edge of wall
                  locale->wallTexture_TEdge.Render(&r);
                  drewLeft = true;
                  drewRight = true;
               }



               // If the tile to our diagonal is a floor tile, then we need to provide a corner piece to its surrounding walls
               if (ground[yi-1][xi-1] == 1 && !drewLeft) {
                  // Render top-left corner
                  locale->wallTexture_LCorner.Render(&r);
               }
               if (ground[yi-1][xi+1] == 1 && !drewRight) {
                  // Render top-right corner
                  locale->wallTexture_RCorner.Render(&r);
               }
               if (ground[yi+1][xi+1] == 1 && !drewRight) {
                  // Render bottom-right corner
                  locale->wallTexture_REdge.Render(&r);
               }
               if (ground[yi+1][xi-1] == 1 && !drewLeft) {
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



void Level::RenderFloor(int camX, int camY) {
   if (!renderTargetsCompiled) { CompileRenderTargets(); }
   floorRender.Render(-camX, -camY);
}



void Level::RenderWalls(int yO, int yF, int camX, int camY) {
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
      -camX,
      yO - camY,
      wallRender.GetWidth(),
      yF - yO
   };

   wallRender.Render(&drawBox, &wallChunk);

   // Paint collided walls
   // for (auto i=walls.begin(); i != walls.end(); i++) {
   //    int x = (i->second.hitBox.x) + 5;
   //    int y = (i->second.hitBox.y) + 5;
   //    int w = (i->second.hitBox.w) - 10;
   //    int h = (i->second.hitBox.h) - 10;
   //
   //    SDL_Rect r = {x, y, w, h};
   //    SDL_SetRenderTarget(gRenderer, wallRender.mTexture);
   //    SDL_RenderFillRect(gRenderer, &r);
   // }
   // SDL_SetRenderTarget(gRenderer, NULL);
}



Level::~Level() {
   delete rooms;
   delete connectedToSpawn;
}
