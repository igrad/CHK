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
      LTexture(string path);
      ~LTexture();

      // Loads image at specified path
      bool LoadFromFile(string path);

      // Load in text
      bool LoadFromRenderedText(string textureText, SDL_Color* textColor, TTF_Font* font);

      // Creates a blank image that we can use as a renderer
      bool CreateBlank(int width, int height, SDL_TextureAccess access = SDL_TEXTUREACCESS_TARGET);

      // Renders texture at given point
      void Render(SDL_Rect* drawBox, SDL_Rect* clip = NULL);
      void Render(int x, int y, SDL_Rect* clip = NULL);

      // Gets image dimensions
      int GetWidth();
      int GetHeight();

      // Deallocates texture
      void Free();

      // The actual hardware texture
      SDL_Texture* mTexture;

   private:
      // Image dimensions
      int mWidth;
      int mHeight;
};

#endif
