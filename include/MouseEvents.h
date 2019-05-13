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

      void OnClick();

      string id;
      SDL_Rect* rect;
      CLICKREGION_TYPE crType;

      bool active;
      bool displayed;
};



class ClickButton: public ClickRegion {
   public:
      ClickButton(string id, string bgPath, string textStr, TTF_Font* fontStyle,
         SDL_Color* fontColor, SDL_Rect* area, CLICKREGION_TYPE ct);

      void Render();

      string path;
      string text;
      LTexture* texture;
      TTF_Font* font;
};



class HintButton: public ClickRegion {
   public:
      HintButton(string id, string text, TTF_Font* font, SDL_Color* color,
         SDL_Rect* area, CLICKREGION_TYPE crType);

      void Render();

      string text;
      LTexture* texture;
      TTF_Font* font;
};

#endif
