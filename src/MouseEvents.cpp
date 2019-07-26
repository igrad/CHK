#include "..\include\MouseEvents.h"

ClickRegion::ClickRegion() {
   clickRect = {0, 0, 0, 0};
   crType = CR_ABSOLUTE;

   clickActive = true;

   for (int i = 0; i < 5; i++) callbackSet[i] = false;
}

ClickRegion::ClickRegion(SDL_Rect* area, CLICKREGION_TYPE ct) {
   clickRect = *area;
   crType = ct;

   clickActive = true;

   for (int i = 0; i < 5; i++) callbackSet[i] = false;
}

void ClickRegion::SetPosition(int x, int y) {
   clickRect.x = x;
   clickRect.y = y;
}

void ClickRegion::SetPosition(SDL_Rect* r) {
   clickRect.x = r->x;
   clickRect.y = r->y;
}

void ClickRegion::SetSize(int w, int h) {
   clickRect.w = w;
   clickRect.h = h;
}

void ClickRegion::SetSize(SDL_Rect* r) {
   clickRect.w = r->w;
   clickRect.h = r->h;
}

void ClickRegion::OnHover() {
   if (callbackSet[HOVER]) {
      MouseEventData* a = &callbacks[HOVER];
      a->proc(a->par1, a->par2, a->par3);
   }
}

void ClickRegion::OnLeftClick() {
   if (callbackSet[LEFTCLICK]) {
      MouseEventData* a = &callbacks[LEFTCLICK];
      a->proc(a->par1, a->par2, a->par3);
   }
}

void ClickRegion::OnRightClick() {
   if (callbackSet[RIGHTCLICK]) {
      MouseEventData* a = &callbacks[RIGHTCLICK];
      a->proc(a->par1, a->par2, a->par3);
   }
}

void ClickRegion::OnScrollUp() {
   if (callbackSet[SCROLLUP]) {
      MouseEventData* a = &callbacks[SCROLLUP];
      a->proc(a->par1, a->par2, a->par3);
   }
}

void ClickRegion::OnScrollDown() {
   if (callbackSet[SCROLLDOWN]) {
      MouseEventData* a = &callbacks[SCROLLDOWN];
      a->proc(a->par1, a->par2, a->par3);
   }
}



ClickButton::ClickButton() {
   // Do nothing, used for creating blank arrays of ClickButtons
}

ClickButton::ClickButton(string textStr, TTF_Font* fontStyle,
   SDL_Color* fontColor, SDL_Rect* area, CLICKREGION_TYPE ct, string bgPath):
   ClickRegion(area, ct) {
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
      texture->Render(&clickRect);
   } else if (crType == CR_RELATIVE) {
      texture->Render(clickRect.x - Camera::x, clickRect.y - Camera::y);
   }
}



DropMenu::DropMenu(int x, int y, int cols, int rows, int btnW, int btnH,
   int margin, int separation, TTF_Font* fontStyle, SDL_Color* fontColor,
   CLICKREGION_TYPE ct, string bgPath, string btnBgPath) {
   // Rule of thumb: never make more than 8 rows
   this->margin = margin;
   this->separation = separation;
   this->ct = ct;
   this->fontStyle = fontStyle;
   this->fontColor = fontColor;
   this->bgPath = bgPath;
   btnRect = {margin, margin, btnW, btnH};
   area = {x, y,
      (btnW * cols) + (2 * margin) + (separation * (cols - 1)),
      (btnH * rows) + (2 * margin) + (separation * (rows - 1))
   };
}

void DropMenu::AddButton(string textString) {
   btnRect.y = margin + (buttons.size() * (btnRect.h + separation));

   buttons.push_back(ClickButton(textString, fontStyle,
      fontColor, &btnRect, ct, btnBgPath));
}
