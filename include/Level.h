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



// Index of tile codes
enum TILE_CODES {
   T_VOID_UNUSED,
   T_GROUND_UNUSED,
   T_WALL_UNUSED,
   T_INVIS_WALL_UNUSED,
   T_VOID_RES,
   T_GROUND_RES,
   T_WALL_RES,
   T_INVIS_WALL_RES,
   T_VOID_FAKE,
   T_GROUND_FAKE,
   T_WALL_FAKE,
   T_INVIS_WALL_FAKE,
   T_UNUSED1,
   T_UNUSED2,
   T_UNUSED3,
   T_UNUSED4,
   T_DOOR
};



struct NavBlock {
   int x;
   int y;
   bool mustUse;  // Even if this isn't the furthest block in LoS, it's used
};



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



const int MAXGROUNDSIZE = 100;



class Level {
   public:
      Level();
      Level(Locale* locale);

      void NearestVacantGrid(int destX, int destY, int* rX, int* rY);
      bool FindGroundRoute(int origX, int origY, int destX, int destY, 
         vector<pair<int,int>>* checkpoints);
      void GetCellsInLineOfSight(int origX, int origY, int destX, int destY,
         vector<pair<int,int>>* hitCells);
      bool HasLineOfSight(int origX, int origY, int destX, int destY);
      bool HasFullLineOfSight(int origX, int origY, int destX, int destY);

      bool IsGround(int x, int y);
      bool IsWall(int x, int y);

      bool CheckNewRoom(SDL_Rect* room);
      void GenerateRandomRoom(int roomNum);

      void UpdateGroundCopy();
      void ResetMod10Changes();
      bool PointInRoom(int x, int y, int room);
      bool PointInAnyRoom(int x, int y);

      bool CheckWallTile(int x, int y);

      bool FindPath(int a, int b, int adir);
      bool DigShortCorridor(int x1, int y1, int x2, int y2);
      bool DigCorridor(int a, int b);
      bool DigNeighbor(int a, int b);
      bool GenerateCorridors();
      bool GenerateCorridors2();
      void CheckConnectionsToSpawn(int iter);
      bool CheckWallHeights();

      void GenerateWalls();
      void FinalizeWalls();
      void GenerateDoors();
      void GenerateLevel();
      bool LoadFromFile(string path, int width, int height);

      SDL_Rect GetPlayerSpawn();

      void WriteOutWholeLevel();

      void CompileRenderTargets(int xQ = -1, int yQ = -1);
      void RenderFloor();
      void RenderWalls(int yO, int yF);
      void RenderDoors(int yO, int yF);

      ~Level();

      // General
      Locale* locale;

      // Wall generation variables
      map<int, Collider> walls;
      int wallCount;

      // Door generation variables
      vector<Door*> doors;

      // Decal variables
      Decal* decals;

      // Floor generation variables
      int groundSize;
      uint8_t ground[MAXGROUNDSIZE][MAXGROUNDSIZE];
      uint8_t groundCopy[MAXGROUNDSIZE][MAXGROUNDSIZE];

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
