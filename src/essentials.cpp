#include "..\include\essentials.h"

SDL_Color FONTC_WHITE = {.r = 255, .g = 255, .b = 255};
SDL_Color FONTC_BLACK = {.r = 0, .g = 0, .b = 0};
SDL_Color FONTC_OFFWHITE = {.r = 240, .g = 240, .b = 240};
SDL_Color FONTC_OFFBLACK = {.r = 12, .g = 12, .b = 12};

void Log(string message) {
   cout << "\n[LOG]\t" << message.c_str();
}



void Warn(string message) {
   cout << "\n[WARN]\t" << message.c_str();
}



void Fatal(string message) {
   cout << "\n[ERROR]\t" << message.c_str();
}



float FindDistance(int oX, int oY, int dX, int dY) {
   float vectX = dX - oX;
   float vectY = dY - oY;

   return pow(pow(vectX, 2) + pow(vectY, 2), 0.5);
}



int max(int x, int y) {
   return (x > y) ? x : y;
}



int min(int x, int y) {
   return (x < y) ? x : y;
}
