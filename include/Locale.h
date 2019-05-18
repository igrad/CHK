#ifndef LOCALE
#define LOCALE

#include "essentials.h"

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
   LTexture doorN_shut;
   LTexture doorN_open;
   LTexture doorN_opening_slow;
   LTexture doorN_opening_fast;
   LTexture doorN_closing_slow;
   LTexture doorN_closing_fast;
   LTexture doorN_break_N;
   LTexture doorN_break_S;
   LTexture doorE_shut;
   LTexture doorE_open;
   LTexture doorE_opening_slow;
   LTexture doorE_opening_fast;
   LTexture doorE_closing_slow;
   LTexture doorE_closing_fast;
   LTexture doorE_break_E;
   LTexture doorE_break_W;
   LTexture doorS_shut;
   LTexture doorS_open;
   LTexture doorS_opening_slow;
   LTexture doorS_opening_fast;
   LTexture doorS_closing_slow;
   LTexture doorS_closing_fast;
   LTexture doorS_break_S;
   LTexture doorS_break_N;
   LTexture doorW_shut;
   LTexture doorW_open;
   LTexture doorW_opening_slow;
   LTexture doorW_opening_fast;
   LTexture doorW_closing_slow;
   LTexture doorW_closing_fast;
   LTexture doorW_break_W;
   LTexture doorW_break_E;
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

#endif
