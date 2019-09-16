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
      int xIter = 0;
      int yIter = 0;
      for (int i = 0; i < frameCount; i++) {
         if (xIter * frameW >= mWidth) {
            yIter++;
            xIter = 0;
         } else xIter++;
         frameClips[i].x = (xIter * frameW) % mWidth;
         frameClips[i].y = yIter * frameH;
         frameClips[i].w = frameW;
         frameClips[i].h = frameH;
      }
   } else {
      // TODO: Rework this so that it uses xIter and yIter instead of this
      // jumbled confusion, because this probably won't work
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
   LTexture::LoadFromReference(ref);

   Log("Checking for NUL");
   if (mTexture == NULL) {
      Warn("Failed to load texture from reference!");
      return false;
   }

   Log("Not null");

   frameClips = new SDL_Rect[frameCount];
   this->frameW = frameW;
   this->frameH = frameH;

   Log("Got dims");
   printf("\nframeW: %i, frameH: %i, mWidth: %i", frameW, frameH, mWidth);

   int xIter = 0;
   int yIter = 0;

   for (int i = 0; i < frameCount; i++) {
      if (xIter * frameW >= mWidth) {
         yIter++;
         xIter = 0;
      } else xIter++;
      frameClips[i].x = xIter * frameW;
      frameClips[i].y = yIter * frameH;
      frameClips[i].w = frameW;
      frameClips[i].h = frameH;
   }

   // If we're reversing the frames of this animation, then we can just reverse
   // the array in-place
   if (reversed) {
      SDL_Rect cpy;

      for (int i = 0; i*2 <= frameCount; i++) {
         cpy = frameClips[i];
         frameClips[i] = frameClips[frameCount - i - 1];
         frameClips[frameCount - i - 1] = cpy;
      }
   }

   Log("Set frameClips");

   return true;
}

void Animation::IncrementCurrentFrame() {
   if (currentFrame + 1 >= frameCount) {
      if (looping) currentFrame = 0;
      else animDone = true;
   } else {
      currentFrame++;
   }
}

void Animation::SetAnimFrameOffset(int screenFrame) {
   // animFrameOffset = ((float)frameCount/animDuration) *
   // ((float)screenFrame/(float)SCREEN_FPS);
}

void Animation::Render(SDL_Rect* drawBox, int screenFrame) {
   //printf("\nRendering frameclip: %i, %i, %i, %i",
   // frameClips[currentFrame].x, frameClips[currentFrame].y,
   // frameClips[currentFrame].w, frameClips[currentFrame].h);
   // SDL_RenderCopy(gRenderer, mTexture, &frameClips[currentFrame], drawBox);
   LTexture::Render(drawBox, &frameClips[currentFrame]);

   int animFrame = (((float)frameCount/animDuration) * ((float)screenFrame/(float)SCREEN_FPS)) - animFrameOffset;

   if (animFrameOfLastScreenFrame != animFrame) IncrementCurrentFrame();

   animFrameOfLastScreenFrame = animFrame;
}

Animation::~Animation() {
   Free();
}
