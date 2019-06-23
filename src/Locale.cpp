#include "..\include\Locale.h"

Locale::Locale() {

}



void Locale::Initialize(string prefix) {
   this->floorTexture.LoadFromFile(prefix + "floor1.png");
   this->voidTexture.LoadFromFile(prefix + "void.png");
	this->wallTexture_Face1.LoadFromFile(prefix + "wall1.png");
	this->wallTexture_Face2.LoadFromFile(prefix + "wall2.png");
	this->wallTexture_Face3.LoadFromFile(prefix + "wall3.png");
	this->wallTexture_LEdge.LoadFromFile(prefix + "wall_edge_l.png");
	this->wallTexture_REdge.LoadFromFile(prefix + "wall_edge_r.png");
	this->wallTexture_TEdge.LoadFromFile(prefix + "wall_edge_t.png");
	this->wallTexture_LCorner.LoadFromFile(prefix + "wall_corner_l.png");
	this->wallTexture_RCorner.LoadFromFile(prefix + "wall_corner_r.png");
	this->doorN_shut.LoadFromFile(prefix + "doorN_shut.png");
	this->doorN_opening_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorN_opening_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorN_closing_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorN_closing_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorN_break_N.LoadFromFile(prefix + "doorN_shut.png");
	this->doorN_break_S.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_shut.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_open.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_opening_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_opening_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_closing_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_closing_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_break_E.LoadFromFile(prefix + "doorN_shut.png");
	this->doorE_break_W.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_shut.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_open.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_opening_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_opening_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_closing_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_closing_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_break_S.LoadFromFile(prefix + "doorN_shut.png");
	this->doorS_break_N.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_shut.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_open.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_opening_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_opening_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_closing_slow.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_closing_fast.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_break_W.LoadFromFile(prefix + "doorN_shut.png");
	this->doorW_break_E.LoadFromFile(prefix + "doorN_shut.png");
}
