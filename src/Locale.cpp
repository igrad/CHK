#include "..\include\Locale.h"

Locale::Locale() {

}



void Locale::Initialize(string prefix) {
   // First, we initialize the Locale numerical properties based on the
   // provided prefix
   if (prefix == "dungeon") InitDungeon();
   // Add future locales here, calling their respective functions

   // Next, we load up the images from storage
   prefix = "media\\images\\" + prefix + "\\";
   floorTexture.LoadFromFile(prefix + "floor1.png");
   voidTexture.LoadFromFile(prefix + "void.png");
	wallTexture_Face1.LoadFromFile(prefix + "wall1.png");
	wallTexture_Face2.LoadFromFile(prefix + "wall2.png");
	wallTexture_Face3.LoadFromFile(prefix + "wall3.png");
	wallTexture_LEdge.LoadFromFile(prefix + "wall_edge_l.png");
	wallTexture_REdge.LoadFromFile(prefix + "wall_edge_r.png");
	wallTexture_TEdge.LoadFromFile(prefix + "wall_edge_t.png");
	wallTexture_LCorner.LoadFromFile(prefix + "wall_corner_l.png");
	wallTexture_RCorner.LoadFromFile(prefix + "wall_corner_r.png");

   doorNS_right_inside_open.LoadFromFile(prefix +
      "doorNS_right_inside_open.png");
   doorNS_right_inside_closed.LoadFromFile(prefix +
      "doorNS_right_inside_closed.png");
   doorNS_right_inside_open_anim.LoadFromFile(prefix +
      "doorNS_right_inside_open_anim.png");
   doorNS_right_inside_close_anim.LoadFromFile(prefix +
      "doorNS_right_inside_close_anim.png");
   doorNS_right_inside_break_out_anim.LoadFromFile(prefix +
      "doorNS_right_inside_break_out_anim.png");
   doorNS_right_inside_break_in_anim.LoadFromFile(prefix +
      "doorNS_right_inside_break_in_anim.png");

   doorNS_left_inside_open.LoadFromFile(prefix +
      "doorNS_left_inside_open.png");
   doorNS_left_inside_closed.LoadFromFile(prefix +
      "doorNS_left_inside_closed.png");
   doorNS_left_inside_open_anim.LoadFromFile(prefix +
      "doorNS_left_inside_open_anim.png");
   doorNS_left_inside_close_anim.LoadFromFile(prefix +
      "doorNS_left_inside_close_anim.png");
   doorNS_left_inside_break_out_anim.LoadFromFile(prefix +
      "doorNS_left_inside_break_out_anim.png");
   doorNS_left_inside_break_in_anim.LoadFromFile(prefix +
      "doorNS_left_inside_break_in_anim.png");

   doorNS_right_outside_open.LoadFromFile(prefix +
      "doorNS_right_outside_open.png");
   doorNS_right_outside_closed.LoadFromFile(prefix +
      "doorNS_right_outside_closed.png");
   doorNS_right_outside_open_anim.LoadFromFile(prefix +
      "doorNS_right_outside_open_anim.png");
   doorNS_right_outside_close_anim.LoadFromFile(prefix +
      "doorNS_right_outside_close_anim.png");
   doorNS_right_outside_break_out_anim.LoadFromFile(prefix +
      "doorNS_right_outside_break_out_anim.png");
   doorNS_right_outside_break_in_anim.LoadFromFile(prefix +
      "doorNS_right_outside_break_in_anim.png");

   doorNS_left_outside_open.LoadFromFile(prefix +
      "doorNS_left_outside_open.png");
   doorNS_left_outside_closed.LoadFromFile(prefix +
      "doorNS_left_outside_closed.png");
   doorNS_left_outside_open_anim.LoadFromFile(prefix +
      "doorNS_left_outside_open_anim.png");
   doorNS_left_outside_close_anim.LoadFromFile(prefix +
      "doorNS_left_outside_close_anim.png");
   doorNS_left_outside_break_out_anim.LoadFromFile(prefix +
      "doorNS_left_outside_break_out_anim.png");
   doorNS_left_outside_break_in_anim.LoadFromFile(prefix +
      "doorNS_left_outside_break_in_anim.png");

   doorE_left_open.LoadFromFile(prefix + "doorE_left_open.png");
   doorE_left_closed.LoadFromFile(prefix + "doorE_left_closed.png");
   doorE_left_broke_in.LoadFromFile(prefix + "doorE_left_broke_in.png");
   doorE_left_broke_out.LoadFromFile(prefix + "doorE_left_broke_out.png");

   doorE_right_open.LoadFromFile(prefix + "doorE_right_open.png");
   doorE_right_closed.LoadFromFile(prefix + "doorE_right_closed.png");
   doorE_right_broke_in.LoadFromFile(prefix + "doorE_right_broke_in.png");
   doorE_right_broke_out.LoadFromFile(prefix + "doorE_right_broke_out.png");

   doorW_left_open.LoadFromFile(prefix + "doorW_left_open.png");
   doorW_left_closed.LoadFromFile(prefix + "doorW_left_closed.png");
   doorW_left_broke_in.LoadFromFile(prefix + "doorW_left_broke_in.png");
   doorW_left_broke_out.LoadFromFile(prefix + "doorW_left_broke_out.png");

   doorW_right_open.LoadFromFile(prefix + "doorW_right_open.png");
   doorW_right_closed.LoadFromFile(prefix + "doorW_right_closed.png");
   doorW_right_broke_in.LoadFromFile(prefix + "doorW_right_broke_in.png");
   doorW_right_broke_out.LoadFromFile(prefix + "doorW_right_broke_out.png");
}



void Locale::InitDungeon() {
   mapSize = 50;
   minRoomDim = 3;
   maxRoomDim = 9;
   minRoomCount = 8;
   maxRoomCount = 18;
   cornerType = CORNER_SHARP;
   cornerSize = 1;
   corridorType = CORRIDOR_CORNERS;
   corridorSize = 1;

   doorSize = 1;
   doorPercentChance = 100;

   floorTextureSize = 320;
   wallHeight = 43;
   voidTextureSize = 640;

   doorNSChangeRate_slow = 1.5;
   doorNSChangeRate_fast = 0.5;
   doorEChangeRate_slow = 1.5;
   doorEChangeRate_fast = 0.5;
   doorWChangeRate_slow = 1.5;
   doorWChangeRate_fast = 0.5;

   doorNS_right_inside_part1Clip = {0, 20, 9, 23};
   doorNS_right_inside_part2Clip = {32, 20, 9, 23};
   doorNS_right_inside_doorClip = {10, 20, 22, 23};
   doorNS_left_inside_part1Clip = {0, 20, 9, 23};
   doorNS_left_inside_part2Clip = {32, 20, 9, 23};
   doorNS_left_inside_doorClip = {10, 20, 22, 23};
   doorNS_right_outside_part1Clip = {0, 20, 9, 23};
   doorNS_right_outside_part2Clip = {32, 20, 9, 23};
   doorNS_right_outside_doorClip = {10, 20, 22, 23};
   doorNS_left_outside_part1Clip = {0, 20, 9, 23};
   doorNS_left_outside_part2Clip = {32, 20, 9, 23};
   doorNS_left_outside_doorClip = {10, 20, 22, 23};
   // doorE_left_part1Clip = {};
   // doorE_left_part2Clip;
   // doorE_left_doorClip;
   // doorE_right_part1Clip;
   // doorE_right_part2Clip;
   // doorE_right_doorClip;
   // doorW_left_part1Clip;
   // doorW_left_part2Clip;
   // doorW_left_doorClip;
   // doorW_right_part1Clip;
   // doorW_right_part2Clip;
   // doorW_right_doorClip;
}
