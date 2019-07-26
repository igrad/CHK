#ifndef MOUSEEVENTS
#define MOUSEEVENTS

#include "essentials.h"
#include "Camera.h"
#include "LTexture.h"

enum CLICKREGION_TYPE {
   CR_ABSOLUTE,
   CR_RELATIVE
};

enum MOUSEEVENT {
   HOVER,
   LEFTCLICK,
   RIGHTCLICK,
   SCROLLUP,
   SCROLLDOWN
};

struct MouseEventData {
   std::function<void(int, int, int)> proc;
   int par1;
   int par2;
   int par3;
};

class ClickRegion {
public:
   ClickRegion();
   ClickRegion(SDL_Rect* area, CLICKREGION_TYPE ct);

   template <typename Proc>
   void SetFunction(MOUSEEVENT m, Proc p, int a1 = -1, int a2 =-1, int a3 = -1){
      callbacks[m] = {p, a1, a2, a3};
      callbackSet[m] = true;
   }

   void SetPosition(int x, int y);
   void SetPosition(SDL_Rect* r);

   void SetSize(int w, int h);
   void SetSize(SDL_Rect* r);

   void OnHover();

   void OnLeftClick();
   void OnRightClick();

   void OnScrollUp();
   void OnScrollDown();

   MouseEventData callbacks[5];
   bool callbackSet[5];

   SDL_Rect clickRect;
   bool clickActive;
   CLICKREGION_TYPE crType;
};



class ClickButton: public ClickRegion {
public:
   ClickButton();
   ClickButton(string textStr, TTF_Font* fontStyle,
      SDL_Color* fontColor, SDL_Rect* area, CLICKREGION_TYPE ct,
      string bgPath);

   void Render();

   string path;
   string text;
   LTexture* texture;
   TTF_Font* font;
};



class DropMenu {
public:
   DropMenu(int x, int y, int cols, int rows, int btnW, int btnH, int margin,
      int separation, TTF_Font* fontStyle, SDL_Color* fontColor,
      CLICKREGION_TYPE ct, string bgPath, string btnBgPath);

   void AddButton(string textString);

   void Open(int x, int y);
   void Close();

   void Render();

   CLICKREGION_TYPE ct;

   SDL_Rect area;
   SDL_Rect btnRect;
   int margin;
   int separation;
   TTF_Font* fontStyle;
   SDL_Color* fontColor;
   string bgPath;
   string btnBgPath;

   vector<ClickButton> buttons;
   int optCount;
};

#endif
