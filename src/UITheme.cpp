#include "..\include\UITheme.h"

SDL_Color FONTC_WHITE = {.r = 255, .g = 255, .b = 255};
SDL_Color FONTC_BLACK = {.r = 0, .g = 0, .b = 0};
SDL_Color FONTC_OFFWHITE = {.r = 240, .g = 240, .b = 240};
SDL_Color FONTC_OFFBLACK = {.r = 12, .g = 12, .b = 12};

DropMenuUI::DropMenuUI() {
   bg = new LTexture();
   btnbg = new LTexture();

   fontSize = 20;
   fontStyle = NULL;
   fontColor = NULL;

   margin = 0;
   separation = 0;

   btnW = 0;
   btnH = 0;
   btnPadding = 0;
}

UITheme::UITheme() {

}
