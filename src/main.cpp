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
#include "..\include\RenderingString.h"
#include "..\include\Animation.h"
#include "..\include\Actor.h"
#include "..\include\Character.h"
#include "..\include\NPCBlueprint.h"
#include "..\include\NPC.h"

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
float RENDERZOOM = 1.f;

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

Character player("player", "player", 1, 10, 15, 16, 0);



// Declare all locales
Locale dungeon;



// Active NPC blueprints
vector<NPCBlueprint*> activeNPCTypes;
vector<NPC*> activeNPCs;



// Starts up SDL and creates window
bool Init() {
	//Initialization flag
	bool success = true;

	// Set surface scaling to not use any anti-aliasing
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Fatal("SDL could not initialize! SDL_Error: %s\n" + 
			string(SDL_GetError()));
		success = false;
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow("Catacombs of Halfwind Keep", 
			1940, 30, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gWindow == NULL) {
			Fatal("Window could not be created! SDL_Error: " + 
				string(SDL_GetError()));
			success = false;
		} else {
         // Create renderer for window
         gRenderer = SDL_CreateRenderer(gWindow, -1, 
				SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

         if (gRenderer == NULL) {
            Fatal("Renderer could not be created! SDL_Error: %s\n" + 
					string(SDL_GetError()));
         } else {
            // Initialize renderer color
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            // Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
               Fatal("SDL_image could not initialize! SDL_image Error: " + 
						string(IMG_GetError()));
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



// Load enemy profiles from storage
void LoadEnemies(Locale* locale) {
	// Eventually, the locale will determine which enemies are initialized and
	// loaded in. As this function is called repeatedly throughout runtime,
	// unused enemy blueprints will be unloaded to free up memory.
	// For now, we just use goblins as our enemies for all levels

	activeNPCTypes.clear();

	BP_goblinGuard.SetAnimBPDefaults(8, 2.0, 26, 40);
	BP_goblinGuard.GenerateAnimsAndTextures();
	activeNPCTypes.push_back(&BP_goblinGuard);

	BP_goblinShaman.SetAnimBPDefaults(8, 2.0, 26, 40);
	BP_goblinShaman.GenerateAnimsAndTextures();
	activeNPCTypes.push_back(&BP_goblinShaman);
}



// Instantiate and place NPCs throughout the level
void SpawnEnemies(Level* level) {
	// Iterate through each of the rooms and determine if there's a faction or
	// NPC-specific point of interest within that room, such as a goblin statue
	// or an orc grub hall

	int gx, gy, x, y;
	
	for (int room = 1; room < level->roomCount; room++) {
		printf("\nSpawning one of each type of enemy in room %i", room);
		for (int iter = 0; iter < activeNPCTypes.size(); iter++) {
			Log("Spawning new NPC");

			// For now, we spawn one of each NPC in each room
			gx = level->rooms[room].x + 1 +
				(rand() % (level->rooms[room].w - 2));
		   gy = level->rooms[room].y + 1 +
				(rand() % (level->rooms[room].h - 2));
		   x = gx * PIXELSPERFEET * 5 * GZOOM;
		   y = gy * PIXELSPERFEET * 5 * GZOOM;

			// Create the NPC from this blueprint 
			NPC* npc = new NPC(activeNPCTypes[iter]);

			// Set the NPCs position and other properties
			npc->SetPos(x, y);

			// Add to activeNPCs
			activeNPCs.push_back(npc);
		}
	}
}



// Frees media and shuts down SDL
void Close() {
	// Delete loaded NPCs
	for (int iter = activeNPCs.size() - 1; iter >= 0; iter--) {
		delete activeNPCs[iter];
	}

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

	Log("Exiting...");
}



// Main thread
int main(int argc, char* args[]) {
   // Start up SDL and create window
	Log("Initializing");
	if (!Init()) Fatal("Failed to initialize!");
	else {
		// Load media
		Log("Loading media");
		if (!LoadMedia()) Fatal("Failed to load media!");
		else {
			// Select the locale
			Locale* currentLocale = &dungeon;

			// Load in the goons
			LoadEnemies(currentLocale);

			// Generate level
			Level randomLevel = Level(currentLocale);
			randomLevel.GenerateLevel();
			randomLevel.WriteOutWholeLevel();

			// Initialize the player within the level
			player.allActorsIndex = Actor::allActors.size();
			Actor::allActors.push_back(&player);
			player.SetSpawnPoint(randomLevel.GetPlayerSpawn());
			Camera::SetFocusOnActor(&player);

			Log("Spawning enemies");
			SpawnEnemies(&randomLevel);
			Log("Spawned enemies successfully");

         // Main loop flag
         bool quit = false;

         // Event handler
         SDL_Event e;

			// The frames per second timer
			LTimer fpsTimer;

			// The frames per second cap timer
			LTimer capTimer;

			// Create a templated vector in main that stores all of the clickable
		   // objects loaded up, and each tick, iterate through that list to find
			// which ones are currently on-screen, and then check if they are
			// being hovered over or are being clicked on.
			vector<ClickRegion*>* clickables = &ClickRegion::clickables;
			vector<DropMenu*>* pendingDMs = &DropMenu::pendingDMs;

			// Compile objects into clickables vector that are sensitive to
			// mouse events
			for (auto d : randomLevel.doors) clickables->push_back(d);

			// Level reload button
			SDL_Rect reloadLevelRect = {0, 0, 120, 40};
			ClickButton reloadLevelBtn = ClickButton("Reload", 14, FONT_TYPED,
			&FONTC_OFFWHITE, 8, &reloadLevelRect, &reloadLevelRect, CR_ABSOLUTE,
			"media\\images\\dmbtnbg.png");
			reloadLevelBtn.SetFunction(LEFTCLICK,
				[&](int a, int b, int c) {
					Log("\n\nCreating new level");
					randomLevel = Level(&dungeon);
					randomLevel.GenerateLevel();
					randomLevel.WriteOutWholeLevel();
					player.SetSpawnPoint(randomLevel.GetPlayerSpawn());

					clickables = &ClickRegion::clickables;
					pendingDMs = &DropMenu::pendingDMs;

					for (auto d : randomLevel.doors) clickables->push_back(d);

					Camera::SetFocusOnActor(&player);

					Log("Created level successfully!");
					return true;
				});
			clickables->push_back(&reloadLevelBtn);

			// Zoom in button
			SDL_Rect zoomInRect = {128, 0, 120, 40};
			SDL_Rect zoomOutRect = {256, 0, 120, 40};
			SDL_Rect zoomResetRect = {384, 0, 120, 40};
			ClickButton zoomInBtn = ClickButton("Zoom In", 14, FONT_TYPED,
				&FONTC_OFFWHITE, 8, &zoomInRect, &zoomInRect, CR_ABSOLUTE,
				"media\\images\\dmbtnbg.png");
			zoomInBtn.SetFunction(LEFTCLICK,
				[&](int a, int b, int c) {
					Log("Zooming in");
					RENDERZOOM = RENDERZOOM * 2;
					if (RENDERZOOM > 2) RENDERZOOM = 2.f;

					SDL_RenderSetScale(gRenderer, RENDERZOOM, RENDERZOOM);

					reloadLevelRect.x = reloadLevelRect.x / RENDERZOOM;
					reloadLevelRect.y = reloadLevelRect.y / RENDERZOOM;
					reloadLevelRect.w = reloadLevelRect.w / RENDERZOOM;
					reloadLevelRect.h = reloadLevelRect.h / RENDERZOOM;
					zoomInRect.x = zoomInRect.x / RENDERZOOM;
					zoomInRect.y = zoomInRect.y / RENDERZOOM;
					zoomInRect.w = zoomInRect.w / RENDERZOOM;
					zoomInRect.h = zoomInRect.h / RENDERZOOM;
					zoomOutRect.x = zoomOutRect.x / RENDERZOOM;
					zoomOutRect.y = zoomOutRect.y / RENDERZOOM;
					zoomOutRect.w = zoomOutRect.w / RENDERZOOM;
					zoomOutRect.h = zoomOutRect.h / RENDERZOOM;
					zoomResetRect.x = zoomResetRect.x / RENDERZOOM;
					zoomResetRect.y = zoomResetRect.y / RENDERZOOM;
					zoomResetRect.w = zoomResetRect.w / RENDERZOOM;
					zoomResetRect.h = zoomResetRect.h / RENDERZOOM;
					return true;
				});
			clickables->push_back(&zoomInBtn);

			// Zoom out button
			ClickButton zoomOutBtn = ClickButton("Zoom Out", 14, FONT_TYPED,
				&FONTC_OFFWHITE, 8, &zoomOutRect, &zoomOutRect, CR_ABSOLUTE,
				"media\\images\\dmbtnbg.png");
			zoomOutBtn.SetFunction(LEFTCLICK,
				[&](int a, int b, int c) {
					Log("Zooming out");
					RENDERZOOM = RENDERZOOM/2;
					if (RENDERZOOM < 0.1) RENDERZOOM = 0.1f;

					SDL_RenderSetScale(gRenderer, RENDERZOOM, RENDERZOOM);

					reloadLevelRect.x = reloadLevelRect.x / RENDERZOOM;
					reloadLevelRect.y = reloadLevelRect.y / RENDERZOOM;
					reloadLevelRect.w = reloadLevelRect.w / RENDERZOOM;
					reloadLevelRect.h = reloadLevelRect.h / RENDERZOOM;
					zoomInRect.x = zoomInRect.x / RENDERZOOM;
					zoomInRect.y = zoomInRect.y / RENDERZOOM;
					zoomInRect.w = zoomInRect.w / RENDERZOOM;
					zoomInRect.h = zoomInRect.h / RENDERZOOM;
					zoomOutRect.x = zoomOutRect.x / RENDERZOOM;
					zoomOutRect.y = zoomOutRect.y / RENDERZOOM;
					zoomOutRect.w = zoomOutRect.w / RENDERZOOM;
					zoomOutRect.h = zoomOutRect.h / RENDERZOOM;
					zoomResetRect.x = zoomResetRect.x / RENDERZOOM;
					zoomResetRect.y = zoomResetRect.y / RENDERZOOM;
					zoomResetRect.w = zoomResetRect.w / RENDERZOOM;
					zoomResetRect.h = zoomResetRect.h / RENDERZOOM;
					return true;
				});
			clickables->push_back(&zoomOutBtn);

			// Reset zoom button
			ClickButton zoomResetBtn = ClickButton("Reset zoom", 14, FONT_TYPED,
				&FONTC_OFFWHITE, 8, &zoomResetRect, &zoomResetRect, CR_ABSOLUTE,
				"media\\images\\dmbtnbg.png");
			zoomResetBtn.SetFunction(LEFTCLICK,
				[&](int a, int b, int c) {
					Log("Zoom reset");
					RENDERZOOM = 1.f;

					SDL_RenderSetScale(gRenderer, RENDERZOOM, RENDERZOOM);

					reloadLevelRect.x = reloadLevelRect.x / RENDERZOOM;
					reloadLevelRect.y = reloadLevelRect.y / RENDERZOOM;
					reloadLevelRect.w = reloadLevelRect.w / RENDERZOOM;
					reloadLevelRect.h = reloadLevelRect.h / RENDERZOOM;
					zoomInRect.x = zoomInRect.x / RENDERZOOM;
					zoomInRect.y = zoomInRect.y / RENDERZOOM;
					zoomInRect.w = zoomInRect.w / RENDERZOOM;
					zoomInRect.h = zoomInRect.h / RENDERZOOM;
					zoomOutRect.x = zoomOutRect.x / RENDERZOOM;
					zoomOutRect.y = zoomOutRect.y / RENDERZOOM;
					zoomOutRect.w = zoomOutRect.w / RENDERZOOM;
					zoomOutRect.h = zoomOutRect.h / RENDERZOOM;
					zoomResetRect.x = zoomResetRect.x / RENDERZOOM;
					zoomResetRect.y = zoomResetRect.y / RENDERZOOM;
					zoomResetRect.w = zoomResetRect.w / RENDERZOOM;
					zoomResetRect.h = zoomResetRect.h / RENDERZOOM;

					return true;
				});
			clickables->push_back(&zoomResetBtn);

			// Debug stats
			RenderingString debugFPS = RenderingString("", &FONTC_WHITE,
				FONT_TYPED, ALIGN_LEFT, VERT_ANCHOR_CENTER, 12);
			SDL_Rect debugFPSRect = {
				SCREEN_WIDTH - 200,
				4,
				200,
				12};

			RenderingString debugCursorPos = RenderingString("", &FONTC_WHITE,
				FONT_TYPED, ALIGN_LEFT, VERT_ANCHOR_CENTER, 12);
			SDL_Rect debugCursorPosRect = {
				SCREEN_WIDTH - 200,
				28,
				200,
				12};
			
			RenderingString debugGCursorPos = RenderingString("", &FONTC_WHITE,
				FONT_TYPED, ALIGN_LEFT, VERT_ANCHOR_CENTER, 12);
			SDL_Rect debugGCursorPosRect = {
				SCREEN_WIDTH - 200,
				52,
				200,
				12};

			RenderingString debugTCursorPos = RenderingString("", &FONTC_WHITE,
				FONT_TYPED, ALIGN_LEFT, VERT_ANCHOR_CENTER, 12);
			SDL_Rect debugTCursorPosRect = {
				SCREEN_WIDTH - 200,
				76,
				200,
				12};

			RenderingString debugPlayerPos = RenderingString("", &FONTC_WHITE,
				FONT_TYPED, ALIGN_LEFT, VERT_ANCHOR_CENTER, 12);
			SDL_Rect debugPlayerPosRect = {
				SCREEN_WIDTH - 200,
				100,
				200,
				12};

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

				// Mouse handling
				SDL_PumpEvents();
				SDL_GetMouseState(&mx, &my);
				gmx = mx + Camera::x;
				gmy = my + Camera::y;

				newDMWaiting = false;

				hoveredCRs = ClickRegion::GetRegionsAtMouse(gmx, gmy);
				hoveredDMs = DropMenu::GetDMsAtMouse(gmx, gmy);

				Actor::renderingActors.clear();

				// Update cursor


            // Handle events in the queue
            while(SDL_PollEvent(&e) != 0) {
               // User requests quit
               if (e.type == SDL_QUIT) {
                  quit = true;
               } else if (e.type == SDL_MOUSEMOTION || // Handle mouse input
						e.type == SDL_MOUSEBUTTONDOWN ||
						e.type == SDL_MOUSEBUTTONUP ||
						e.type == SDL_MOUSEWHEEL) {
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

								// User has clicked in an area with no buttons or
								// clickRegions, meaning they want to move to this spot
								player.PlotMovement(&randomLevel, gmx, gmy);
								player.SetCurrentSpeed(2);
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

				// Camera control
				bool W = keyStates[SDL_SCANCODE_W];
				bool S = keyStates[SDL_SCANCODE_S];
				bool A = keyStates[SDL_SCANCODE_A];
				bool D = keyStates[SDL_SCANCODE_D];

				// Player movement speed controls
				bool ctrl = keyStates[SDL_SCANCODE_LCTRL];
				bool shift = keyStates[SDL_SCANCODE_LSHIFT];

				// System controls
				bool bksp = keyStates[SDL_SCANCODE_BACKSPACE];

				if (W && !S) Camera::SetYDirNeg();
				else if (!W && S) Camera::SetYDirPos();

				if (A && !D) Camera::SetXDirNeg();
				else if (!A && D) Camera::SetXDirPos();

				if (ctrl) player.SetCurrentSpeed(1);
				else if (shift) player.SetCurrentSpeed(3);

				if (bksp) Close();

				// Handle character movements
				player.HandleMovement(&randomLevel);

				// Handle collisions
				// First, players
				// if (player.queueCollisions) {
				// 	// Check collisions with other characters, and then walls
				// 	CheckWallCollisions(&player, &randomLevel.walls);
				// 	for (int i = 0; i < (&randomLevel.doors)->size(); i++) {
				// 		CheckDoorCollisions(&player, &randomLevel.doors[i]->part1);
				// 		CheckDoorCollisions(&player, &randomLevel.doors[i]->door);
				// 		CheckDoorCollisions(&player, &randomLevel.doors[i]->part2);
				// 	}
				// }

				// Next, other characters
				// Finally, items
				// Next, projectiles

				// Clear screen
				SDL_RenderClear(gRenderer);

				int screenFrame = countedFrames%SCREEN_FPS;

				if (Camera::focusMode == FOCUS_PLAYER) {
					Camera::SetFocusOnActor(&player);
				}

				// Render everything to screen
				// Order of renders:
				// 1. floor (if we want to implement multiple floors later on,
				//		this will need to be part of a loop)
				// 2. Walls behind actor(s) at current y position
				// 3. Doors behind actor(s) at current y position
				// 4. Actor(s) at current y position
				// 5. Walls after current y position
				// 6. Doors after current y position
				// Iterate through the list of actors here, but until we have more
				// than one actor, we can do this without iterations
				// We need a vector of actors, sorted by their Y position. If two
				// of them tie, it doesn't matter
				// We iterate through this vector, and get the Y tile of the
				// current actor so that we can chop up the walls

				// First, check through all actors and see which ones are rendering
				for (int ai = 0; ai < Actor::allActors.size(); ai++) {
					if (Actor::allActors[ai]->CheckRendering()) {
						Actor::renderingActors.push_back(Actor::allActors[ai]);
					}
				}

				// Old code for rendering walls and Characters
				// randomLevel.RenderWalls(currentY,
				// 	player.hitBox.y + player.hitBox.h);
				// randomLevel.RenderDoors(currentY,
				// 	player.hitBox.y + player.hitBox.h);
				
				// currentY = player.hitBox.y + player.hitBox.h;
				
				// Render players
				// player.Render(screenFrame);
				
				// Render NPCs
				// for (auto npc = activeNPCs.begin(); npc < activeNPCs.end(); npc++) {
				// 	(*npc).Render(screenFrame);
				// }

				// Render walls and actors
				// TEST SECTION 2
				int currentY = Camera::y;

				randomLevel.RenderFloor();

				Actor::SortRenderingActorVector();

				for (int iter = 0; iter < Actor::renderingActors.size(); iter++) {
					RenderedActor* actor = Actor::renderingActors[iter];
					int newY;// = (int)actor->yPos + actor->hitBoxYOffset + 
					// actor->hitBox.h;
					actor->GetFoot(NULL, &newY);
					randomLevel.RenderWalls(currentY, newY);
					randomLevel.RenderDoors(currentY, newY);

					actor->Render(screenFrame);
					
					currentY = newY;
				}

				// TODO: This cameraHeight isn't high enough - it's clipping the very bottom of the screen.
				int cameraHeight = Camera::GetRect().h;
				randomLevel.RenderWalls(currentY, Camera::y + cameraHeight);
				randomLevel.RenderDoors(currentY, Camera::y + cameraHeight);

				// Render UI
				for (auto d : randomLevel.doors) {
					d->dropmenu->Render();
				}

				// Render debugging tools & data
				reloadLevelBtn.Render(&reloadLevelRect);
				zoomInBtn.Render(&zoomInRect);
				zoomOutBtn.Render(&zoomOutRect);
				zoomResetBtn.Render(&zoomResetRect);

				debugFPS.str = "FPS: " + to_string((double)avgFPS);
				debugCursorPos.str = "cX: " + to_string(mx) + 
					", cY: " + to_string(my);
				debugGCursorPos.str = "cGX: " + to_string(mx + (int)Camera::x) + 
					", cGY: " + to_string(my + (int)Camera::y);
				debugTCursorPos.str = "cTX: " + to_string((int)((mx + (int)Camera::x)/(PIXELSPERFEET * GZOOM * 5))) + ", cTY: " + to_string((int)((my + (int)Camera::y)/(PIXELSPERFEET * GZOOM * 5)));
				debugPlayerPos.str = "pX: " + to_string((int)player.xPos) + 
					", pY: " + to_string((int)player.yPos);
				debugFPS.Render(&debugFPSRect);
				debugCursorPos.Render(&debugCursorPosRect);
				debugGCursorPos.Render(&debugGCursorPosRect);
				debugTCursorPos.Render(&debugTCursorPosRect);
				debugPlayerPos.Render(&debugPlayerPosRect);

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

