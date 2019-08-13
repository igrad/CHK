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

ClickButton::ClickButton(string textStr, int fontSize, TTF_Font* fontStyle,
   SDL_Color* fontColor, int padding, SDL_Rect* area, CLICKREGION_TYPE ct,
   string bgPath):
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

   int tw, th;
   tempText.LoadFromRenderedText(textStr, fontColor, fontStyle, &tw, &th);

   // Shrink the text to fit the font size
   double woh = tw/th;
   double fontH = fontSize * GZOOM;
   r = {
      padding,       // This assumes we want L-justified text (liable to change)
      (int)((h - fontH)/2), // Assumes vertically-centered text
      (int)(tw * fontH/th),
      (int)(fontH)
   };
   tempText.Render(&r);

   SDL_SetRenderTarget(gRenderer, NULL);
}

ClickButton::ClickButton(string textStr, int fontSize, TTF_Font* fontStyle,
   SDL_Color* fontColor, int padding, SDL_Rect* area, CLICKREGION_TYPE ct,
   LTexture* bg):
   ClickRegion(area, ct) {
   texture = new LTexture();
   text = textStr;
   font = fontStyle;

   LTexture tempText;
   if (bg != NULL) tempText = *bg;
   int w = area->w * GZOOM;
   int h = area->h * GZOOM;

   texture->CreateBlank(w, h);

   SDL_Rect r = {0, 0, w, h};
   SDL_SetRenderTarget(gRenderer, texture->mTexture);
   tempText.Render(NULL);

   int tw, th;
   tempText.LoadFromRenderedText(textStr, fontColor, fontStyle, &tw, &th);

   // Shrink the text to fit the font size
   double woh = tw/th;
   double fontH = fontSize * GZOOM;
   r = {
      padding,       // This assumes we want L-justified text (liable to change)
      (int)((h - fontH)/2), // Assumes vertically-centered text
      (int)(tw * fontH/th),
      (int)(fontH)
   };
   tempText.Render(&r);

   SDL_SetRenderTarget(gRenderer, NULL);
}

void ClickButton::Render(SDL_Rect* r) {
   if (r != NULL) clickRect = *r;

   if (crType == CR_ABSOLUTE) {
      texture->Render(&clickRect);
   } else if (crType == CR_RELATIVE) {
      texture->Render(clickRect.x, clickRect.y);
   }
}



DropMenu::DropMenu(int x, int y, int cols, int rows, CLICKREGION_TYPE ct,
   DropMenuUI* UIScheme) {
   // Rule of thumb: never make more than 8 rows
   this->margin = UIScheme->margin;
   this->separation = UIScheme->separation;
   this->ct = ct;
   this->fontSize = UIScheme->fontSize;
   this->fontStyle = UIScheme->fontStyle;
   this->fontColor = UIScheme->fontColor;
   this->background = UIScheme->bg;
   this->btnbg = UIScheme->btnbg;
   this->btnPadding = UIScheme->btnPadding;

   rendering = false;

   btnRect = {margin * GZOOM, margin * GZOOM,
      UIScheme->btnW * GZOOM, UIScheme->btnH * GZOOM};
   area = {x * GZOOM, y * GZOOM,
      ((UIScheme->btnW * cols) + (2 * margin) + (separation * (cols - 1))) *
      GZOOM,
      ((UIScheme->btnH * rows) + (2 * margin) + (separation * (rows - 1))) *
      GZOOM
   };
}

DropMenu::DropMenu(int x, int y, int cols, int rows, int btnW, int btnH,
   int margin, int separation, int fontSize, TTF_Font* fontStyle,
   SDL_Color* fontColor, int btnPadding, CLICKREGION_TYPE ct, LTexture* bg,
   LTexture* btnbg) {
   // Rule of thumb: never make more than 8 rows
   this->margin = margin;
   this->separation = separation;
   this->ct = ct;
   this->fontSize = fontSize;
   this->fontStyle = fontStyle;
   this->fontColor = fontColor;
   this->background = bg;
   this->btnbg = btnbg;
   this->btnPadding = btnPadding;

   rendering = false;

   btnRect = {margin * GZOOM, margin * GZOOM, btnW * GZOOM, btnH * GZOOM};
   area = {x * GZOOM, y * GZOOM,
      ((btnW * cols) + (2 * margin) + (separation * (cols - 1))) * GZOOM,
      ((btnH * rows) + (2 * margin) + (separation * (rows - 1))) * GZOOM
   };
}

void DropMenu::ClearButtons() {
   buttons.clear();
}

void DropMenu::Open(int x, int y) {
   rendering = true;
   // We can add opening or closing animations later on if needed
}

void DropMenu::Close() {
   rendering = false;
}

void DropMenu::Render() {
   if (rendering) {
      background->Render(&area);
      for (int i = 0; i < buttons.size(); i++) {
         SDL_Rect* r = &buttons[i].clickRect;
         buttons[i].Render();
      }
   }
}
