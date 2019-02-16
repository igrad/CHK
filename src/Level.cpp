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
   srand(time(NULL));
   walls = NULL;
   decals = NULL;

   zoom = 1.0 * GZOOM;

   for (int i = 0; i < groundSize; i++) {
      for (int j = 0; j < groundSize; j++) {
         ground[i][j] = 0;
      }
   }

   minRoomDim = 6;
   maxRoomDim = 32;

   corridorType = CORRIDOR_CORNERS;
   cornerType = CORNER_SHARP;
   corridorSize = 1;
   roomCornerSize = 1;

   // Generate 10 to 16 rooms
   roomCount = (rand() % 7) + 10;
   rooms = new Room[roomCount];

   // 10000 tiles
   // 5776 max tiles possible (using avg lengths)
   // 4693 pure average
   // 1805 - 2888 tolerable lower bound vvv
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

   // Check for collisions with other rooms
   bool passingCollision = false;
   for (int i = 0; i < roomCount; i++) {
      SDL_Rect r = rooms[i].rect;

      // TODO: This section needs heavy cleaning. We shouldn't have to make 3 calls for collision detection

      // If the new room is completely enveloped by another room, it fails
      if (IsWholeCollision(room, &r)) { return false; }

      // If more than 3/4ths of the new room is merged with another room, it fails
      if (IsPartialCollision(room, &r) > ((3/4) * room->w * room->h)) {
         return false;
      }

      // If we already have too many collisions, it fails
      if (IsRectCollision(room, &r)) {
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
            rooms[roomNum].cX = x + (w/2);
            rooms[roomNum].cY = y + (h/2);

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



bool Level::DigCorridor(int a, int b) {
   int ax = rooms[a].rect.x;
   int ay = rooms[a].rect.y;
   int aw = rooms[a].rect.w;
   int ah = rooms[a].rect.h;
   int bx = rooms[b].rect.x;
   int by = rooms[b].rect.y;
   int bw = rooms[b].rect.w;
   int bh = rooms[b].rect.h;

   int dX = abs(rooms[a].cX - rooms[b].cX);
   int dY = abs(rooms[a].cY - rooms[b].cY);


   int aNSb = by - (ay + ah);
   int bNSa = ay - (by + bh);
   int aEWb = bx - (ax + aw);
   int bEWa = ax - (bx + bw);

   int dir = 0;

   if (aNSb > 0) { dir = SOUTH; }
   else if (bNSa > 0) { dir = NORTH; }
   else if (aEWb > 0) { dir = WEST; }
   else if (bEWa > 0) { dir = EAST; }

   //printf("\n[DigCorridor] Set dir between %i and %i with the following\n\taNSb: %i\n\tbNSa: %i\n\taEWb: %i\n\tbEWa: %i", a, b, aNSb, bNSa, aEWb, bEWa);

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



   int xDist = abs(cAX - cBX);
   int yDist = abs(cAY - cBY);
   int cX = cAX;
   int cY = cAY;

   bool pathing = true;

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

         // If we're a short distance away, let's just get to the finish line and not
         // do anything special or random
         if (remainingDistance <= (2 * segmentLength)) {
            if (cX == cBX) { segmentLength = yDist; }
            else if (cY == cBY) { segmentLength = xDist; }
            else {
               if ((dir == NORTH) || (dir == SOUTH)) { segmentLength = yDist; }
               else { segmentLength = xDist; }
            }
         } else {
            // If we're still far enough away from our end point, let's put a spin
            // on the formula
            if (lastDirSet && ((rand() % 5) == 4)) { dir == lastDir; }
            lastDir = dir;
            lastDirSet = true;
         }

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

            // If this block is already true, it's probably in another corridor or in a room. If it's in a room, then it needs to count as a connection.
            if (ground[cY][cX]) {
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
                           }
                        }

                        if (!skipAConnection) {
                           //printf("\n[Accidental connection via corridor A] Adding connection from %i to %i. a.connections", a, j);
                           // for (int k = 0; k < rooms[a].connections; k++) {
                           //    printf(", %i", rooms[a].connectedRooms[k]);
                           // }
                           // Log("Room connections matrix");
                           // for (int q = 0; q < roomCount; q++) {
                           //    printf("\n\tRoom %i:", q);
                           //    for (int w = 0; w < rooms[q].connections; w++) {
                           //       printf(" %i,", rooms[q].connectedRooms[w]);
                           //    }
                           // }

                           rooms[a].connectedRooms[rooms[a].connections] = j;
                           rooms[a].connections++;

                           rooms[j].connectedRooms[rooms[j].connections] = a;
                           rooms[j].connections++;
                        }

                        bool skipBConnection = false;
                        for (int k = 0; k < rooms[b].connections; k++) {
                           if (rooms[b].connectedRooms[k] == j) {
                              skipBConnection = true;
                           }
                        }

                        if (!skipBConnection) {
                           //printf("\n[Accidental connection via corridor B] Adding connection from %i to %i. b.connections", b, j);
                           // for (int k = 0; k < rooms[b].connections; k++) {
                           //    printf(", %i", rooms[b].connectedRooms[k]);
                           // }
                           // Log("Room connections matrix");
                           // for (int q = 0; q < roomCount; q++) {
                           //    printf("\n\tRoom %i:", q);
                           //    for (int w = 0; w < rooms[q].connections; w++) {
                           //       printf(" %i,", rooms[q].connectedRooms[w]);
                           //    }
                           // }
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

   return true;
}



bool Level::GenerateCorridors() {
   int numCorridors = 0;

   for (int j = 0; j < 3; j++) {
      for (int i = 0; i < roomCount; i++) {
         int dest = (rand() % roomCount);
         bool bypass = false;

         if ((j == 2) && (rooms[i].connections == 0)) {
            // If we're on our last try, and this room has no connections, we override everything so that a connection can be made no matter what
            bypass = true;
         }

         if (!bypass && (rooms[i].connections >= rooms[i].maxConnections)) {
            // If we aren't bypassing requirements, and our destination is already at max connections, we can skip it and try another one.
            continue;
         }
         else if ((dest == i)) { i--; }
         else if (!bypass && (FindDistance(rooms[i].x, rooms[i].y, rooms[dest].x, rooms[dest].y) > (groundSize/2))) {
            // If the room is more than half the map away, then we need to try again. Super long corridors make everything messier.
            i--;
         }
         else {
            if (bypass || (rooms[dest].connections < rooms[dest].maxConnections)) {
               // We have a solid connection ready to be made, potentially
               // First, we check and see if these two rooms are already connected
               bool alreadyConnected = false;
               for (int k = 0; k < rooms[i].connections; k++) {

                  int connRoom = rooms[i].connectedRooms[k];
                  // If our room is already in the list of connections, prevent it
                  if (dest == connRoom) {
                     alreadyConnected = true;
                     k = rooms[i].connections;
                  }

                  // If our room collides with a room that's already connected, prevent it
                  if (IsRectCollision(&rooms[dest].rect, &rooms[connRoom].rect)) {
                     alreadyConnected = true;
                     k = rooms[i].connections;
                  }
               }

               // If they aren't connected already, we can try to build the corridor between the two
               if (!alreadyConnected) {
                  //printf("\nCalling dig on %i, %i", i, dest);
                  if (DigCorridor(i, dest)) {
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
      if (connectedToSpawn != NULL) { delete connectedToSpawn; }
      connectedToSpawn = new bool[roomCount];
      connectedToSpawn[0] = true;
      CheckConnectionsToSpawn(0);

      // If a room isn't connected, then we need to force a connection to a room that is
      bool madeConnection = false;
      for (int i = 0; i < roomCount; i++) {
         if (!connectedToSpawn[i]) {
            madeConnection = false;
            // Give it 10 tries to make a connection naturally (nearby)
            for (int j = 0; j < 10; j++) {
               int dest = rand() % roomCount;

               if (!connectedToSpawn[dest]) { continue; }
               else if (dest == i) { continue; }
               else if (FindDistance(rooms[i].x, rooms[i].y, rooms[dest].x, rooms[dest].y) <= (groundSize/2)) {
                  if (DigCorridor(i, dest)) {
                     madeConnection = true;
                     i = roomCount;
                     j = 10;
                  }
               }
            }

            // If it still failed after 10 tries, connect directly to spawn
            if (!madeConnection) {
               while(!DigCorridor(i, 0)) { Log("Trying to link to spawn"); }
            }
         }
      }

      verifyingConnections = madeConnection;
   }

   // Simple logging to print out the room dimensions and connections to the console
   // for (int i = 0; i < roomCount; i++) {
   //    printf("\n\tRoom %i: \t{%i, %i, %i, %i}:    \t", i, rooms[i].x, rooms[i].y, rooms[i].w, rooms[i].h);
   //    for (int j = 0; j < rooms[i].connections; j++) {
   //       printf(" %i,", rooms[i].connectedRooms[j]);
   //    }
   // }

   printf("\nNumber of corridors: %i", numCorridors);
   return true;
}



void Level::CheckConnectionsToSpawn(int iter) {
   //printf("\nconnectedToSpawn status: %s", connectedToSpawn[0]?"T":"FALSE");
   // for (int i = 1; i < roomCount; i++) {
   //    printf(", %i:%s", i, connectedToSpawn[i]?"T":"FALSE");
   // }

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

            if (isWall) { ground[cY][cX] = 2; }
         }
      }
   }
}



void Level::GenerateLevel(Locale* locale) {
   this->locale = locale;
   floorRender.CreateBlank(PIXELSPERFEET * 5 * groundSize * GMAXZOOM, PIXELSPERFEET * 5 * groundSize * GMAXZOOM);
   wallRender.CreateBlank(PIXELSPERFEET * 5 * groundSize * GMAXZOOM, PIXELSPERFEET * 5 * groundSize * GMAXZOOM);
   int gridSize = floor(SCREEN_HEIGHT/groundSize);

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

   spawnRoom = rooms[0].rect;
   exitRoom = rooms[roomCount - 1].rect;

   // Create corridors between rooms
   GenerateCorridors();

   // TODO: Come up with locale struct for dungeons, and later, caves

   // Plant secrets throughout the level

   // Set secrets in rooms

   // Block off secret corridors

   // Scan the floorplan and build locales

   // Build walls
   GenerateWalls();
   // Give large rooms a chance to have big pits in them (no more than 60% of the room). This chance should be tied to the locale of the level

   printf("\nFinal tile count: %i", roomTileCount);
   printf("\nMax collisions: %i, total: %i", NUMROOMCOLLISIONSALLOWED, NUMROOMCOLLISIONS);
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

   for (int i = 0; i < roomCount; i++) {
      for (int y = rooms[i].y; y < rooms[i].y; y++) {
         for (int x = rooms[i].x; x < rooms[i].w; x++) {
            myFile << i;
         }
      }
   }

   for (int yi = 0; yi < groundSize; yi++) {
      for (int xi = 0; xi < groundSize; xi++) {
         char o = ' ';
         switch (ground[yi][xi]) {
            case 1: o = ':'; break;
            case 2: o = '#'; break;
            default: o = ' ';
         }

         myFile << o;
      }
      myFile << "\n";
   }

   myFile.close();
}



void Level::CompileRenderTargets() {
   int tileW = PIXELSPERFEET * 5 * zoom;

   // Blit the repeating textures to the appropriate buffer textures so that we can render one large texture instead of 300 small textures every frame
   for (int yi = 0; yi < groundSize; yi++) {
      for (int xi = 0; xi < groundSize; xi++) {
         int x = (tileW * xi);
         int y = (tileW * yi);
         int w = tileW;
         int h = tileW;
         SDL_Rect r = {x, y, w, h};
         if (ground[yi][xi] == 1) {
            SDL_SetRenderTarget(gRenderer, floorRender.mTexture);
            locale->floorTexture.Render(&r);
         } else if (ground[yi][xi] == 2) {
            SDL_SetRenderTarget(gRenderer, wallRender.mTexture);
            locale->wallTexture.Render(&r);
         }
      }
   }

   SDL_SetRenderTarget(gRenderer, NULL);

   renderTargetsCompiled = true;
}



void Level::Render(int camX, int camY) {
   if (!renderTargetsCompiled) { CompileRenderTargets(); }
   floorRender.Render(-camX, -camY);
   wallRender.Render(-camX, -camY);
}



Level::~Level() {
   delete rooms;
   delete connectedToSpawn;
}
