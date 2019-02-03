#include "..\include\CollisionDetection.h"

bool PointInRect(int x, int y, SDL_Rect* b) {
   if ((x >= b->x) && (x <= (b->x + b->w))) {
      if ((y >= b->y) && (y <= (b->y + b->h))) {
         return true;
      }
   }
   return false;
}



int IsPartialCollision(SDL_Rect* a, SDL_Rect* b) {
   SDL_Rect intersect;
   if (SDL_IntersectRect(a, b, &intersect) == SDL_FALSE) {
      return 0;
   } else {
      return (intersect.w * intersect.h);
   }
}



bool IsWholeCollision(SDL_Rect* a, SDL_Rect* b) {
   if (PointInRect(a->x, a->y, b) &&
   PointInRect(a->x + a->w, a->y, b) &&
   PointInRect(a->x, a->y + a->h, b) &&
   PointInRect(a->x + a->w, a->y + a->h, b)) {
      return true;
   }
   return false;
}



bool IsRectCollision(SDL_Rect* a, SDL_Rect* b) {
   int ax = a->x;
   int ay = a->y;
   int aw = a->w;
   int ah = a->h;

   bool flag = false;
   if (PointInRect(ax, ay, b)) { flag = true; }
   if (PointInRect(ax + aw, ay, b)) { flag = true; }
   if (PointInRect(ax, ay + ah, b)) { flag = true; }
   if (PointInRect(ax + aw, ay + ah, b)) { flag = true; }

   return flag;
}
