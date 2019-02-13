#include "..\include\LTexture.h"
#include <windows.h> // for EXCEPTION_ACCESS_VIOLATION
#include <iostream>

LTexture::LTexture() {
   //Initialize
   mTexture = NULL;
   mWidth = 0;
   mHeight = 0;
}



void LTexture::Free() {
   // Free texture if it exists
   if (mTexture != NULL) {
      SDL_DestroyTexture(mTexture);
      mTexture = NULL;
      mWidth = 0;
      mHeight = 0;
   }
}



void LTexture::Render(SDL_Rect* drawBox, SDL_Rect* clip) {
   SDL_RenderCopy(gRenderer, mTexture, clip, drawBox);
}



int LTexture::GetWidth() {
   return mWidth;
}



int LTexture::GetHeight() {
   return mHeight;
}



bool LTexture::LoadFromFile(string path) {
   // Get rid of preexisting texture
   Free();

   // The final texture
   SDL_Texture* newTexture = NULL;

   // Load the image at specified path
   SDL_Surface* loadedSurface = IMG_Load(path.c_str());
   if (loadedSurface == NULL) {
      printf("Unable to load image %s! SDL_image error: %s\n", path.c_str(), IMG_GetError());
   } else {
      // Color key the image
      SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

      // Create texture from surface picels
      newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
      if (newTexture == NULL) {
         printf("Unable to create texture from %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
      } else {
         // Get image dimensions
         mWidth = loadedSurface->w;
         mHeight = loadedSurface->h;
      }

      // Get rid of the old loaded surface
      SDL_FreeSurface(loadedSurface);
   }

   // Return success
   mTexture = newTexture;
   return mTexture != NULL;
}



LTexture::~LTexture() {
   //Deallocate
   Free();
}
