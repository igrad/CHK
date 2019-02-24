// Program: Catacombs of Halfwind Keep

// File: main.cpp
// Author: Ian Gradert
// Description: Main file for running "Catacombs of Halfwind Keep" game
// Huge credit to Lazy Foo Productions (http://lazyfoo.net/tutorials/SDL/index.php) for the stellar guide they've provided on SDL 2.0. Without it, this project would be DOA.

#include "..\include\essentials.h"

#include "..\include\CollisionDetection.h"
#include "..\include\LTimer.h"
#include "..\include\Collider.h"
#include "..\include\LTexture.h"
#include "..\include\Camera.h"
#include "..\include\Animation.h"
#include "..\include\Actor.h"
#include "..\include\Character.h"
#include "..\include\Level.h"

// Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;
SDL_Rect SCREEN_RECT = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

// Global zoom
const float GZOOM = 2.0;
const float GMAXZOOM = 2.0;

// Unscaled level size constants
const int LEVEL_HEIGHT = 100;
const int LEVEL_WIDTH = 100;

// Screen FPS rate
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000/SCREEN_FPS;


const float PIXELSPERFEET = 8.0; // This is preliminary and only used for testing. Once we have a proper camera class up and running, the zoom attribute of the camera will determine this value



// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

// The currently displayed texture
LTexture gTexture;

Character player(1, 10, 15, 16, 0);


// Declare all locales
Locale dungeon;



// Starts up SDL and creates window
bool Init() {
	Log("Initializing...");
	//Initialization flag
	bool success = true;

	// Set surface scaling to not use any anti-aliasing
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Fatal("SDL could not initialize! SDL_Error: %s\n" + string(SDL_GetError()));
		success = false;
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow("Catacombs of Halfwind Keep", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("\nWindow could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		} else {
         // Create renderer for window
         gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
         if (gRenderer == NULL) {
            Fatal("Renderer could not be created! SDL_Error: %s\n" + string(SDL_GetError()));
         } else {
            // Initialize renderer color
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            // Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
               Fatal("SDL_image could not initialize! SDL_image Error: " + string(IMG_GetError()));
               success = false;
            }
         }
		}
	}

	return success;
}



// Loads media
bool LoadMedia() {
	//Loading success flag
	bool success = true;

   if (!gTexture.LoadFromFile("media\\images\\hello_world.png")) {
      Warn("Failed to load texture image!\n");
      success = false;
   }

	player.SetDrawBoxSize(0, 0, 20, 40);
	player.SetHitBoxSize(5, 35, 10, 5);
	player.LoadAnimation(ANIM_IDLE_I, "media\\images\\Idle_I.png", 8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_IDLE_II, "media\\images\\Idle_II.png", 8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_IDLE_III, "media\\images\\Idle_III.png", 8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_IDLE_IV, "media\\images\\Idle_IV.png", 8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_WALK_I, "media\\images\\Walking_I.png", 8, 1.2, 20, 40);
	player.LoadAnimation(ANIM_WALK_II, "media\\images\\Walking_II.png", 8, 1.2, 20, 40);
	player.LoadAnimation(ANIM_WALK_III, "media\\images\\Walking_III.png", 8, 1.2, 20, 40);
	player.LoadAnimation(ANIM_WALK_IV, "media\\images\\Walking_IV.png", 8, 1.2, 20, 40);
	player.SetActiveAnim(ANIM_IDLE_IV);

	return success;
}



// Frees media and shuts down SDL
void Close() {
   // Free loaded images
   gTexture.Free();
	//myDummy.Free();
	player.Free();

	// Destroy window
   SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
   gWindow = NULL;
   gRenderer = NULL;

	// Quit SDL subsystems
   IMG_Quit();
	SDL_Quit();
}



int main(int argc, char* args[]) {
   // Start up SDL and create window
	Log("Initializing");
	if (!Init()) {
		Fatal("Failed to initialize!");
	}
	else {
		// Load media
		Log("Loading media");
		if (!LoadMedia()) {
			Fatal("Failed to load media!");
		}
		else {
			// Initialize all locales
			dungeon.mapSize = 100;
			dungeon.minRoomDim = 5;
			dungeon.maxRoomDim = 18;
			dungeon.minRoomCount = 8;
			dungeon.maxRoomCount = 18;
			dungeon.cornerType = CORNER_SHARP;
			dungeon.cornerSize = 1;
			dungeon.corridorType = CORRIDOR_CORNERS;
			dungeon.corridorSize = 1;
			dungeon.floorTexture.LoadFromFile("media\\images\\dungeon\\floor1.png");
			dungeon.floorTextureSize = 320;
			dungeon.wallTexture.LoadFromFile("media\\images\\dungeon\\wall1.png");
			dungeon.wallHeight = 43;
			dungeon.voidTexture.LoadFromFile("media\\images\\dungeon\\void.png");
			dungeon.wallTexture_LEdge.LoadFromFile("media\\images\\dungeon\\wall_edge_l.png");
			dungeon.wallTexture_REdge.LoadFromFile("media\\images\\dungeon\\wall_edge_r.png");
			dungeon.wallTexture_TEdge.LoadFromFile("media\\images\\dungeon\\wall_edge_t.png");
			dungeon.voidTextureSize = 640;

			// Generate level
			Level randomLevel = Level(&dungeon);
			randomLevel.GenerateLevel();
			randomLevel.WriteOutWholeLevel();

			player.SetSpawnPoint(randomLevel.GetPlayerSpawn());

         // Main loop flag
         bool quit = false;

         // Event handler
         SDL_Event e;

			// The frames per second timer
			LTimer fpsTimer;

			// The frames per second cap timer
			LTimer capTimer;

			// Start counting frames per second
			int countedFrames = 0;
			fpsTimer.start();

			Log("Starting main event loop");

         // While the application is running
         while(!quit) {
				// Start the cap timer
				capTimer.start();
            // Handle events in the queue
            while(SDL_PollEvent(&e) != 0) {
               // User requests quit
               if (e.type == SDL_QUIT) {
                  quit = true;
               }
            }

				// Calculate and lock the FPS
				float avgFPS = countedFrames/(fpsTimer.getTicks()/1000.f);
				if (avgFPS > 2000000) {
					avgFPS = 0;
				}

				// Handle keyboard input
				const Uint8* keyStates = SDL_GetKeyboardState(NULL);

				// Player movement
				bool W = keyStates[SDL_SCANCODE_W];
				bool S = keyStates[SDL_SCANCODE_S];
				bool A = keyStates[SDL_SCANCODE_A];
				bool D = keyStates[SDL_SCANCODE_D];

				if (W && S) {
					player.SetYVelocity(0);
				} else if (W) {
					player.yDirection = false;
					player.SetYVelocity(-1);
				} else if (S) {
					player.yDirection = true;
					player.SetYVelocity(1);
				}

				if (A && D) {
					player.SetXVelocity(0);
				} else if (A) {
					player.xDirection = false;
					player.SetXVelocity(-1);
				} else if (D) {
					player.xDirection = true;
					player.SetXVelocity(1);
				}

				// Handle character movements
				player.HandleMovement(Camera::x, Camera::y);

				// Handle collisions
				// First, players
				if (player.queueCollisions) {
					// Check collisions with other characters, and then walls
					CheckCollisions(&player, &randomLevel.walls);
				}

				// Next, other characters
				// Next, projectiles
				// Finally, items

            // Clear screen
            SDL_RenderClear(gRenderer);

				int screenFrame = countedFrames%SCREEN_FPS;
            // Render texture to screen
				gTexture.Render(&SCREEN_RECT);

				if (Camera::focusMode == FOCUS_PLAYER) {
					Camera::SetFocusOnActor(&player);
				}

				randomLevel.Render(Camera::x, Camera::y);

				player.Render(screenFrame, Camera::x, Camera::y);

            // Update screen
            SDL_RenderPresent(gRenderer);

				++countedFrames;

				// If the frame finished early
				int frameTicks = capTimer.getTicks();
				if (frameTicks < SCREEN_TICKS_PER_FRAME) {
					// Wait for the remaining time
					SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
				}
         }
		}
	}

	Close();
   return 0;
}
