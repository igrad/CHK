// Program: Catacombs of Halfwind Keep

// File: main.cpp
// Author: Ian Gradert
// Description: Main file for running "Catacombs of Halfwind Keep" game
// Huge credit to Lazy Foo Productions (http://lazyfoo.net/tutorials/SDL/index.php) for the stellar guide they've provided on SDL 2.0. Without it, this project would be DOA.

#include "..\include\essentials.h"
#include "..\include\CollisionDetection.h"

#include "..\include\LTimer.h"
#include "..\include\Camera.h"
#include "..\include\MouseEvents.h"

#include "..\include\Collider.h"
#include "..\include\LTexture.h"
#include "..\include\Animation.h"
#include "..\include\Actor.h"
#include "..\include\Character.h"

#include "..\include\Locale.h"
#include "..\include\Level.h"

#include "..\include\Door.h"

#include "..\include\UITheme.h"

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



// UI
UITheme UI;

// Fonts
TTF_Font* FONT_SCROLL = NULL;
TTF_Font* FONT_HAND = NULL;
TTF_Font* FONT_TYPED = NULL;
TTF_Font* FONT_TYPED_BOLD = NULL;
TTF_Font* FONT_TYPED_ITALIC = NULL;



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

				// Initialize TTF module
				if (TTF_Init() == -1) {
					Fatal("SDL_TTF failed to initialize!");
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

	// Initialize fonts
	FONT_SCROLL = TTF_OpenFont("fonts/BLKCHCRY.TTF", 24);
	FONT_HAND = TTF_OpenFont("fonts/Quikhand.ttf", 24);
	FONT_TYPED = TTF_OpenFont("fonts/BaskervaldADFStd.otf", 48);
	FONT_TYPED_BOLD = TTF_OpenFont("fonts/BaskervaldADFStd-Bold.otf", 24);
	FONT_TYPED_ITALIC = TTF_OpenFont("fonts/BaskervaldADFStd-Italic.otf", 24);

	// TODO: Initialize locale (this should eventually be moved to level loading)
	dungeon.Initialize("dungeon");

	player.SetDrawBoxSize(0, 0, 20, 40);
	player.SetHitBoxSize(5, 35, 10, 5);
	player.LoadAnimation(ANIM_IDLE_I, "media\\images\\Idle_I.png",
		8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_IDLE_II, "media\\images\\Idle_II.png",
		8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_IDLE_III, "media\\images\\Idle_III.png",
		8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_IDLE_IV, "media\\images\\Idle_IV.png",
		8, 2.0, 20, 40);
	player.LoadAnimation(ANIM_WALK_I, "media\\images\\Walking_I.png",
		8, 1.2, 20, 40);
	player.LoadAnimation(ANIM_WALK_II, "media\\images\\Walking_II.png",
		8, 1.2, 20, 40);
	player.LoadAnimation(ANIM_WALK_III, "media\\images\\Walking_III.png",
		8, 1.2, 20, 40);
	player.LoadAnimation(ANIM_WALK_IV, "media\\images\\Walking_IV.png",
		8, 1.2, 20, 40);
	// player.LoadAnimation(ANIM_IDLE_I, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_IDLE_I, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_IDLE_II, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_IDLE_III, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_IDLE_IV, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_WALK_I, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_WALK_II, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_WALK_III, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);
	// player.LoadAnimation(ANIM_WALK_IV, &dungeon.doorNS_left_inside_open_anim,
	// 	8, 2.0, 40, 80);

	// TODO: Initialize locale (this should eventually be moved to level loading)
	//dungeon.Initialize("dungeon");

	Log("Loading media for UI");
	// Load UI themes
	UI.doorDM.bg->LoadFromFile("media\\images\\dmbg.png");
	UI.doorDM.btnbg->LoadFromFile("media\\images\\dmbtnbg.png");
	UI.doorDM.fontSize = 16;
	UI.doorDM.fontStyle = FONT_TYPED;
	UI.doorDM.fontColor = &FONTC_OFFWHITE;
	UI.doorDM.margin = 4;
	UI.doorDM.separation = 4;
	UI.doorDM.btnW = 100;
	UI.doorDM.btnH = 30;

	Log("Media loaded");

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

			// Test button
			// Create a templated vector in main that stores all of the clickable
		   // objects loaded up, and each tick, iterate through that list to find
			// which ones are currently on-screen, and then check if they are
			// being hovered over or are being clicked on.
			vector<ClickRegion*>* clickables = &ClickRegion::clickables;
			vector<DropMenu*>* pendingDMs = &DropMenu::pendingDMs;

			// Compile objects into clickables vector that are sensitive to
			// mouse events
			for (auto d : randomLevel.doors) clickables->push_back(d);

			// Start counting frames per second
			int countedFrames = 0;
			fpsTimer.start();

			vector<ClickRegion*> hoveredCRs;
			vector<DropMenu*> hoveredDMs;
			bool newDMWaiting = false;

			Log("Starting main event loop");

			int mx, my, gmx, gmy;

         // While the application is running
         while(!quit) {
				// Start the cap timer
				capTimer.start();
            // Handle events in the queue
            while(SDL_PollEvent(&e) != 0) {
               // User requests quit
               if (e.type == SDL_QUIT) {
                  quit = true;
               } else if (e.type == SDL_MOUSEMOTION || // Handle mouse input
						e.type == SDL_MOUSEBUTTONDOWN ||
						e.type == SDL_MOUSEBUTTONUP ||
						e.type == SDL_MOUSEWHEEL) {
						SDL_PumpEvents();
						SDL_GetMouseState(&mx, &my);
						gmx = mx + Camera::x;
						gmy = my + Camera::y;

						newDMWaiting = false;

						hoveredCRs = ClickRegion::GetRegionsAtMouse(gmx, gmy);
						hoveredDMs = DropMenu::GetDMsAtMouse(gmx, gmy);

						// TODO: Working on updating this chunk with the new static members of ClickRegion and DropMenu
						bool eventIssued = false;
						switch (e.type) {
							case SDL_MOUSEBUTTONDOWN:
							if (e.button.button == SDL_BUTTON_LEFT) {
								eventIssued = true;

								if (hoveredCRs.size() > 0) {
									newDMWaiting =
									hoveredCRs.back()->OnLeftClick(mx, my);
								}
							} else if (e.button.button == SDL_BUTTON_RIGHT) {
								eventIssued = true;

								if (hoveredCRs.size() > 0) {
									newDMWaiting =
									hoveredCRs.back()->OnRightClick(mx, my);
								}
							}

							break;

							case SDL_MOUSEWHEEL:
							if (e.wheel.y > 0) {
								eventIssued = true;

								if (hoveredCRs.size() > 0) {
									newDMWaiting =
									hoveredCRs.back()->OnScrollUp(mx, my);
								}
							} else if (e.wheel.y < 0) {
								eventIssued = true;

								if (hoveredCRs.size() > 0) {
									newDMWaiting =
									hoveredCRs.back()->OnScrollDown(mx, my);
								}
							}
							break;
						}

						// Check to see if user has clicked out of the drop menu
						if (e.type == SDL_MOUSEBUTTONDOWN) {
							if (DropMenu::focusedDM != NULL) {
								if (find(hoveredDMs.begin(), hoveredDMs.end(),
									DropMenu::focusedDM) == hoveredDMs.end()) {
									if (!newDMWaiting) DropMenu::focusedDM->Close();
								} else {
									// If they click on a button in the focused DM
									// and it doesn't open a new DM, we can close the
									// focused DM after the action is handled above
									if ((hoveredCRs.size() > 0) && !newDMWaiting) {
										DropMenu::focusedDM->Close();
									}
								}
							}
						}

						// If we have a new DM opened on this frame, we add it to the
						// pendingDMs vector and automatically set it as the new
						// focused DM
						if (newDMWaiting) {
							DropMenu::AddPendingDM(hoveredCRs.back()->GetDM());
						}

						// If the cursor isn't over the focused DM, and the DM has
						// already been moused-over, then it means the user isn't
						// interested
						if (DropMenu::focusedDM != NULL) {
							if (DropMenu::focusedDM->hovered &&
								find(pendingDMs->begin(), pendingDMs->end(),
								DropMenu::focusedDM) == pendingDMs->end()) {
								DropMenu::focusedDM->Close();
								DropMenu::RemovePendingDM(DropMenu::focusedDM);
							}
						}
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
					CheckWallCollisions(&player, &randomLevel.walls);
					for (int i = 0; i < (&randomLevel.doors)->size(); i++) {
						CheckDoorCollisions(&player, &randomLevel.doors[i]->part1);
						CheckDoorCollisions(&player, &randomLevel.doors[i]->door);
						CheckDoorCollisions(&player, &randomLevel.doors[i]->part2);
					}
				}

				// Next, other characters
				// Finally, items
				// Next, projectiles

            // Clear screen
            SDL_RenderClear(gRenderer);

				int screenFrame = countedFrames%SCREEN_FPS;
            // Render texture to screen
				gTexture.Render(&SCREEN_RECT);

				if (Camera::focusMode == FOCUS_PLAYER) {
					Camera::SetFocusOnActor(&player);
				}

				// Iterate through the list of actors here, but until we have more
				// than one actor, we can do this without iterations
				// We need a vector of actors, sorted by their Y position. If two
				// of them tie, it doesn't matter
				// We iterate through this vector, and get the Y tile of the
				// current actor so that we can chop up the walls

				//vector sortedActors

				int currentY = 0;

				randomLevel.RenderFloor();

				randomLevel.RenderWalls(currentY,
					player.hitBox.y + player.hitBox.h);
				randomLevel.RenderDoors(currentY,
					player.hitBox.y + player.hitBox.h);

				currentY = player.hitBox.y + player.hitBox.h;

				player.Render(screenFrame, Camera::x, Camera::y);

				randomLevel.RenderWalls(currentY, -1);
				randomLevel.RenderDoors(currentY, -1);

				// Render UI
				for (auto d : randomLevel.doors) {
					d->dropmenu->Render();
				}

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
