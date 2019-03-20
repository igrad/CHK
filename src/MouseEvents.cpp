#include "..\include\MouseEvents.h"

ClickRegion::ClickRegion(SDL_Rect* area, CLICKREGION_TYPE ct) {
   rect = area;
   crType = ct;
}



void ClickRegion::SetPosition(int x, int y) {
   rect->x = x;
   rect->y = y;
}

void ClickRegion::SetPosition(SDL_Rect* r) {
   rect->x = r->x;
   rect->y = r->y;
}

void ClickRegion::SetSize(int w, int h) {
   rect->w = w;
   rect->h = h;
}

void ClickRegion::SetSize(SDL_Rect* r) {
   rect->w = r->w;
   rect->h = r->h;
}



ClickButton::ClickButton(string bgPath, string textStr, TTF_Font* fontStyle, SDL_Color* fontColor, SDL_Rect* area, CLICKREGION_TYPE ct) :
ClickRegion(area, ct) {
   path = bgPath;
   text = textStr;
   font = fontStyle;

   LTexture tempText;
   tempText.LoadFromFile(bgPath);
   int w = area->w;
   int h = area->h;

   texture->CreateBlank(w, h);

   SDL_Rect r = {0, 0, w, h};
   SDL_SetRenderTarget(gRenderer, texture->mTexture);
   tempText.Render(NULL);

   tempText.LoadFromRenderedText(textStr, fontColor, fontStyle);
   w = tempText.GetWidth();
   h = tempText.GetHeight();
   r = {0, 0, w, h};
   tempText.Render(&r);

   SDL_SetRenderTarget(gRenderer, NULL);
}



void ClickButton::Render() {
   if (crType == CR_ABSOLUTE) {
      texture->Render(rect);
   } else if (crType == CR_RELATIVE) {
      texture->Render(rect->x - Camera::x, rect->y - Camera::y);
   }
}



HintButton::HintButton(string text, TTF_Font* font, SDL_Color* color,
   SDL_Rect* area, CLICKREGION_TYPE crType) : ClickRegion(area, crType) {
   this->text = text;
   this->font = font;

   LTexture tempText;
   tempText.LoadFromRenderedText(text, color, font);
   int w = tempText.GetWidth();
   int h = tempText.GetHeight();

   this->texture->CreateBlank(w, h);

   SDL_Rect r = {0, 0, w, h};

   SDL_SetRenderTarget(gRenderer, this->texture->mTexture);
   tempText.Render(&r);

   SDL_SetRenderTarget(gRenderer, NULL);
}



void HintButton::Render() {
   if (crType == CR_ABSOLUTE) {
      texture->Render(rect);
   } else if (crType == CR_RELATIVE) {
      texture->Render(rect->x - Camera::x, rect->y - Camera::y);
   }
}
