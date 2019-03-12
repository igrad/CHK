#ifndef LEVEL_LOADED
#define LEVEL_LOADED

#include "essentials.h"
#include "Camera.h"
#include "Collider.h"
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



struct Locale {
   int mapSize = 100;
   int minRoomDim = 4;
   int maxRoomDim = 20;
   int minRoomCount = 10;
   int maxRoomCount = 16;
   CORNER_TYPE cornerType = CORNER_SHARP;
   int cornerSize = 1;
   CORRIDOR_TYPE corridorType = CORRIDOR_CORNERS;
   int corridorSize = 1;
   LTexture floorTexture;
   int floorTextureSize; // The floor is not a single repeating tile, but one vast
   LTexture wallTexture_Face1;
   LTexture wallTexture_Face2;
   LTexture wallTexture_Face3;
   int wallHeight; // Height of the wall in the file, so that we can account for overlapping
   LTexture wallTexture_LEdge;
   LTexture wallTexture_REdge;
   LTexture wallTexture_TEdge;
   LTexture wallTexture_LCorner;
   LTexture wallTexture_RCorner;
   // Might want to add secondary/tertiary floor and wall textures here later in development
   LTexture voidTexture;
   int voidTextureSize;
   // food object 1
   // food object 1
   // food object 1
   // Random item grade range

   // grunt enemy 1
   // grunt enemy 2
   // grunt enemy 3
   // grunt enemy 4
   // grunt enemy 5
   // bruiser enemy 1
   // bruiser enemy 2
   // bruiser enemy 3
   // artillery enemy 1
   // artillery enemy 2
   // artillery enemy 3
   // miniboss enemy 1
   // miniboss enemy 2
   // miniboss enemy 3
   // boss enemy 1
   // boss enemy 2

   Locale();
};



class Room {
   public:
      Room();
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
};



class Level {
   public:
      Level();
      Level(Locale* locale);

      bool CheckNewRoom(SDL_Rect* room);
      void GenerateRandomRoom(int roomNum);

      bool DigCorridor(int a, int b);
      bool GenerateCorridors();
      void CheckConnectionsToSpawn(int iter);

      void GenerateWalls();
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
