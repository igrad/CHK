#include "..\include\RenderingString.h"

RenderingString::RenderingString() {

}

RenderingString::RenderingString(string str, SDL_Color* textColor, 
   TTF_Font* font, TEXT_ALIGNMENT alignment, TEXT_VERT_ANCHOR vertAnchor, 
   int fontHeight) {
   this->str = str;
   this->textColor = textColor;
   this->font = font;
   this->alignment = alignment;
   this->vertAnchor = vertAnchor;
   this->fontHeight = fontHeight;

   stretchToFit = false;
}

void RenderingString::Render(SDL_Rect* drawBox, SDL_Rect* clip) {
   int rw, rh;
   LoadFromRenderedText(str, textColor, font, &rw, &rh);

   if (!stretchToFit) {
      mHeight = fontHeight * GZOOM;
      mWidth = (int)(rw * GZOOM * fontHeight / rh);

      switch (alignment) {
         case ALIGN_CENTER:
            drawBox->x += (int)((drawBox->w - mWidth)/2);
            break;
         case ALIGN_RIGHT:
            drawBox->x += drawBox->w - mWidth;
            break;
      }

      switch (vertAnchor) {
         case VERT_ANCHOR_CENTER:
            drawBox->y += (int)((drawBox->h - mHeight)/2);
            break;
         case VERT_ANCHOR_BOTTOM:
            drawBox->y += drawBox->h - mHeight;
      }

      drawBox->w = mWidth;
      drawBox->h = mHeight;
   }

   LTexture::Render(drawBox, clip);
}