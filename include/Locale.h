#ifndef LOCALE
#define LOCALE

#include "essentials.h"
#include "LTexture.h"

enum CORRIDOR_TYPE {
   CORRIDOR_CORNERS,
   CORRIDOR_DIRECT
};

enum CORNER_TYPE {
   CORNER_ROUNDED,
   CORNER_SHARP
};

class Locale {
public:
   Locale();

   void Initialize(string prefix);

   void InitDungeon();

   int mapSize = 100;
   int minRoomDim = 4;
   int maxRoomDim = 20;
   int minRoomCount = 10;
   int maxRoomCount = 16;
   CORNER_TYPE cornerType = CORNER_SHARP;
   int cornerSize = 1;
   CORRIDOR_TYPE corridorType = CORRIDOR_CORNERS;
   int corridorSize = 1;
   int doorSize = 1;
   int doorPercentChance = 100;
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

   float doorNSChangeRate_slow;
   float doorNSChangeRate_fast;
   float doorEChangeRate_slow;
   float doorEChangeRate_fast;
   float doorWChangeRate_slow;
   float doorWChangeRate_fast;

   LTexture doorNS_right_inside_open;
   LTexture doorNS_right_inside_closed;
   LTexture doorNS_right_inside_break_in;
   LTexture doorNS_right_inside_break_out;
   LTexture doorNS_right_inside_open_anim;
   LTexture doorNS_right_inside_close_anim;
   LTexture doorNS_right_inside_break_in_anim;
   LTexture doorNS_right_inside_break_out_anim;
   SDL_Rect doorNS_right_inside_part1Clip;
   SDL_Rect doorNS_right_inside_part2Clip;
   SDL_Rect doorNS_right_inside_doorClip;

   LTexture doorNS_left_inside_open;
   LTexture doorNS_left_inside_closed;
   LTexture doorNS_left_inside_break_in;
   LTexture doorNS_left_inside_break_out;
   LTexture doorNS_left_inside_open_anim;
   LTexture doorNS_left_inside_close_anim;
   LTexture doorNS_left_inside_break_in_anim;
   LTexture doorNS_left_inside_break_out_anim;
   SDL_Rect doorNS_left_inside_part1Clip;
   SDL_Rect doorNS_left_inside_part2Clip;
   SDL_Rect doorNS_left_inside_doorClip;

   LTexture doorNS_right_outside_open;
   LTexture doorNS_right_outside_closed;
   LTexture doorNS_right_outside_break_in;
   LTexture doorNS_right_outside_break_out;
   LTexture doorNS_right_outside_open_anim;
   LTexture doorNS_right_outside_close_anim;
   LTexture doorNS_right_outside_break_in_anim;
   LTexture doorNS_right_outside_break_out_anim;
   SDL_Rect doorNS_right_outside_part1Clip;
   SDL_Rect doorNS_right_outside_part2Clip;
   SDL_Rect doorNS_right_outside_doorClip;

   LTexture doorNS_left_outside_open;
   LTexture doorNS_left_outside_closed;
   LTexture doorNS_left_outside_break_in;
   LTexture doorNS_left_outside_break_out;
   LTexture doorNS_left_outside_open_anim;
   LTexture doorNS_left_outside_close_anim;
   LTexture doorNS_left_outside_break_in_anim;
   LTexture doorNS_left_outside_break_out_anim;
   SDL_Rect doorNS_left_outside_part1Clip;
   SDL_Rect doorNS_left_outside_part2Clip;
   SDL_Rect doorNS_left_outside_doorClip;

   LTexture doorE_left_open;
   LTexture doorE_left_closed;
   LTexture doorE_left_broke_in;
   LTexture doorE_left_broke_out;
   LTexture doorE_left_open_anim;
   LTexture doorE_left_closed_anim;
   LTexture doorE_left_broke_in_anim;
   LTexture doorE_left_broke_out_anim;
   SDL_Rect doorE_left_part1Clip;
   SDL_Rect doorE_left_part2Clip;
   SDL_Rect doorE_left_doorClip;

   LTexture doorE_right_open;
   LTexture doorE_right_closed;
   LTexture doorE_right_broke_in;
   LTexture doorE_right_broke_out;
   LTexture doorE_right_open_anim;
   LTexture doorE_right_closed_anim;
   LTexture doorE_right_broke_in_anim;
   LTexture doorE_right_broke_out_anim;
   SDL_Rect doorE_right_part1Clip;
   SDL_Rect doorE_right_part2Clip;
   SDL_Rect doorE_right_doorClip;

   LTexture doorW_left_open;
   LTexture doorW_left_closed;
   LTexture doorW_left_broke_in;
   LTexture doorW_left_broke_out;
   LTexture doorW_left_open_anim;
   LTexture doorW_left_closed_anim;
   LTexture doorW_left_broke_in_anim;
   LTexture doorW_left_broke_out_anim;
   SDL_Rect doorW_left_part1Clip;
   SDL_Rect doorW_left_part2Clip;
   SDL_Rect doorW_left_doorClip;

   LTexture doorW_right_open;
   LTexture doorW_right_closed;
   LTexture doorW_right_broke_in;
   LTexture doorW_right_broke_out;
   LTexture doorW_right_open_anim;
   LTexture doorW_right_closed_anim;
   LTexture doorW_right_broke_in_anim;
   LTexture doorW_right_broke_out_anim;
   SDL_Rect doorW_right_part1Clip;
   SDL_Rect doorW_right_part2Clip;
   SDL_Rect doorW_right_doorClip;

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
};

#endif
