#ifndef UITHEME
#define UITHEME

#include "essentials.h"
#include "LTexture.h"

struct DropMenuUI {
   DropMenuUI();

   LTexture* bg;
   LTexture* btnbg;

   int fontSize;
   TTF_Font* fontStyle;
   SDL_Color* fontColor;

   int margin;
   int separation;

   int btnW;
   int btnH;
   int btnPadding;
};

struct UITheme {
   UITheme();

   // Door drop menu
   DropMenuUI doorDM;
};

extern UITheme UI;

// Fonts
extern TTF_Font* FONT_SCROLL;
extern TTF_Font* FONT_HAND;
extern TTF_Font* FONT_TYPED;
extern TTF_Font* FONT_TYPED_BOLD;
extern TTF_Font* FONT_TYPED_ITALIC;

// Common font colors
extern SDL_Color FONTC_WHITE;
extern SDL_Color FONTC_BLACK;
extern SDL_Color FONTC_OFFWHITE;
extern SDL_Color FONTC_OFFBLACK;

#endif
