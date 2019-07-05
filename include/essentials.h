#ifndef ESSENTIALS_LOADED
#define ESSENTIALS_LOADED

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

//Key press surfaces constants
enum KeyPressSurfaces {
   KEY_PRESS_SURFACE_DEFAULT,
   KEY_PRESS_SURFACE_W,
   KEY_PRESS_SURFACE_S,
   KEY_PRESS_SURFACE_A,
   KEY_PRESS_SURFACE_D,
   KEY_PRESS_SURFACE_TOTAL
};

// Directions
enum DIRECTION { NORTH, EAST, SOUTH, WEST };

// Level constants
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SCREEN_FPS;

extern const float GZOOM;
extern const float GMAXZOOM;

extern const int LEVEL_WIDTH;
extern const int LEVEL_HEIGHT;

extern const float PIXELSPERFEET;

// Fonts
extern TTF_Font* FONT_SCROLL;
extern TTF_Font* FONT_HAND;
extern TTF_Font* FONT_TYPED;
extern TTF_Font* FONT_TYPED_BOLD;
extern TTF_Font* FONT_TYPED_ITALIC;



// The window we'll be rendering to
extern SDL_Window* gWindow;

// The window renderer
extern SDL_Renderer* gRenderer;


// Function declarations
bool Init();
bool LoadMedia();
void Close();

void Log(string message);
void Warn(string message);
void Fatal(string message);

float FindDistance(int oX, int oY, int dX, int dY);

int max(int x, int y);
int min(int x, int y);

#endif
