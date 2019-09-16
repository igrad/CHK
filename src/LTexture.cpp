#include "..\include\LTexture.h"
#include <windows.h> // for EXCEPTION_ACCESS_VIOLATION
#include <iostream>

LTexture::LTexture() {
   //Initialize
   mTexture = NULL;
   mWidth = 0;
   mHeight = 0;
   loaded = false;
}

LTexture::LTexture(string path) {
   LTexture();
   LoadFromFile(path);
}

bool LTexture::LoadFromFile(string path) {
   // Get rid of preexisting texture
   if (loaded) Free();

   // The final texture
   SDL_Texture* newTexture = NULL;

   // Load the image at specified path
   SDL_Surface* loadedSurface = IMG_Load(path.c_str());
   if (loadedSurface == NULL) {
      printf("\nUnable to load image! SDL_image error: %s",
      path.c_str(), IMG_GetError());
   } else {
      // Color key the image
      SDL_SetColorKey(loadedSurface, SDL_TRUE,
         SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

      // Create texture from surface pixels
      newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
      if (newTexture == NULL) {
         printf("\nUnable to create texture from %s! SDL_Error: %s",
         path.c_str(), SDL_GetError());
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
   loaded = true;
   return mTexture != NULL;
}

bool LTexture::LoadFromReference(LTexture* ref) {
   Log("Assigning mTexture");
   mTexture = ref->mTexture;
   mWidth = ref->mWidth;
   mHeight = ref->mHeight;

   Log("Assigned");
   loaded = true;

   Log("returning");
   // Return success
   return mTexture != NULL;
}

bool LTexture::LoadFromRenderedText(string textureText, SDL_Color* textColor,
   TTF_Font* font, int* rw, int* rh) {
   if (loaded) Free();

   SDL_Surface* textSurface = TTF_RenderText_Blended(font, textureText.c_str(),
   *textColor);
   if (textSurface == NULL) {
      Warn("Failed to load texture from rendered text");
   } else {
      // Create texture from surface pixels
      mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
      SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_ADD);
      if (mTexture == NULL) {
         Warn("Unable to create texture from textSurface!");
      } else {
         // Get image dimensions
         mWidth = textSurface->w;
         mHeight = textSurface->h;

         TTF_SizeText(font, textureText.c_str(), rw, rh);
      }

      // Delete old surface
      SDL_FreeSurface(textSurface);
   }

   loaded = true;

   return mTexture != NULL;
}

bool LTexture::CreateBlank(int width, int height, SDL_TextureAccess access) {
   if (loaded) Free();
   mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access,
      width, height);

   SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

   if (mTexture == NULL) {
      Fatal("Unable to create blank texture!");
      printf("\nSDL ERROR: %s", SDL_GetError());
   }
   mWidth = width;
   mHeight = height;

   loaded = true;

   return mTexture != NULL;
}

void LTexture::Render(SDL_Rect* drawBox, SDL_Rect* clip) {
   SDL_RenderCopy(gRenderer, mTexture, clip, drawBox);
}

void LTexture::Render(int x, int y, SDL_Rect* clip)  {
   SDL_Rect r = {x, y, mWidth, mHeight};
   SDL_RenderCopy(gRenderer, mTexture, clip, &r);
}

int LTexture::GetWidth() {
   return mWidth;
}

int LTexture::GetHeight() {
   return mHeight;
}

void LTexture::Free() {
   // Free texture if it exists
   if (loaded) {
      SDL_DestroyTexture(mTexture);
      mTexture = NULL;
      mWidth = 0;
      mHeight = 0;
      loaded = false;
   }
}

LTexture::~LTexture() {
   //Deallocate
   Free();
}
