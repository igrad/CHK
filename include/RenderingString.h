#ifndef RENDERINGSTRING_DEFINED
#define RENDERINGSTRING_DEFINED

#include "essentials.h"
#include "LTexture.h"

enum TEXT_ALIGNMENT {
   ALIGN_LEFT,
   ALIGN_CENTER,
   ALIGN_RIGHT
};

enum TEXT_VERT_ANCHOR {
   VERT_ANCHOR_TOP,
   VERT_ANCHOR_CENTER,
   VERT_ANCHOR_BOTTOM
};

class RenderingString : public LTexture {
public:
   RenderingString();
   RenderingString(string str, SDL_Color* textColor, 
      TTF_Font* font, TEXT_ALIGNMENT alignment, TEXT_VERT_ANCHOR vertAnchor, 
      int fontHeight);

   void Render(SDL_Rect* drawBox, SDL_Rect* clip = NULL);

   string str;
   SDL_Color* textColor;
   TTF_Font* font;
   TEXT_ALIGNMENT alignment;
   TEXT_VERT_ANCHOR vertAnchor;
   int fontHeight;

   bool stretchToFit;
};

#endif