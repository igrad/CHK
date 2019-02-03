#ifndef ANIMATION_LOADED
#define ANIMATION_LOADED

#include "essentials.h"
#include "LTexture.h"


/**
* Class: Animation
* Inherits: LTexture
* Authors: Ian Gradert
* Description: Creates an animated version of an LTexture object by use of
* spritesheets. Also allows for color-keying.
*/
class Animation: public LTexture {
   public:
      Animation();

      // Getters
      int GetFrameWidth();
      int GetFrameHeight();

      // Loads image at specified path
      bool LoadFromFile(string path, int frames, float duration, int frameW = 0, int frameH = 0);

      // Increment the current frame count
      void IncrementCurrentFrame();

      // Render the current frame to screen
      void Render(SDL_Rect* drawBox, int screenFrame);

      ~Animation();

      // Current phase of the animation
      int currentFrame;
      int animFrameOfLastScreenFrame;

   private:
      // The actual hardware texture
      SDL_Texture* mTexture;

      // Image dimensions
      int mWidth;
      int mHeight;

      // Number of frames stored in the sprite
      int frameCount;

      // Duration of the animation
      float animDuration;

      // Frame dimensions
      int frameW;
      int frameH;

      // Sections of the spritesheet corresponding to each animation frame
      SDL_Rect *frameClips;
};

#endif
