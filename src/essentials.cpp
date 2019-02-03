#include "..\include\essentials.h"

void Log(string message) {
   cout << "\n[LOG]\t" + message;
}



void Warn(string message) {
   cout << "\n[WARN]\t" + message;
}



void Fatal(string message) {
   cout << "\n[ERROR]\t" + message;
}



float FindDistance(int oX, int oY, int dX, int dY) {
   float vectX = dX - oX;
   float vectY = dY - oY;

   return pow(pow(vectX, 2) + pow(vectY, 2), 1/2);
}
