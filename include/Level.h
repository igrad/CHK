#ifndef LEVEL_LOADED
#define LEVEL_LOADED

#include "essentials.h"
#include "Camera.h"
#include "Collider.h"
#include "Decal.h"
#include "Locale.h"
#include "Door.h"
#include "CollisionDetection.h"
#include "LTimer.h"

// For each genre of room, there will be prescribed values for the number of permitted mega rooms, as well as the size of each of the mega rooms and (perhaps) their suggested dimensions. Also include the number of rooms for a given level, and the width of all hallways, as well as if the hallways should be static width or varied.
// Eventually, when a level is generated, it will take into account the rough location of the previous level's exit, and factor it into the entrance for the coming level



class Room {
   public:
      Room();
      ~Room();

      int x;
      int y;
      int w;
      int h;
      int area;
      int cX;
      int cY;
      SDL_Rect rect;
      int connections;
      int* connectedRooms;
      int maxConnections;
};



class Level {
   public:
      Level();
      Level(Locale* locale);

      bool CheckNewRoom(SDL_Rect* room);
      void GenerateRandomRoom(int roomNum);

      void FindPath(int a, int b, int adir, SDL_Rect* r);
      bool DigCorridor(int a, int b);
      bool DigCorridor2(int a, int b);
      bool DigNeighbor(int a, int b);
      bool GenerateCorridors();
      void CheckConnectionsToSpawn(int iter);
      bool CheckWallHeights();

      void GenerateWalls();
      void FinalizeWalls();
      void GenerateDoors();
      void GenerateLevel();
      bool LoadFromFile(string path, int width, int height);

      void SetZoom(float newZoom);

      SDL_Rect GetPlayerSpawn();

      void WriteOutWholeLevel();

      void CompileRenderTargets(int xQ = -1, int yQ = -1);
      void RenderFloor(int camX, int camY);
      void RenderWalls(int yO, int yF, int camX, int camY);

      ~Level();

      // General
      float zoom;
      Locale* locale;

      // Wall generation variables
      map<int, Collider> walls;
      int wallCount;

      // Door generation variables
      vector<Door*> doors;

      // Decal variables
      Decal* decals;

      // Floor generation variables
      const static int maxGroundSize = 100;
      int groundSize;
      uint8_t ground[maxGroundSize][maxGroundSize];

      Room* rooms;

      int roomsBuilt;
      int roomCount;
      int roomTileCount;
      int desiredRoomTiles;

      bool* connectedToSpawn;

      SDL_Rect spawnRoom;
      SDL_Rect exitRoom;

      CORRIDOR_TYPE corridorType;
      int corridorSize;
      CORNER_TYPE cornerType;
      int cornerSize;

      int minRoomDim;
      int maxRoomDim;

      // Texture variables
      bool renderTargetsCompiled;
      LTexture floorRender;
      LTexture wallRender;
};

#endif
