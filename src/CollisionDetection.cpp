#include "..\include\CollisionDetection.h"

bool PointInRect(int x, int y, SDL_Rect* b) {
   // if ((x >= b->x) && (x < (b->x + b->w))) {
   //    if ((y >= b->y) && (y < (b->y + b->h))) {
   //       return true;
   //    }
   // }
   // return false;

   bool flag = true;
   if (x < b->x) flag = false;
   if (x > b->x + b->w) flag = false;
   if (y < b->y) flag = false;
   if (y > b->y + b->h) flag = false;
   return flag;
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
   int bx = b->x;
   int by = b->y;
   int bw = b->w;
   int bh = b->h;

   bool flag = false;
   if (PointInRect(ax, ay, b)) { flag = true; }
   else if (PointInRect(ax + aw, ay, b)) { flag = true; }
   else if (PointInRect(ax, ay + ah, b)) { flag = true; }
   else if (PointInRect(ax + aw, ay + ah, b)) { flag = true; }
   else if (PointInRect(bx, by, a)) { flag = true; }
   else if (PointInRect(bx + bw, by, a)) { flag = true; }
   else if (PointInRect(bx, by + bh, a)) { flag = true; }
   else if (PointInRect(bx + bw, by + bh, a)) { flag = true; }

   return flag;
}



void CheckWallCollisions(Actor* actor, map<int, Collider>* others) {
   for (auto i = others->end(); i != others->begin(); i--) {
      if (IsRectCollision(&actor->hitBox, &i->second.hitBox)) {
         SDL_Rect* a = &actor->hitBox;
         SDL_Rect* b = &i->second.hitBox;
         int xPosOverlap = (a->x + a->w) - b->x;
         int xNegOverlap = a->x - (b->x + b->w);
         int yPosOverlap = (a->y + a->h) - b->y;
         int yNegOverlap = a->y - (b->y + b->h);

         int minOverlap = min(min(min(abs(xPosOverlap), abs(xNegOverlap)), abs(yPosOverlap)), abs(yNegOverlap));

         if (actor->yVelocity < 0 && actor->xVelocity > 0) {
            if (abs(yPosOverlap) == minOverlap) {
               actor->SetYPos(actor->yPos - yPosOverlap);
            } else if (abs(yNegOverlap) == minOverlap) {
               actor->SetYPos(actor->yPos - yNegOverlap);
            } else if (abs(xPosOverlap) == minOverlap) {
               actor->SetXPos(actor->xPos - xPosOverlap);
            } else if (abs(xNegOverlap) == minOverlap) {
               actor->SetXPos(actor->xPos - xNegOverlap);
            }
         } else {
            if (abs(xPosOverlap) == minOverlap) {
               actor->SetXPos(actor->xPos - xPosOverlap);
            } else if (abs(xNegOverlap) == minOverlap) {
               actor->SetXPos(actor->xPos - xNegOverlap);
            } else if (abs(yPosOverlap) == minOverlap) {
               actor->SetYPos(actor->yPos - yPosOverlap);
            } else if (abs(yNegOverlap) == minOverlap) {
               actor->SetYPos(actor->yPos - yNegOverlap);
            }
         }
      }
   }
}



void CheckDoorCollisions(Actor* actor, Collider* other) {
   if (IsRectCollision(&actor->hitBox, &other->hitBox)) {
      SDL_Rect* a = &actor->hitBox;
      SDL_Rect* b = &other->hitBox;
      int xPosOverlap = (a->x + a->w) - b->x;
      int xNegOverlap = a->x - (b->x + b->w);
      int yPosOverlap = (a->y + a->h) - b->y;
      int yNegOverlap = a->y - (b->y + b->h);

      int minOverlap = min(min(min(abs(xPosOverlap), abs(xNegOverlap)),
         abs(yPosOverlap)), abs(yNegOverlap));

      if (actor->yVelocity < 0 && actor->xVelocity > 0) {
         if (abs(yPosOverlap) == minOverlap) {
            actor->SetYPos(actor->yPos - yPosOverlap);
         } else if (abs(yNegOverlap) == minOverlap) {
            actor->SetYPos(actor->yPos - yNegOverlap);
         } else if (abs(xPosOverlap) == minOverlap) {
            actor->SetXPos(actor->xPos - xPosOverlap);
         } else if (abs(xNegOverlap) == minOverlap) {
            actor->SetXPos(actor->xPos - xNegOverlap);
         }
      } else {
         if (abs(xPosOverlap) == minOverlap) {
            actor->SetXPos(actor->xPos - xPosOverlap);
         } else if (abs(xNegOverlap) == minOverlap) {
            actor->SetXPos(actor->xPos - xNegOverlap);
         } else if (abs(yPosOverlap) == minOverlap) {
            actor->SetYPos(actor->yPos - yPosOverlap);
         } else if (abs(yNegOverlap) == minOverlap) {
            actor->SetYPos(actor->yPos - yNegOverlap);
         }
      }
   }
}
