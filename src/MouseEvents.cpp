#include "..\include\MouseEvents.h"

ClickRegion::ClickRegion() {
   this->id = "";
   SDL_Rect r = {0, 0, 0, 0};
   rect = &r;
   crType = CR_ABSOLUTE;

   active = true;
   displayed = true;
}

ClickRegion::ClickRegion(string id, SDL_Rect* area, CLICKREGION_TYPE ct) {
   this->id = id;
   rect = area;
   crType = ct;

   active = true;
   displayed = true;
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

void ClickRegion::OnLeftClick() {
   Log("Left click");
}

void ClickRegion::OnRightClick() {
   Log("Right click");
}

void ClickRegion::OnScrollUp() {
   Log("Scrolled up");
}

void ClickRegion::OnScrollDown() {
   Log("Scrolled down");
}


ClickButton::ClickButton(string id, string textStr, TTF_Font* fontStyle,
   SDL_Color* fontColor, SDL_Rect* area, CLICKREGION_TYPE ct, string bgPath):
   ClickRegion(id, area, ct) {
   texture = new LTexture();
   path = bgPath;
   text = textStr;
   font = fontStyle;

   LTexture tempText;
   if (bgPath != "") tempText.LoadFromFile(bgPath);
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
