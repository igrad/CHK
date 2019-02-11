#include "..\include\Level.h"

enum DIRECTION { NORTH, EAST, SOUTH, WEST };

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



void Level::PrintRooms() {
   printf("\nRoomcount: %i", roomCount);
   for (int k = 0; k < roomCount; k++) {
      printf("\nRoom #%i: {%i, %i, %i, %i}, connections: %i, maxConnections: %i", k, rooms[k].x, rooms[k].y, rooms[k].w, rooms[k].h, rooms[k].connections, rooms[k].maxConnections);
   }
}



int NUMROOMCOLLISIONS = 0;
int NUMROOMCOLLISIONSALLOWED = 0;
int MAXCORRIDORS = 0;

Level::Level() {
   srand(time(NULL));
   walls = NULL;
   decals = NULL;

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
}



int Level::GenerateRandomRoomDim() {
   return (rand() % (maxRoomDim - minRoomDim + 1)) + minRoomDim;
}



int Level::GenerateRandomCoord() {
   return (rand() % (groundSize - 1)) + 1;
}



bool Level::CheckNewRoom(SDL_Rect* room) {
   // Check that it still fits on the board
   if (room->x + room->w > groundSize - 1) {
      return false;
   } else if (room->y + room->h > groundSize - 1) {
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
      // Use the generated coordinates to check if there is a room at that coordinate already, and if so, if there's
      int x = GenerateRandomCoord();
      int y = GenerateRandomCoord();

      // Give it 5 tries to try to make a room out of this chosen coordinate. If it can't create a room after 5 tries, it's probably too tight anyways
      for (int i = 0; i < 5; i++) {
         int w = GenerateRandomRoomDim();
         int h = GenerateRandomRoomDim();

         // If it passes the check, fill in the ground with this room's dimensions
         SDL_Rect room = {x, y, w, h};
         if (CheckNewRoom(&room)) {
            for (int yi = y; yi < y + h; yi++) {
               for (int xi = x; xi < x + w; xi++) {
                  ground[yi][xi] = true;
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
                  printf("\n[Room collision during generation] Adding connection from %i to %i", roomNum, j);
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

   printf("\n[DigCorridor] Set dir between %i and %i with the following\n\taNSb: %i\n\tbNSa: %i\n\taEWb: %i\n\tbEWa: %i", a, b, aNSb, bNSa, aEWb, bEWa);

   int cAX = 0;
   int cAY = 0;
   int cBX = 0;
   int cBY = 0;

   //
   //
   // if (dX > dY) {
   //    if (ax > bx) { dir = WEST; }
   //    else { dir = EAST; }
   // } else {
   //    if (ay > by) { dir = NORTH; }
   //    else { dir = SOUTH; }
   // }



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

         ground[cY][cX] = true;
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
                           printf("\n[Accidental connection via corridor A] Adding connection from %i to %i. a.connections", a, j);
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
                           printf("\n[Accidental connection via corridor B] Adding connection from %i to %i. b.connections", b, j);
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
               ground[cY][cX] = true;
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
   Log("Entering generate corridors");
   // Log("[Start of GenerateCorridors] Room connections matrix");
   // for (int i = 0; i < roomCount; i++) {
   //    printf("\n\tRoom %i:", i);
   //    for (int j = 0; j < rooms[i].connections; j++) {
   //       printf(" %i,", rooms[i].connectedRooms[j]);
   //    }
   // }
   int numCorridors = 0;

   for (int j = 0; j < 3; j++) {
      for (int i = 0; i < roomCount; i++) {
         int dest = (rand() % roomCount);
         bool bypass = false;

         if ((j == 2) && (rooms[i].connections == 0)) {
            bypass = true;
         }

         if (!bypass && (rooms[i].connections >= rooms[i].maxConnections)) { continue; }
         else if ((dest == i)) { i--; }
         else if (!bypass && (FindDistance(rooms[i].x, rooms[i].y, rooms[dest].x, rooms[dest].y) > (groundSize/2))) {
            i--;
         }
         else {
            printf("\n\tIn the else call");
            if (bypass || (rooms[dest].connections < rooms[dest].maxConnections)) {
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

               printf("\n\tMade it to alreadyConnected");
               if (!alreadyConnected) {
                  //printf("\nCalling dig on %i, %i", i, dest);
                  if (DigCorridor(i, dest)) {
                     printf("\n\tPast DigCorridor1");
                     numCorridors++;
                     rooms[i].connectedRooms[rooms[i].connections] = dest;
                     rooms[dest].connectedRooms[rooms[dest].connections] = i;
                     rooms[i].connections++;
                     rooms[dest].connections++;

                     if (numCorridors == MAXCORRIDORS) {
                        j = 2;
                        i = roomCount;
                        Log("Max corridor limit reached");
                     }
                  }
                  printf("\n\tPast DigCorridor2");
               }
            }
         }
      }
   }

   // Verify that all rooms have at least one connection
   // for (int j = 0; j < 2; j++) {
   //    for (int i = 0; i < roomCount; i++) {
   //       // Find another room with an open connection slot
   //       int partner = -1;
   //       bool connectPartner = false;
   //       if (rooms[i].connections == 0) {
   //          for (int j = i + 1; j < roomCount; j++) {
   //             if (rooms[j].connections == 0) {
   //                partner = j;
   //                connectPartner = true;
   //                break;
   //             }
   //          }
   //
   //          // If there are no other rooms with no connections, just connect to a room
   //          // with an availability
   //          if (partner == -1) {
   //             for (int j = 0; j < roomCount; j++) {
   //                if (j == i) {
   //                   continue;
   //                }
   //
   //                if (rooms[j].connections < rooms[j].maxConnections) {
   //                   partner = j;
   //                   break;
   //                }
   //             }
   //          }
   //       }
   //    }
   // }
   PrintRooms();
   WriteOutWholeLevel();
   Log("[Halfway through GenerateCorridors] Room connections matrix");
   for (int i = 0; i < roomCount; i++) {
      printf("\n\tRoom %i:", i);
      for (int j = 0; j < rooms[i].connections; j++) {
         printf(" %i,", rooms[i].connectedRooms[j]);
      }
   }

   bool verifyingConnections = true;
   while(verifyingConnections) {
      Log("\n\nVerifying connections");

      // Verify that all rooms are reachable from the spawn room
      if (connectedToSpawn != NULL) { delete connectedToSpawn; }
      connectedToSpawn = new bool[roomCount];
      connectedToSpawn[0] = true;
      CheckConnectionsToSpawn(0);


      // If a room isn't connected, then we need to force a connection to a room that is
      bool madeConnection = false;
      for (int i = 0; i < roomCount; i++) {
         if (!connectedToSpawn[i]) {
            printf("\n\t%i not connected to spawn", i);
            madeConnection = false;
            // Give it 10 tries to make a connection naturally (nearby)
            for (int j = 0; j < 10; j++) {
               int dest = rand() % roomCount;

               if (dest == i) { continue; }
               if (FindDistance(rooms[i].x, rooms[i].y, rooms[dest].x, rooms[dest].y) <= (groundSize/2)) {
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

   PrintRooms();
   printf("\nNumber of corridors: %i", numCorridors);
   return true;
}



void Level::CheckConnectionsToSpawn(int iter) {
   printf("\nconnectedToSpawn status: %s", connectedToSpawn[0]?"T":"FALSE");
   for (int i = 1; i < roomCount; i++) {
      printf(", %i:%s", i, connectedToSpawn[i]?"T":"FALSE");
   }

   int conns = rooms[iter].connections; // number of connections to iter
   for (int i = 0; i < conns; i++) {
      int dest = rooms[iter].connectedRooms[i];
      if (!connectedToSpawn[dest]) {
         connectedToSpawn[dest] = true;
         CheckConnectionsToSpawn(dest);
      }
   }
}



void Level::GenerateLevel() {
   int gridSize = floor(SCREEN_HEIGHT/groundSize);

   //printf("\nRoom Count: %i\nDesired tiles: %i", roomCount, desiredRoomTiles);

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
               ground[i][j] = false;
            }
         }
      }
   }

   spawnRoom = rooms[0].rect;
   exitRoom = rooms[roomCount - 1].rect;

   // Create corridors between rooms
   GenerateCorridors();

   // Check and make sure that all rooms are connected. If they aren't, loop back to the start

   // Plant secrets throughout the level
   // Set secrets in rooms
   // Block off secret corridors

   // Scan the floorplan and build locales

   // Build walls

   printf("\nFinal tile count: %i", roomTileCount);
   printf("\nMax collisions: %i, total: %i", NUMROOMCOLLISIONSALLOWED, NUMROOMCOLLISIONS);
}



bool Level::LoadFromFile(string path, int width, int height) {
   return false;
}



bool Level::WriteLevelToFile() {
   return false;
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
         if (ground[xi][yi]) {
            bool inRect = false;
            for (int r = 0; r < roomCount; r++) {
               if (PointInRect(yi, xi, &rooms[r].rect)) {
                  myFile << hex << r;
                  inRect = true;
               }
            }

            if (!inRect) {
               myFile << "#";
               //cout << "#";
            }
         } else {
            myFile << " ";
            //cout << " ";
         }
      }
      myFile << "\n";
      //cout << "\n";
   }

   myFile.close();
}



void Level::Render() {
   // Render to screen
   for (int yi = 0; yi < groundSize; yi++) {
      for (int xi = 0; xi < groundSize; xi++) {
         if (ground[xi][yi]) {
            SDL_Rect r = {8 * xi, 8 * yi, 8, 8};
            SDL_RenderFillRect(gRenderer, &r);
         }
      }
   }
}



Level::~Level() {
   delete rooms;
   delete connectedToSpawn;
}
