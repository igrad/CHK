#include "..\include\MouseEvents.h"

vector<ClickRegion*> ClickRegion::clickables = {};
DropMenu* DropMenu::focusedDM = NULL;
vector<DropMenu*> DropMenu::pendingDMs = {};

ClickRegion::ClickRegion() {
   clickRect = {0, 0, 0, 0};
   crType = CR_ABSOLUTE;

   clickActive = true;

   for (auto cb: callbackSet) cb = false;
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

bool ClickRegion::OnLeftClick() {
   if (callbackSet[LEFTCLICK]) {
      MouseEventData* a = &callbacks[LEFTCLICK];
      return a->proc(a->par1, a->par2, a->par3);
   }
   return false;
}

bool ClickRegion::OnRightClick() {
   if (callbackSet[RIGHTCLICK]) {
      MouseEventData* a = &callbacks[RIGHTCLICK];
      return a->proc(a->par1, a->par2, a->par3);
   }
   return false;
}

bool ClickRegion::OnScrollUp() {
   if (callbackSet[SCROLLUP]) {
      MouseEventData* a = &callbacks[SCROLLUP];
      return a->proc(a->par1, a->par2, a->par3);
   }
   return false;
}

bool ClickRegion::OnScrollDown() {
   if (callbackSet[SCROLLDOWN]) {
      MouseEventData* a = &callbacks[SCROLLDOWN];
      return a->proc(a->par1, a->par2, a->par3);
   }
   return false;
}

DropMenu* ClickRegion::GetDM() {
   return linkedDM;
}

vector<ClickRegion*> ClickRegion::GetRegionsAtMouse(int mx, int my) {
   vector<ClickRegion*> results;
   vector<ClickRegion*>::iterator i = clickables.begin();

   int sx = mx - Camera::x;
   int sy = my - Camera::y;
   while (i != clickables.end()) {
      if ((*i)->crType == CR_ABSOLUTE) {
         // printf("\nChecking CR_ABSOLUTE: {%4i %4i} %4i %4i %4i %4i", sx, sy,
         // (*i)->clickRect.x, (*i)->clickRect.y,
         // (*i)->clickRect.w, (*i)->clickRect.h);
         if (PointInRect(sx, sy, &((*i)->clickRect))) results.push_back(*i);
      } else {
         // printf("\nChecking CR_RELATIVE: {%4i %4i} %4i %4i %4i %4i", mx, my,
         // (*i)->clickRect.x, (*i)->clickRect.y,
         // (*i)->clickRect.w, (*i)->clickRect.h);
         if (PointInRect(mx, my, &((*i)->clickRect))) results.push_back(*i);
      }
      advance(i, 1);
   }

   return results;
}

void ClickRegion::RemoveClickable(ClickRegion* r) {
   auto i = find(clickables.begin(), clickables.end(), r);
   if (i != clickables.end()) {
      if (clickables.size() == 1) clickables.clear();
      else clickables.erase(i);
   }
}



ClickButton::ClickButton() {
   // Do nothing, used for creating blank arrays of ClickButtons
}

ClickButton::ClickButton(string textStr, int fontSize, TTF_Font* fontStyle,
   SDL_Color* fontColor, int padding, SDL_Rect* clickRect, SDL_Rect* drawRect,
   CLICKREGION_TYPE ct, string bgPath):
   ClickRegion(clickRect, ct) {
   texture = new LTexture();
   path = bgPath;
   text = textStr;
   font = fontStyle;

   LTexture tempText;
   if (bgPath != "") tempText.LoadFromFile(bgPath);
   if (clickRect != NULL) this->clickRect = *clickRect;
   if (drawRect != NULL) this->drawRect = *drawRect;
   int w = clickRect->w;
   int h = clickRect->h;

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
   SDL_Color* fontColor, int padding, SDL_Rect* clickRect, SDL_Rect* drawRect,
   CLICKREGION_TYPE ct, LTexture* bg):
   ClickRegion(clickRect, ct) {
   texture = new LTexture();
   text = textStr;
   font = fontStyle;

   LTexture tempText;
   if (bg != NULL) tempText = *bg;
   if (clickRect != NULL) this->clickRect = *clickRect;
   if (drawRect != NULL) this->drawRect = *drawRect;
   int w = clickRect->w * GZOOM;
   int h = clickRect->h * GZOOM;

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
   if (r != NULL) drawRect = *r;

   if (crType == CR_ABSOLUTE) {
      texture->Render(&drawRect);
   } else if (crType == CR_RELATIVE) {
      texture->Render(drawRect.x, drawRect.y);
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

   btnRect = {(int)(margin * GZOOM), (int)(margin * GZOOM),
      (int)(UIScheme->btnW * GZOOM), (int)(UIScheme->btnH * GZOOM)};
   area = {(int)(x * GZOOM), (int)(y * GZOOM),
      (int)(((UIScheme->btnW * cols) + (2 * margin) +
      (separation * (cols - 1))) * GZOOM),
      (int)(((UIScheme->btnH * rows) + (2 * margin) +
      (separation * (rows - 1))) * GZOOM)
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

   btnRect = {(int)(margin * GZOOM), (int)(margin * GZOOM),
      (int)(btnW * GZOOM), (int)(btnH * GZOOM)};
   area = {(int)(x * GZOOM), (int)(y * GZOOM),
      (int)(((btnW * cols) + (2 * margin) + (separation * (cols - 1))) *
      GZOOM),
      (int)(((btnH * rows) + (2 * margin) + (separation * (rows - 1))) *
      GZOOM)
   };
}

void DropMenu::ClearButtons() {
   buttons.clear();
}

void DropMenu::Open(int x, int y) {
   // Important that this comes after all of the AddButton calls are completed,
   // or we can see some buttons missing from the DM
   rendering = true;

   // We can add opening or closing animations here later on if needed

   for (int i = 0; i < buttons.size(); i++) {
      ClickRegion::clickables.push_back(&buttons[i]);
   }
}

void DropMenu::Close() {
   for (int i = 0; i < buttons.size(); i++) {
      ClickRegion::RemoveClickable(&buttons[i]);
   }

   rendering = false;
   DropMenu::RemovePendingDM(this);
}

void DropMenu::Render() {
   if (rendering) {
      background->Render(&area);
      for (int i = 0; i < buttons.size(); i++) buttons[i].Render();
   }
}

vector<DropMenu*> DropMenu::GetDMsAtMouse(int mx, int my) {
   vector<DropMenu*> results;
   vector<DropMenu*>::iterator i = pendingDMs.begin();

   int x = mx;
   int y = my;
   int sx = mx - Camera::x;
   int sy = my - Camera::y;
   while (i != pendingDMs.end()) {
      // printf("\nChecking DM collisions: %i, %i {%i %i %i %i}", x, y,
      // (*i)->area.x, (*i)->area.y, (*i)->area.w, (*i)->area.h);

      if ((*i)->ct == CR_ABSOLUTE) {
         if (PointInRect(sx, sy, &((*i)->area))) {
            results.push_back(*i);
            (*i)->hovered = true;
         }
      } else {
         if (PointInRect(mx, my, &((*i)->area))) {
            results.push_back(*i);
            (*i)->hovered = true;
         }
      }
      advance(i, 1);
   }

   return results;
}

void DropMenu::AddPendingDM(DropMenu* dm) {
   pendingDMs.push_back(dm);
   focusedDM = dm;
}

void DropMenu::RemovePendingDM(DropMenu* dm) {
   auto i = find(pendingDMs.begin(), pendingDMs.end(), dm);
   if (i != pendingDMs.end()) {
      if (pendingDMs.size() == 1) pendingDMs.clear();
      else pendingDMs.erase(i);
      //erase(pendingDMs.begin(), pendingDMs.end(), dm);
      dm->hovered = false;
   }

   if (pendingDMs.size() > 0) focusedDM = pendingDMs.back();
   else focusedDM = NULL;
}

void DropMenu::ClearPendingDMs() {
   pendingDMs.clear();
   focusedDM = NULL;
}
