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

void ClickRegion::OnHover(int mx, int my) {
   if (callbackSet[HOVER]) {
      MouseEventData* a = &callbacks[HOVER];
      a->proc(mx, my, a->par3);
   }
}

bool ClickRegion::OnLeftClick(int mx, int my) {
   if (callbackSet[LEFTCLICK]) {
      MouseEventData* a = &callbacks[LEFTCLICK];
      return a->proc(mx, my, a->par3);
   }
   return false;
}

bool ClickRegion::OnRightClick(int mx, int my) {
   if (callbackSet[RIGHTCLICK]) {
      MouseEventData* a = &callbacks[RIGHTCLICK];
      return a->proc(mx, my, a->par3);
   }
   return false;
}

bool ClickRegion::OnScrollUp(int mx, int my) {
   if (callbackSet[SCROLLUP]) {
      MouseEventData* a = &callbacks[SCROLLUP];
      return a->proc(mx, my, a->par3);
   }
   return false;
}

bool ClickRegion::OnScrollDown(int mx, int my) {
   if (callbackSet[SCROLLDOWN]) {
      MouseEventData* a = &callbacks[SCROLLDOWN];
      return a->proc(mx, my, a->par3);
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

   loadedFromReference = false;

   texture = new LTexture();
   path = bgPath;
   text = textStr;
   font = fontStyle;
   displaceSet = false;

   bool autosize = (drawRect == NULL);
   if (clickRect != NULL) this->clickRect = *clickRect;
   if (!autosize) this->drawRect = *drawRect;

   // Make a temporary texture that will help us render the text
   LTexture tempText;
   int tw, th;
   if (bgPath != "") tempText.LoadFromFile(bgPath);
   tempText.Render(NULL);
   tempText.LoadFromRenderedText(textStr, fontColor, fontStyle, &tw, &th);
   th = TTF_FontHeight(fontStyle);

   // Shrink the text to fit the font size
   double woh = tw/th;
   double fontH = fontSize * GZOOM;

   // TODO: If we're making an auto-sized button, we want to use 'padding' for
   // the y value here. If not, use '(int)((th - fontH)/2)'
   SDL_Rect tempr = {
      padding, // This assumes we want L-justified text (liable to change)
      padding, // Used only for auto-sized buttons
      (int)(tw * fontH/th) + (2 * padding),
      (int)(fontH) + (2 * padding)
   };

   // Size up the LTexture that we blit our temptext to
   int w, h;
   if (!autosize) {
      w = drawRect->w;
      h = drawRect->h;
   } else {
      w = tempr.w;
      h = tempr.h;
   }

   texture->CreateBlank(w, h);
   SDL_SetRenderTarget(gRenderer, texture->mTexture);

   // Now we can render our text onto the proper texture
   tempText.Render(&tempr);

   SDL_SetRenderTarget(gRenderer, NULL);
}

ClickButton::ClickButton(string textStr, int fontSize, TTF_Font* fontStyle,
   SDL_Color* fontColor, int padding, SDL_Rect* clickRect, SDL_Rect* drawRect,
   CLICKREGION_TYPE ct, LTexture* bg):
   ClickRegion(clickRect, ct) {

   loadedFromReference = true;

   texture = new LTexture();
   text = textStr;
   font = fontStyle;
   displaceSet = false;

   if (clickRect != NULL) this->clickRect = *clickRect;
   if (drawRect != NULL) this->drawRect = *drawRect;
   int w = drawRect->w * GZOOM;
   int h = drawRect->h * GZOOM;

   texture->CreateBlank(w, h);

   SDL_Rect r = {0, 0, w, h};
   SDL_SetRenderTarget(gRenderer, texture->mTexture);
   if (bg != NULL) bg->Render(NULL);

   int tw, th;
   LTexture tempText;
   tempText.LoadFromRenderedText(textStr, fontColor, fontStyle, &tw, &th);
   th = TTF_FontHeight(fontStyle);

   // Shrink the text to fit the font size
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

void ClickButton::Displace(int ax, int ay) {
   if (!displaceSet) {
      dX = ax;
      dY = ay;
      clickRect.x += dX;
      clickRect.y += dY;
      drawRect.x += dX;
      drawRect.y += dY;

      displaceSet = true;
   }
}

void ClickButton::ResetDisplace() {
   if (displaceSet) {
      clickRect.x -= dX;
      clickRect.y -= dY;
      drawRect.x -= dX;
      drawRect.y -= dY;

      displaceSet = false;
   }
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
   this->margin = (int)(UIScheme->margin * GZOOM);
   this->separation = (int)(UIScheme->separation * GZOOM);
   this->ct = ct;
   this->fontSize = (int)(UIScheme->fontSize * GZOOM);
   this->fontStyle = UIScheme->fontStyle;
   this->fontColor = UIScheme->fontColor;
   this->background = UIScheme->bg;
   this->btnbg = UIScheme->btnbg;
   this->btnPadding = (int)(UIScheme->btnPadding);

   rendering = false;

   btnRect = {margin, margin,
      (int)(UIScheme->btnW * GZOOM), (int)(UIScheme->btnH)};
   area = {x, y,
      (btnRect.w*cols) + (2*margin) + (separation*(cols-1)),
      (btnRect.h*rows) + (2*margin) + (separation*(rows-1))
   };
}

DropMenu::DropMenu(int x, int y, int cols, int rows, int btnW, int btnH,
   int margin, int separation, int fontSize, TTF_Font* fontStyle,
   SDL_Color* fontColor, int btnPadding, CLICKREGION_TYPE ct, LTexture* bg,
   LTexture* btnbg) {
   // Rule of thumb: never make more than 8 rows
   this->margin = (int)(margin * GZOOM);
   this->separation = (int)(separation * GZOOM);
   this->ct = ct;
   this->fontSize = (int)(fontSize * GZOOM);
   this->fontStyle = fontStyle;
   this->fontColor = fontColor;
   this->background = bg;
   this->btnbg = btnbg;
   this->btnPadding = (int)(btnPadding * GZOOM);

   rendering = false;

   btnRect = {this->margin, this->margin,
      (int)(btnW * GZOOM), (int)(btnH * GZOOM)};
   area = {x, y,
      (btnRect.w*cols) + (2*this->margin) + (this->separation*(cols-1)),
      (btnRect.h*rows) + (2*this->margin) + (this->separation*(rows-1))
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

   // Recalculate the background size, as DMs are liable to change the number of
   // options over time
   // This is also where we can determine proximity to window edge and
   // adjust the X/y coords as needed
   int btnc = buttons.size();
   int cols = floor(btnc / 8) + 1;
   int rows = btnc % 8;
   int btnW = btnRect.w;
   int btnH = btnRect.h;
   area = {x, y,
      (btnW*cols) + ((2*margin) + (separation*(cols-1))),
      (btnH*rows) + ((2*margin) + (separation*(rows-1)))
   };

   if ((x + area.w) > SCREEN_WIDTH) area.x -= area.w;
   if ((y + area.h) > SCREEN_HEIGHT) area.y -= area.h;

   for (int i = 0; i < buttons.size(); i++) buttons[i].Displace(area.x, area.y);

   // Push the updated buttons to the clickables vector
   for (int i = 0; i < buttons.size(); i++) {
      ClickRegion::clickables.push_back(&buttons[i]);
   }

   DropMenu::AddPendingDM(this);
}

void DropMenu::Close() {
   for (int i = 0; i < buttons.size(); i++) {
      buttons[i].ResetDisplace();
      ClickRegion::RemoveClickable(&buttons[i]);
   }

   rendering = false;
   DropMenu::RemovePendingDM(this);
}

void DropMenu::Render() {
   if (rendering) {
      background->Render(&area);
      for (int i = 0; i < buttons.size(); i++) {
         buttons[i].Render();
      }
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

// find where this is called in main
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

   if (focusedDM == dm) {
      if (pendingDMs.size() > 0) focusedDM = pendingDMs.back();
      else focusedDM = NULL;
   }
}

void DropMenu::ClearPendingDMs() {
   pendingDMs.clear();
   focusedDM = NULL;
}

void DropMenu::Free() {
   ClearButtons();
   RemovePendingDM(this);
}
