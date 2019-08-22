#ifndef MOUSEEVENTS
#define MOUSEEVENTS

#include "essentials.h"
#include "CollisionDetection.h"
#include "Camera.h"
#include "LTexture.h"
#include "UITheme.h"

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
   std::function<bool(int, int, int)> proc;
   int par1;
   int par2;
   int par3;
};

class DropMenu;

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

   // Click handling methods
   void OnHover();
   bool OnLeftClick();
   bool OnRightClick();
   bool OnScrollUp();
   bool OnScrollDown();

   // General UI handling methods
   DropMenu* GetDM();
   static vector<ClickRegion*> GetRegionsAtMouse(int mx, int my);
   static void RemoveClickable(ClickRegion* r);
   static vector<ClickRegion*> clickables;

   MouseEventData callbacks[5];
   bool callbackSet[5];

   SDL_Rect clickRect;
   bool clickActive;
   CLICKREGION_TYPE crType;

   DropMenu* linkedDM;
};



class ClickButton: public ClickRegion {
public:
   ClickButton();
   ClickButton(string textStr, int fontSize, TTF_Font* fontStyle,
      SDL_Color* fontColor, int padding, SDL_Rect* clickRect,
      SDL_Rect* drawRect, CLICKREGION_TYPE ct, string bgPath);
   ClickButton(string textStr, int fontSize, TTF_Font* fontStyle,
      SDL_Color* fontColor, int padding, SDL_Rect* clickRect,
      SDL_Rect* drawRect, CLICKREGION_TYPE ct, LTexture* bg);

   void Render(SDL_Rect* r = NULL);

   string path;
   string text;
   int size;
   SDL_Rect drawRect;
   LTexture* texture;
   TTF_Font* font;
};



class DropMenu {
public:
   DropMenu(int x, int y, int cols, int rows, CLICKREGION_TYPE ct,
      DropMenuUI* UIScheme);
   DropMenu(int x, int y, int cols, int rows, int btnW, int btnH, int margin,
      int separation, int fontSize, TTF_Font* fontStyle, SDL_Color* fontColor,
      int btnPadding, CLICKREGION_TYPE ct, LTexture* bg, LTexture* btnbg);

   void ClearButtons();

   void Open(int x = 0, int y = 0);
   void Close();

   void Render();

   template <typename Proc>
   void AddButton(string textString, MOUSEEVENT m, Proc p,
      int a1 = -1, int a2 =-1, int a3 = -1) {
      SDL_Rect r = {
         btnRect.x,
         btnRect.y + (int)(buttons.size() * (separation + btnRect.h)),
         btnRect.w,
         btnRect.h
      };

      ClickButton b = ClickButton(textString, fontSize, fontStyle,
         fontColor, btnPadding, &r, &r, ct, btnbg);
      b.SetFunction(m, p, a1, a2, a3);

      buttons.push_back(b);
   }

   static vector<DropMenu*> GetDMsAtMouse(int mx, int my);
   static void AddPendingDM(DropMenu* dm);
   static void RemovePendingDM(DropMenu* dm);
   static void ClearPendingDMs();
   static vector<DropMenu*> pendingDMs;
   static DropMenu* focusedDM;

   CLICKREGION_TYPE ct;

   SDL_Rect area;
   SDL_Rect btnRect;
   int margin;
   int separation;
   int fontSize;
   int btnPadding;
   TTF_Font* fontStyle;
   SDL_Color* fontColor;

   LTexture* background;
   LTexture* btnbg;

   vector<ClickButton> buttons;
   int optCount;

   bool rendering;

   bool hovered;
};

#endif
