#ifndef LEVEL_LOADED
#define LEVEL_LOADED

#include "essentials.h"
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
   int mapSize;
   int minRoomDim;
   int maxRoomDim;
   int minRoomCount;
   int maxRoomCount;
   LTexture floorTexture;
   LTexture wallTexture;
   // Might want to add secondary/tertiary floor and wall textures here later in development
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


      void GenerateLevel();

      bool LoadFromFile(string path, int width, int height);

      void SetZoom(float newZoom);


      SDL_Rect GetPlayerSpawn();


      void WriteOutWholeLevel();

      void CompileRenderTargets();

      void Render(int camX, int camY);

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
      const static int groundSize = 100;
      uint8_t ground[groundSize][groundSize];

      Room* rooms;

      int roomsBuilt;
      int roomCount;
      int roomTileCount;
      int desiredRoomTiles;

      bool* connectedToSpawn;

      SDL_Rect spawnRoom;
      SDL_Rect exitRoom;

      CORRIDOR_TYPE corridorType;
      CORNER_TYPE cornerType;
      int corridorSize;
      int roomCornerSize;

      int minRoomDim;
      int maxRoomDim;

      // Texture variables
      bool renderTargetsCompiled;
      LTexture floorRender;
      LTexture wallRender;
};

#endif
