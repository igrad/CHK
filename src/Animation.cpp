#include "..\include\LTexture.h"
#include "..\include\Animation.h"

// Animation
// Animations build off of LTextures and allow for multiple frames to be applied
// to the same entity. Animations are typically loaded from PNG or JPEG files
// color-keyed with cyan
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

// Return the width of the frame
int Animation::GetFrameWidth() {
   return frameW;
}

// Return the height of the frame
int Animation::GetFrameHeight() {
   return frameH;
}

// Load an animation from a file
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

   int xIter = 0;
   int yIter = 0;

   for (int i = 0; i < frameCount; i++) {
      if (xIter * frameW >= mWidth) {
         yIter++;
         xIter = 0;
      }
      // HEY DUMBASS: Was doing else xIter++ here, which started off
      // new frame loads at index 1 lol
      // Double-check that other animation loads aren't doing this same thing

      frameClips[i].x = xIter * frameW;
      frameClips[i].y = yIter * frameH;
      frameClips[i].w = frameW;
      frameClips[i].h = frameH;

      xIter++;
   }

   // If we're reversing the frames of this animation, then we can just reverse
   // the array in-place
   if (reversed) {
      SDL_Rect cpy;

      int i = 0;
      int j = frameCount - 1;
      while (j > i) {
         cpy = frameClips[i];
         frameClips[i] = frameClips[j];
         frameClips[j] = cpy;

         i++;
         j--;
      }
   }

   return true;
}

// Load an animation from a reference to an LTexture
bool Animation::LoadFromReference(LTexture* ref, int frames, float duration,
   int frameW, int frameH, bool reversed) {
   // Set the frame count
   frameCount = frames;

   // Set the animation duration
   animDuration = duration;

   // Get rid of preexisting texture
   LTexture::LoadFromReference(ref);

   if (mTexture == NULL) {
      // Warn("Failed to load texture from reference!");
      return false;
   }

   frameClips = new SDL_Rect[frameCount];
   this->frameW = frameW;
   this->frameH = frameH;

   int xIter = 0;
   int yIter = 0;

   for (int i = 0; i < frameCount; i++) {
      if (xIter * frameW >= mWidth) {
         yIter++;
         xIter = 0;
      }
      // HEY DUMBASS: Was doing else xIter++ here, which started off
      // new frame loads at index 1 lol
      // Double-check that other animation loads aren't doing this same thing

      frameClips[i].x = xIter * frameW;
      frameClips[i].y = yIter * frameH;
      frameClips[i].w = frameW;
      frameClips[i].h = frameH;

      xIter++;
   }

   // If we're reversing the frames of this animation, then we can just reverse
   // the array in-place
   if (reversed) {
      SDL_Rect cpy;

      int i = 0;
      int j = frameCount - 1;
      while (j > i) {
         cpy = frameClips[i];
         frameClips[i] = frameClips[j];
         frameClips[j] = cpy;

         i++;
         j--;
      }
   }

   // Log("Set frameClips");

   return true;
}

// Iterate to the next frame, and account for looping
void Animation::IncrementCurrentFrame() {
   //printf("\nElement: %08p frame: %i", this, currentFrame);
   if (currentFrame + 1 >= frameCount) {
      if (looping) currentFrame = 0;
      else animDone = true;
   } else {
      currentFrame++;
   }
}

// An offset of the animation (basically SetCurrentFrame)
void Animation::SetAnimFrameOffset(int screenFrame) {
   animFrameOffset = ((float)frameCount/animDuration) *
   ((float)screenFrame/(float)SCREEN_FPS);
}

// Render animation to screen
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

// Destructor
Animation::~Animation() {
   delete[] frameClips;
   Free();
}
