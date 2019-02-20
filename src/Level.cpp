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

            if (isWall) {
               int blockSize = PIXELSPERFEET * zoom * 5;
               ground[cY][cX] = 2;

               int key = (cY * 100) + cX;
               int lKey = (cY * 100) + cX - 1;
               int uKey = ((cY - 1) * 100) + cX;

               bool isWallAbove = (walls.find(uKey) != walls.end());
               bool isWallLeft = (walls.find(lKey) != walls.end());


               bool wallEdited = false;
               // First, we check and see if there's a wall above AND to the left of this wall piece. If there is, then it's safe to assume that the two can be combined as long as neither of them are only one unit wide/tall (respectively)
               if (isWallLeft && isWallAbove) {
                  SDL_Rect* lC = &walls[lKey].hitBox;
                  SDL_Rect* uC = &walls[uKey].hitBox;
                  SDL_Rect* lD = &walls[lKey].drawBox;
                  SDL_Rect* uD = &walls[uKey].drawBox;
                  if (lC->h != blockSize || uC->w != blockSize) {
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

                     if (!((ground[cY - 1][cX] != 2) && (dC.hitBox.h != blockSize))) {
                        walls.erase(lKey);

                        dC.hitBox.w += blockSize;
                        dC.drawBox.w += blockSize;
                        walls.insert(pair<int, Collider>(key, dC));

                        wallEdited = true;
                     }
                  } else if (isWallAbove) {
                     // When the width of the wall above is more than 1 block wide, and there isn't a wall to the left, we need to start a new wall instead of continuing the wall above
                     Collider dC = walls[uKey];

                     if (!((ground[cY][cX - 1] != 2) && (dC.hitBox.w != blockSize))) {
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

                  dC.SetPos(cX * blockSize, cY * blockSize);
                  dC.SetHitBox(cX * blockSize, cY * blockSize, blockSize, blockSize);
                  dC.SetDrawBox(cX * blockSize, cY * blockSize, blockSize, blockSize);

                  walls.insert(pair<int, Collider>(key, dC));
               }
            }
         }
      }
   }
}



void Level::GenerateLevel() {
   floorRender.CreateBlank(PIXELSPERFEET * 5 * groundSize * GMAXZOOM, PIXELSPERFEET * 5 * groundSize * GMAXZOOM);
   wallRender.CreateBlank(PIXELSPERFEET * 5 * groundSize * GMAXZOOM, PIXELSPERFEET * 5 * groundSize * GMAXZOOM);

   Log("Blanked render targets. Beginning level");
   printf("\ngroundSize = %i", groundSize);
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

   Log("Setting spawn rooms");

   spawnRoom = rooms[0].rect;
   exitRoom = rooms[roomCount - 1].rect;

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
   // Give large rooms a chance to have big pits in them (no more than 60% of the room). This chance should be tied to the locale of the level

   printf("\nFinal tile count: %i", roomTileCount);
   printf("\nMax collisions: %i, total: %i", NUMROOMCOLLISIONSALLOWED, NUMROOMCOLLISIONS);

   printf("\n\nPrinting walls:");
   for (auto i=walls.begin(); i != walls.end(); i++) {
      int x = i->second.hitBox.x / (PIXELSPERFEET * zoom * 5);
      int y = i->second.hitBox.y / (PIXELSPERFEET * zoom * 5);
      int w = i->second.hitBox.w / (PIXELSPERFEET * zoom * 5);
      int h = i->second.hitBox.h / (PIXELSPERFEET * zoom * 5);
      printf("\n\tWall: {%i, %i, %i, %i}", x, y, w, h);
   }
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

   // Paint collided walls
   for (auto i=walls.begin(); i != walls.end(); i++) {
      int x = (i->second.hitBox.x) + 20;
      int y = (i->second.hitBox.y) + 20;
      int w = (i->second.hitBox.w) - 40;
      int h = (i->second.hitBox.h) - 40;

      SDL_Rect r = {x, y, w, h};
      SDL_SetRenderTarget(gRenderer, wallRender.mTexture);
      SDL_RenderFillRect(gRenderer, &r);
   }

   SDL_SetRenderTarget(gRenderer, NULL);
}



Level::~Level() {
   delete rooms;
   delete connectedToSpawn;
}
