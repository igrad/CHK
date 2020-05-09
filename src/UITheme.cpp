#include "..\include\UITheme.h"

SDL_Color FONTC_WHITE = {255, 255, 255};
SDL_Color FONTC_BLACK = {0, 0, 0};
SDL_Color FONTC_OFFWHITE = {240, 240, 240};
SDL_Color FONTC_OFFBLACK = {12, 12, 12};

DropMenuUI::DropMenuUI() {
   bg = new LTexture();
   btnbg = new LTexture();

   fontSize = 14;
   fontStyle = NULL;
   fontColor = NULL;

   margin = 0;
   separation = 0;

   btnW = 0;
   btnH = 0;
   btnPadding = 0;
}

DropMenuUI::~DropMenuUI() {
   delete bg;
   delete btnbg;
}

UITheme::UITheme() {

}
