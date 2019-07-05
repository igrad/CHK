#include "..\include\LTexture.h"
#include "..\include\Animation.h"



Animation::Animation() {
   //Initialize
   mTexture = NULL;
   mWidth = 0;
   mHeight = 0;

   frameW = 0;
   frameH = 0;

   frameClips = NULL;

   frameCount = 0;
   currentFrame = 0;
}



int Animation::GetFrameWidth() {
   return frameW;
}



int Animation::GetFrameHeight() {
   return frameH;
}



bool Animation::LoadFromFile(string path, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   // Set the frame count
   frameCount = frames;

   // Set the animation duration
   animDuration = duration;

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

   mTexture = newTexture;
   if (mTexture == NULL) {
      return false;
   }

   frameClips = new SDL_Rect[frameCount];
   this->frameW = frameW;
   this->frameH = frameH;

   if (!reversed) {
      int yIter = 0;

      for (int i = 0; i < frameCount; i++) {
         if (i * frameW >= mWidth) {
            yIter++;
         }
         frameClips[i].x = (i * frameW) % mWidth;
         frameClips[i].y = yIter * frameH;
         frameClips[i].w = frameW;
         frameClips[i].h = frameH;
      }
   } else {
      int yIter = ((mHeight > frameH) ? (int)floor(mHeight/frameH) : 0);
      for (int i = frameCount - 1; i >= 0; i--) {
         if (i * frameW >= mWidth) {
            yIter--;
         }
         frameClips[i].x = (i * frameW) % mWidth;
         frameClips[i].y = yIter * frameH;
         frameClips[i].w = frameW;
         frameClips[i].h = frameH;
      }
   }

   return true;
}



bool Animation::LoadFromReference(LTexture* ref, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   // Set the frame count
   frameCount = frames;

   // Set the animation duration
   animDuration = duration;

   // Get rid of preexisting texture
   Free();

   this->mTexture = ref->mTexture;
   if (mTexture == NULL) {
      return false;
   }

   frameClips = new SDL_Rect[frameCount];
   this->frameW = frameW;
   this->frameH = frameH;

   int yIter = 0;

   for (int i = 0; i < frameCount; i++) {
      if (i * frameW >= mWidth) {
         ++yIter;
      }
      frameClips[i].x = i * frameW;
      frameClips[i].y = yIter;
      frameClips[i].w = frameW;
      frameClips[i].h = mHeight;
   }

   return true;
}



void Animation::IncrementCurrentFrame() {
   if (currentFrame + 1 >= frameCount) {
      currentFrame = 0;
   } else {
      currentFrame++;
   }
}



void Animation::Render(SDL_Rect* drawBox, int screenFrame) {
   //printf("\nRendering: %i, %i, %i, %i", drawBox->x,drawBox->y,drawBox->w,drawBox->h);
   SDL_RenderCopy(gRenderer, mTexture, &frameClips[currentFrame], drawBox);

   int animFrame = ((float)frameCount/animDuration) * ((float)screenFrame/(float)SCREEN_FPS);

   if (animFrameOfLastScreenFrame != animFrame) {
      IncrementCurrentFrame();
   }

   animFrameOfLastScreenFrame = animFrame;
}



Animation::~Animation() {
   Free();
}
