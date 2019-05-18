#ifndef MOUSEEVENTS
#define MOUSEEVENTS

#include "essentials.h"
#include "Camera.h"
#include "LTexture.h"

enum CLICKREGION_TYPE {
   CR_ABSOLUTE,
   CR_RELATIVE
};

class ClickRegion {
   public:
      ClickRegion();
      ClickRegion(string id, SDL_Rect* area, CLICKREGION_TYPE ct);

      void SetPosition(int x, int y);
      void SetPosition(SDL_Rect* r);

      void SetSize(int w, int h);
      void SetSize(SDL_Rect* r);

      void OnLeftClick();
      void OnRightClick();

      void OnScrollUp();
      void OnScrollDown();

      string id;
      SDL_Rect* rect;
      CLICKREGION_TYPE crType;

      bool active;
      bool displayed;
};



class ClickButton: public ClickRegion {
   public:
      ClickButton(string id, string textStr, TTF_Font* fontStyle,
         SDL_Color* fontColor, SDL_Rect* area,
         CLICKREGION_TYPE ct, string bgPath = "");

      void Render();

      string path;
      string text;
      LTexture* texture;
      TTF_Font* font;
};

#endif
