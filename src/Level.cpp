#include "..\include\Level.h"

enum DIRECTION { NORTH, EAST, SOUTH, WEST };

Room::Room() {
   x, y, w, h, cX, cY, connections, maxConnections = 0;
   neighbors = new int[4];
   connectedRooms = new int[4];
}

int Room::X(int newVal) {
   if (newVal != -1) { x = newVal; }
   return x;
}

int Room::Y(int newVal) {
   if (newVal != -1) { y = newVal; }
   return y;
}

int Room::W(int newVal) {
   if (newVal != -1) { w = newVal; }
   return w;
}

int Room::H(int newVal) {
   if (newVal != -1) { h = newVal; }
   return h;
}

int Room::x2() {
   return x + w;
}

int Room::y2() {
   return y + h;
}

void Room::SetCenter() {
   cX = (x + (x + w))/2;
   cY = (y + (y + h))/2;
}

Room::~Room() {
   delete connectedRooms;
   delete neighbors;
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

      //printf("\nrooms[i].rect = %i, %i, %i, %i", rooms[i].rect.x, rooms[i].rect.y, rooms[i].rect.w, rooms[i].rect.h);

      // If the new room is completely enveloped by another room, it fails
      if (IsWholeCollision(room, &r)) { return false; }

      // If more than half of the new room is merged with another room, it fails
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
   while(buildingRoom) {
      // Use the generated coordinates to check if there is a room at that coordinate already, and if so, if there's
      int x = GenerateRandomCoord();
      int y = GenerateRandomCoord();

      // Give it 5 tries to try to make a room out of this chosen coordinate. If it can't create a room after 5 tries, it's probably too tight anyways
      for (int i = 0; i < 5; i++) {
         int w = GenerateRandomRoomDim();
         int h = GenerateRandomRoomDim();

         // If there were no collisions with existing rooms, fill in the ground with this room's dimensions
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

   int cAX = 0;
   int cAY = 0;
   int cBX = 0;
   int cBY = 0;

   int dir = 0;

   if (dX > dY) {
      if (ax > bx) { dir = WEST; }
      else { dir = EAST; }
   } else {
      if (ay > by) { dir = NORTH; }
      else { dir = SOUTH; }
   }



   // printf("\ncAX: %i, cAY: %i", cAX, cAY);
   // printf("\na: %i, %i, %i, %i", ax, ay, aw, ah);
   // printf("\nb: %i, %i, %i, %i", bx, by, bw, bh);
   switch (dir) {
      case 0: {
         cAX = ax + (rand() % aw);
         cAY = ay;
         cBX = bx + (rand() % bw);
         cBY = by + bh;
         break;
      }
      case 1: {
         cAX = ax + (rand() % aw);
         cAY = ay + ah;
         cBX = bx + (rand() % bw);
         cBY = by;
         break;
      }
      case 2: {
         cAX = ax;
         cAY = ay + (rand() % ah);
         cBX = bx + bw;
         cBY = by + (rand() % bh);
         break;
      }
      case 3: {
         cAX = ax + aw;
         cAY = ay + (rand() % ah);
         cBX = bx + bw;
         cBY = by + (rand() % bh);
         break;
      }
   }



   float distanceBetweenPoints = pow(pow(cAX - cBX, 2) + pow(cAY + cBY, 2), 1/2);

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

         int segmentLength = (rand() % 7) + 2;
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

         if ((cX == cBX) && (cY == cBY)) {
            pathing = false;
            break;
         }
         else if (PointInRect(cX, cY, &rooms[b].rect)) {
            pathing = false;
            break;
         }

         //printf("\n\tSegment %i, length: %i, dir: %i", segmentCount, segmentLength, dir);
         for (int i = 0; i < segmentLength; i++) {
            ground[cY][cX] = true;



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

            ground[cY][cX] = true;

            xDist = abs(cX - cBX);
            yDist = abs(cY - cBY);

            if ((cX == cBX) && (cY == cBY)) {
               pathing = false;
               break;
            }
            else if (PointInRect(cX, cY, &rooms[b].rect)) {
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
   int numCorridors = 0;
   // Each room has 1-4 connections
   // Go through each room and make a connection to another room. This ensures that each room generated can be reached. Then, go through a second time, and add more corridors until the max number of corridors has been reached
   for (int i = 0; i < roomCount; i++) {
      int val = (rand() % 1) + 1;
      rooms[i].maxConnections = val;
   }

   for (int j = 0; j < 3; j++) {
      for (int i = 0; i < roomCount; i++) {
         Log("In roomcount");

         int dest = (rand() % roomCount);
         if (dest == i) { i--; }
         else {
            if (rooms[dest].connections < rooms[dest].maxConnections) {
               bool alreadyConnected = false;
               for (int k = 0; k < rooms[i].connections; k++) {
                  int connRoom = rooms[i].connectedRooms[k];
                  // If our room is already in the list of connections, prevent it
                  if (dest == connRoom) {
                     alreadyConnected = true;
                     k = 10;
                  }

                  // If our room collides with a room that's already connected, prevent it
                  if (IsRectCollision(&rooms[dest].rect, &rooms[connRoom].rect)) {
                     alreadyConnected = true;
                     break;
                  }
               }

               if (!alreadyConnected) {
                  printf("\nCalling dig on %i, %i", i, dest);
                  if (DigCorridor(i, dest)) {
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
               }
            }
         }
      }
   }

   Log("Matching up island rooms");

   // Verify that all rooms have at least one connection
   for (int i = 0; i < roomCount; i++) {
      // Find another room with an open connection slot
      int partner = -1;
      bool connectPartner = false;
      if (rooms[i].connections == 0) {
         for (int j = i + 1; j < roomCount; j++) {
            if (rooms[j].connections == 0) {
               partner = j;
               connectPartner = true;
               break;
            }
         }

         // If there are no other rooms with no connections, just connect to a room
         // with an availability
         if (partner == -1) {
            for (int j = 0; j < roomCount; j++) {
               if (j == i) {
                  continue;
               }

               if (rooms[j].connections < rooms[j].maxConnections) {
                  partner = j;
                  break;
               }
            }
         }
      }

      if (partner != -1) {
         while (DigCorridor(i, partner)) {
            Log("Digging corridor from island to partner");
         }
         if (connectPartner) {
            bool connectingPartner = true;
            while (connectingPartner) {
               int dest = (rand() % roomCount);
               bool destIsValid = true;
               if (rooms[dest].connections != rooms[dest]maxConnections) {
                  for (int j = 0; j < rooms[dest].connections; j++) {
                     if (rooms[dest].connectedRooms[j] == partner) {
                        destIsValid = false;
                     }
                  }

                  if (destIsValid) {
                     while (DigCorridor(partner, dest)) {
                        Log("Digging corridor from partner to dest");
                     }
                  }
               }
            }
         }
      }
   }
   printf("\nNumber of corridors: %i", numCorridors);
   return true;
}



void Level::GenerateLevel() {
   int gridSize = floor(SCREEN_HEIGHT/groundSize);

   printf("\nRoom Count: %i\nDesired tiles: %i", roomCount, desiredRoomTiles);

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

   printf("\nFinal tile count: %i", roomTileCount);
   printf("\nMax collisions: %i, total: %i", NUMROOMCOLLISIONSALLOWED, NUMROOMCOLLISIONS);

   // Create corridors between rooms
   GenerateCorridors();
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

   for (int yi = 0; yi < groundSize; yi++) {
      for (int xi = 0; xi < groundSize; xi++) {
         if (ground[yi][xi]) {
            myFile << "#";
            //cout << "#";
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

}
