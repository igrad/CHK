#ifndef LTEXTURE_LOADED
#define LTEXTURE_LOADED

#include "essentials.h"

/**
* Class: LTexture
* Inherits: none
* Description: Custom wrapper for SDL_Texture objects that allows for easy creation
* and maintenance.
* Authors: Lazyfoo, Ian Gradert
*/
class LTexture {
   public:
      // Initializes variables
      LTexture();
      ~LTexture();

      // Loads image at specified path
      bool LoadFromFile(string path);

      // Deallocates texture
      void Free();

      // Renders texture at given point
      void Render(int x, int y, int w = 0, int h = 0, SDL_Rect* clip = NULL);

      // Gets image dimensions
      int GetWidth();
      int GetHeight();

      // The actual hardware texture
      SDL_Texture* mTexture;

   private:
      // Image dimensions
      int mWidth;
      int mHeight;
};

#endif
