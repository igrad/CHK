#ifndef LEVEL_LOADED
#define LEVEL_LOADED

#include "essentials.h"
#include "StaticCollider.h"
#include "Decal.h"
#include "CollisionDetection.h"

// For each genre of room, there will be prescribed values for the number of permitted mega rooms, as well as the size of each of the mega rooms and (perhaps) their suggested dimensions. Also include the number of rooms for a given level, and the width of all hallways, as well as if the hallways should be static width or varied.
// Eventually, when a level is generated, it will take into account the rough location of the previous level's exit, and factor it into the entrance for the coming level

enum CORRIDOR_TYPE {
   CORRIDOR_CORNERS,
   CORRIDOR_DIRECT
};

enum CORNER_TYPE {
   CORNER_ROUNDED,
   CORNER_SHARP
};

class Room {
   public:
      Room();
      int X(int newVal = -1);
      int Y(int newVal = -1);
      int W(int newVal = -1);
      int H(int newVal = -1);
      int x2();
      int y2();
      void SetCenter();
      ~Room();

      int x;
      int y;
      int w;
      int h;
      int cX;
      int cY;
      SDL_Rect rect;
      int connections;
      int* connectedRooms;
      int maxConnections;
      int* neighbors;
};



class Level {
   public:
      Level();

      int GenerateRandomRoomDim();

      int GenerateRandomCoord();

      bool CheckNewRoom(SDL_Rect* room);

      void GenerateRandomRoom(int roomNum);

      bool DigCorridor(int a, int b);

      bool GenerateCorridors();

      void GenerateLevel();

      bool LoadFromFile(string path, int width, int height);

      bool WriteLevelToFile();

      void WriteOutWholeLevel();

      void Render();

      ~Level();

      StaticCollider* walls;
      Decal* decals;

      const static int groundSize = 100;
      bool ground[groundSize][groundSize];

      Room* rooms;

      int roomsBuilt;
      int roomCount;
      int roomTileCount;
      int desiredRoomTiles;

      SDL_Rect spawnRoom;
      SDL_Rect exitRoom;

      CORRIDOR_TYPE corridorType;
      CORNER_TYPE cornerType;
      int corridorSize;
      int roomCornerSize;

      int minRoomDim;
      int maxRoomDim;
};

#endif
