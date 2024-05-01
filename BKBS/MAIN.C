//
// Hi! This is hardrockbluesky, standing on the shoulders of giants.
// The base for this game is MBDesigns' project at https://mbdesigns.itch.io/ps1-homebrew-source-code-rendering-3d-graphics-with-psyq
// I've made pretty extensive edits and additions, treating his project as an engine to build on. Basically anything that isn't "Display and move a single object" is me.
// I leaned towards over-explaining because I want to see more games get made.
// Anything not explained by my [or his] comments is likely documented in PSY-Q's reference pdf's or it's just good ol' C.
// Original header notes below:
//
////////////////////////////////////////////////////////////////////////////
//
// Some of The code in this project comes from a combination of things from
// Wituz's Playstation Development series on youtube
// and Lameguy64's TMD model viewer example.
//
// Edits by Rubixcube6 aka MBDesigns
// Website: https://www.masonbarrydesigns.com/
//
////////////////////////////////////////////////////////////////////////////

//Include all the features you want for your project here
//Each of these files contains a set of functions you can
//use in your projects.

#include "Constants.c"
#include "Controller.c"
#include "ReadCD.c"
#include "Audio.c"
#include "2D.c"
#include "3D.c"
#include "Cine.c"

// Define your constants here. Many of the vars below could/should probably be moved up here at some point.
#define NUM_NOTES 30
#define GOAL_Y 90

// Do not pass expressions into these macros, only plain values. Make function versions if you want to use expressions!
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LERP(a,b,t) ((a*(4096-t) + b * t) >> 12)

// Declare any function you make here
void Initialize();
void Start();
void Update();
void Render();
void Controls();
void CamControls();
char ArrowUpdate();
void CamUpdate();
void LoadingScreen();
void EndingScreen();
char ReadChart(unsigned char memLoc, char whichBit);



//////////////////////////////////////
////    Game variables go here    ////
//////////////////////////////////////

// How fast the camera moves
int movementSpeed = 20;

// How fast the camera turns
int rotationSpeed = 10;

// Store your CD Files in these pointers
// The array length is how many files
// you eventually want to load.
u_long* cdData[10];

// Pointer for reading chart data from loaded file
u_char* currentAddress;

// Used to sync audio
int frameCount = 0;
int lastVSync = 0;
int frameDelay = 12;
int totalRCNT = 0;
int frameLagTotal = 0;

// Control arrowNote default color
u_char desaturation = 80;

// Every TMD model you load needs a struct (which is basically just the transform) These should just be their own typedef, honestly.
struct {
	VECTOR position;
	SVECTOR rotation;
} house1;

struct {
	VECTOR position;
	SVECTOR rotation;
} house2;

struct {
	VECTOR position;
	SVECTOR rotation;
} house3;

struct {
	VECTOR position;
	SVECTOR rotation;
} house4;

// Goalline for the notes
GsBOXF noteField;

// Note beams
GsBOXF beam1, beam2, beam3, beam4;

// Create a pool of Arrow notes and 
// a counter for most recently accessed note
struct arrowNote {
	GsSPRITE arrowSprite;
	char active;
	char hit;
	char pad1, pad2; // There's probably a better way to align these but I don't know it yet!
};

struct arrowNote arrowNotes[NUM_NOTES];

unsigned char currentNote;

// Pools for visual effects and pool index counter
struct Diamond {
	GsLINE lines[4];
	char active;
	char pad1, pad2, pad3;
};

struct Diamond diamonds[8];

unsigned char currentDiamond;

struct Ripple {
	GsSPRITE rippleSprite;
	char active;
	char pad1, pad2, pad3;
};

struct Ripple ripples[8];

unsigned char currentRipple;

// Controls which effects are shown
unsigned char effectType = 5;

// Which frame to start the end credits on
int endingFrame = 6060;

// Game state var
int playing;

// Camera cine vars, global for fast re-init [PS1 can be weird and I'm still learning the lower level reasons]
int APos;
int BPos;
int lerpTime;


//////////////////////////////////////////////////////////BEGIN/////////////////////////////////////////////////////////////////

// The main function is the first
// thing that runs in your game
int main() {
	
	Initialize();
	
	while(1) {
		
		Start();
		
		// This is the main loop of your
		// game that will run every frame
		while( playing == 1 ) {
			Update();
			Render();
		}
		printf("restarting...\n");
	}
	
	return 0;
}

// This function runs once right when the game launches, loads and sets data for play
void Initialize() {
	
	// clear out the reverb from boot sounds
	audio_init();

	// Get the CD system started so you can read files
	ReadCDInit();

	// Do all the necessary steps for getting
	// the screen ready to display things
	initializeScreen();

	// Get the controller system ready for
	// reading input from the controller
	initializePad();
	
	// The next part is going to take a while, display loading screen
	LoadingScreen();

	// Start reading the CD
	cd_open();

	// Read a specific file by name and
	// store a pointer to it in the cdData variable.
	// (make sure to edit mkpsxiso/cuesheet.xml and
	// add it there or it won't be included on the CD)
	// Think of these array indices as file slots.
	cd_read_file("HOUSE1.TMD", &cdData[0]);
	cd_read_file("HOUSE1.TIM", &cdData[1]);
	cd_read_file("HOUSE2.TMD", &cdData[2]);
	cd_read_file("HOUSE2.TIM", &cdData[3]);
	cd_read_file("HOUSE3.TMD", &cdData[4]);
	cd_read_file("HOUSE3.TIM", &cdData[5]);
	cd_read_file("HOUSE4.TMD", &cdData[6]);
	cd_read_file("HOUSE4.TIM", &cdData[7]);
	cd_read_file("CHART.BIN", &cdData[8]);
	cd_read_file("RIPPLE.TIM", &cdData[9]);
	cd_read_file("ARROWS.TIM", &cdData[10]);

	// Stop reading the CD
	cd_close();

	// Load the TIM textures into VRAM
	// The number is the slot you want to load from.
	loadTexture((u_char *)cdData[1]);
	loadTexture((u_char *)cdData[3]);
	loadTexture((u_char *)cdData[5]);
	loadTexture((u_char *)cdData[7]);
	loadTexture((u_char *)cdData[9]);
	loadTexture((u_char *)cdData[10]);
	
	// Load TMD models

	// LoadTMD(
	//    Pointer to the TMD Model to load,
	//    The Object variable to store the TMD model in [defined in 3D.C],
	//    Lighting on=1 off=0
	// );
	// Returns number of models loaded from the TMD

	ObjectCount += LoadTMD(cdData[0], &Object[0], 0);
	ObjectCount += LoadTMD(cdData[2], &Object[1], 0);
	ObjectCount += LoadTMD(cdData[4], &Object[2], 0);
	ObjectCount += LoadTMD(cdData[6], &Object[3], 0);
	
	// Fill arrays for camera movement
	initCine();
}

// This function runs right after the Initialize function
// and before the Update function, the game restarts here after each loop.
// Anything that needs to be set/reset at the beginning of play needs to be
// set here.
void Start() {

	// Just a for loop counter
	int i;

	// Set all the initial starting positions and
	// rotations here for every loaded object
	// The V in vx, vy, and vz stands for Vector, ezpz
	// This project doesn't really use any!

	// Camera default position (overwritten by cine system but here for debug)
	Camera.position.vx = -1550;
	Camera.position.vy = 600;
	Camera.position.vz = 3950;
	Camera.rotation.vy = 600;
	
	/////////////////////////////////////////////
	////    Setup the scene lighting here    ////
	/////////////////////////////////////////////
	
	// This project doesn't use lighting except for the BG color so if you need that stuff it's in MBDesign's project

	// The background color of the scene
	// in RGB (values between 0-255)
	SetBGColor(130, 228, 255);
	
	///////////////////////////////////////////////
	////   Setup goalline and effects here     ////
	///////////////////////////////////////////////
	
	noteField.x = -200;
	noteField.y = GOAL_Y;
	noteField.w = 400;
	noteField.h = 1;
	noteField.attribute=(1<<30); // bit 30 is usually semi-trans toggle bit for GS types
	
	beam1.x = -30;
	beam2.x = -10;
	beam3.x = 10;
	beam4.x = 30;
	beam1.y = beam2.y = beam3.y = beam4.y = -120;
	beam1.h = beam2.h = beam3.h = beam4.h = 240;
	//beam1.r = beam1.g = beam1.b = 254;
	beam1.attribute=(3<<28) + (1<<30); // bit 28 is usually semi-trans blend mode for GS types
	beam2.attribute=(3<<28) + (1<<30);
	beam3.attribute=(3<<28) + (1<<30);
	beam4.attribute=(3<<28) + (1<<30);
	
	// Set up diamonds
	for ( i = 0; i < 8; i++ ) {
		int j;
		for ( j = 0; j < 4; j++ ) {
			diamonds[i].lines[j].attribute=(1<<28) + (1<<30);
		}
	}
	
	// Set up ripples
	for ( i = 0; i < 8; i++ ) { 
			ripples[i].rippleSprite.w = 66;
			ripples[i].rippleSprite.h = 66;
			ripples[i].rippleSprite.u = 0;
			ripples[i].rippleSprite.v = 0;
			ripples[i].rippleSprite.y = GOAL_Y;
			ripples[i].rippleSprite.tpage = 13;
			ripples[i].rippleSprite.cx = 0;
			ripples[i].rippleSprite.cy = 481;
			ripples[i].rippleSprite.scalex = 0;
			ripples[i].rippleSprite.scaley = 0;
			ripples[i].rippleSprite.mx = ripples[i].rippleSprite.w/2;
			ripples[i].rippleSprite.my = ripples[i].rippleSprite.h/2;
			ripples[i].rippleSprite.attribute=(0<<24)+(0<<6)+(1<<28)+(1<<30);
	}
	
	// Set up arrowNotes
	for ( i = 0; i < NUM_NOTES; i++ ) { 
			arrowNotes[i].arrowSprite.w = 16;
			arrowNotes[i].arrowSprite.h = 16;
			arrowNotes[i].arrowSprite.v = 1;
			arrowNotes[i].arrowSprite.y = 120;
			arrowNotes[i].arrowSprite.cx = 48;
			arrowNotes[i].arrowSprite.cy = 498;
			arrowNotes[i].arrowSprite.tpage = 25;
			arrowNotes[i].arrowSprite.scalex = 4096;
			arrowNotes[i].arrowSprite.scaley = 4096;
			arrowNotes[i].arrowSprite.mx = arrowNotes[i].arrowSprite.w/2;
			arrowNotes[i].arrowSprite.my = arrowNotes[i].arrowSprite.h/2;
			arrowNotes[i].arrowSprite.attribute=(0<<24)+(0<<6);//(2<<24)+(0<<6);
	}
	
	// Load the chartdata pointer
	currentAddress = cdData[8];
	
	// Init camera position vars
	APos = 0;
	BPos = 1;
	lerpTime = 0;
	
	// Init framecount and lag counter
	frameCount = 0;
	frameLagTotal = 0;
	
	// Set playing to 1(true) so our tick runs in main after this function returns. 
	// This will get flipped to 0 at the end of play, leaving the tick loop and restarting the game.
	playing = 1;
	
	// Start playing CD track next to last so it's as close to the first tick as possible
	PlaySyncedTrack();
	
	// VSync(-1) returns the number of syncs [~60/s for NTSC] since .exe was started.
	// We'll use this to compensate for missed frames in the note update logic.
	// This should be the last thing before tick starts
	lastVSync = VSync(-1) + (frameDelay * 2);
}

// This is where the action happens. This function runs every frame.
void Update () {
	
	frameCount++;
	
	// This gets the status of all the controller buttons
	padUpdate();

	// Update arrowNotes, delay to ensure notes are synced. 
	// To advance note position instead of delay, add silence to the beginning of track. 
	// Don't update arrows after the end credits, it'll keep reading junk data in as arrows.
	if ( frameCount > frameDelay && frameCount < endingFrame ) {
		ArrowUpdate();
	}
	
	// Move the cine camera
	CamUpdate();

	// Regular gameplay controls
	Controls();
	
	// Debug camera controls
	//CamControls();
	
	if ( frameCount > endingFrame ) {
		EndingScreen();
	}
	
}

// This function runs right after the Update
// function and displays everything on screen.
void Render () {
	
	unsigned char i;

	// Prepare for rendering
	clear_display();

	// Render debug text. this is good for seeing what
	// some variables are doing while the game is running
	// or just to display some text to the player.
	//FntPrint("camera Position: (%d, %d, %d)\n", Camera.position.vx, Camera.position.vy, Camera.position.vz);
	//FntPrint("house1 Rotation: (%d, %d, %d)\n", Camera.rotation.vx, Camera.rotation.vy, Camera.rotation.vz);

	// Calculate the camera and viewpoint
	CalculateCamera();

	// Sort and render objects
	RenderObject(house1.position, house1.rotation, &Object[0]);
	RenderObject(house2.position, house2.rotation, &Object[1]);
	RenderObject(house3.position, house3.rotation, &Object[2]);
	RenderObject(house4.position, house4.rotation, &Object[3]);
	
	// Sort and render goalline - for all following GsSort functions,
	// arg format is pointer to object, ordering table to sort the object into, draw order starting with 0.
	GsSortBoxFill(&noteField, &orderingTable[myActiveBuff], 2);
	
	// Sort beams
	if ( effectType > 2 ) {
		GsSortBoxFill(&beam1, &orderingTable[myActiveBuff], 3);
		GsSortBoxFill(&beam2, &orderingTable[myActiveBuff], 3);
		GsSortBoxFill(&beam3, &orderingTable[myActiveBuff], 3);
		GsSortBoxFill(&beam4, &orderingTable[myActiveBuff], 3);
	}
	
	// Sort active diamonds
	if ( effectType == 2 || effectType == 5 ) {
		for ( i = 0; i < 8; i++ ) {
			int j;
			for ( j = 0; j < 4; j++ ) {
				if ( diamonds[i].active == 1 ) {
					GsSortLine(&diamonds[i].lines[j], &orderingTable[myActiveBuff], 0);
				}
			}
		}
	}
	
	// Sort active ripples
	if ( effectType == 1 || effectType == 4 ) {
		for ( i = 0; i < 8; i++ ) {
			if ( ripples[i].active == 1 ) {
				GsSortSprite(&ripples[i].rippleSprite, &orderingTable[myActiveBuff], 0);
			}
		}
	}
	
	// Sort arrow notes
	for ( i = 0; i < NUM_NOTES; i++ ) {
		if ( arrowNotes[i].arrowSprite.y < 115 ) {
		GsSortSprite(&arrowNotes[i].arrowSprite, &orderingTable[myActiveBuff], 1);
		}
	}
	
	// Showtime!
	Display();

}

void Controls () {

	// Could do this as a callback event instead, need to research
	if ( padCheckPressed(Pad1Select) ) {
		// Scroll through effect types
		if ( effectType == 5 ) {
			effectType = 0;
		} else {
			effectType++;
		}
	}
	
}

void CamControls () {
	
    // Temp vector for local movement
    VECTOR newMoveVec;
	
	if (padCheck(Pad1Left)) {
		// Move cam left
		newMoveVec = TranslateCam(Camera.rotation, movementSpeed, 0, 0);
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Right)) {
		// Move cam right
		newMoveVec = TranslateCam(Camera.rotation, -movementSpeed, 0, 0);
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}
	
	if (padCheck(Pad1Up)) {
		// Move cam forward
		newMoveVec = TranslateCam(Camera.rotation, 0, 0, -movementSpeed);
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}
	
	if (padCheck(Pad1Down)) {
		// Move cam backwards
		newMoveVec = TranslateCam(Camera.rotation, 0, 0, movementSpeed);
		Camera.position.vx += newMoveVec.vx;
		Camera.position.vy += newMoveVec.vy;
		Camera.position.vz += newMoveVec.vz;
	}

	if (padCheck(Pad1Cross)) {
		// Move cam down
		Camera.position.vy -= rotationSpeed;
	}

	if (padCheck(Pad1Square)) {
		// Move cam up
		Camera.position.vy += rotationSpeed;
	}
	
	if (padCheck(Pad1L1)) {
		// Tilt cam back
		Camera.rotation.vx -= rotationSpeed;
	}
	
	if (padCheck(Pad1R1)) {
		// Tilt cam forward
		Camera.rotation.vx += rotationSpeed;
	}
	
	if (padCheck(Pad1L2)) {
		// Turn cam Left
		Camera.rotation.vy += rotationSpeed;
	}
	
	if (padCheck(Pad1R2)) {
		// Turn cam Right
		Camera.rotation.vy -= rotationSpeed;
	}
	
}

// Check, move, hide, show notes and effects
char ArrowUpdate () {
	u_char i;
	static int frameDiff;
	int noteHit = 0;
	int noteMissed = 0;
	int random = rand();
	
	// Record frames since last ArrowUpdate so if we drop a frame or two the note movement compensates to match the CD audio
	frameDiff = VSync(-1) - lastVSync;
	frameLagTotal += frameDiff - 2;
	lastVSync = VSync(-1);
	//FntPrint("vsync diff = %d\n", frameDiff);
	//FntPrint("frameLagTotal = %d\n", frameLagTotal);
	
	// Run through the first four bits of the current chart byte
	// and activate notes, sending them to their spawn location
	for (i = 0; i < 4; i++) {
        if (0 != ReadChart(*currentAddress, i)) {
			arrowNotes[currentNote].active = 1;
			arrowNotes[currentNote].hit = 0;
			// Set x based on i and reset y to start point
			arrowNotes[currentNote].arrowSprite.x = (i * 20) - 30;
			arrowNotes[currentNote].arrowSprite.y = -240 + frameLagTotal;
			// Reset scale
			arrowNotes[currentNote].arrowSprite.scalex = ONE;
			arrowNotes[currentNote].arrowSprite.scaley = ONE;
			// Reset transparency
			arrowNotes[currentNote].arrowSprite.attribute=(0<<24)+(0<<6)+(1<<28)+(0<<30);
			// Set u-coord to select note sprite based on direction
			arrowNotes[currentNote].arrowSprite.u = (i * 16);
			// Color stuff
			// Desaturation = 0-128
			// Uses same random value for all notes per line so they're the same color
			if (random < 11000) {
				arrowNotes[currentNote].arrowSprite.r = 128; 
				arrowNotes[currentNote].arrowSprite.g = desaturation;
				arrowNotes[currentNote].arrowSprite.b = desaturation;
			} else if (random < 22000) {
				arrowNotes[currentNote].arrowSprite.r = desaturation;
				arrowNotes[currentNote].arrowSprite.g = 128; 
				arrowNotes[currentNote].arrowSprite.b = desaturation;
			} else {
				arrowNotes[currentNote].arrowSprite.r = desaturation;
				arrowNotes[currentNote].arrowSprite.g = desaturation;
				arrowNotes[currentNote].arrowSprite.b = 128; 
			}
			// Switch to the next note in the pool or reset to 0
			if (currentNote == NUM_NOTES-1) {
				currentNote = 0;
			} else {
				currentNote++;
			}
		}
    }
	
	// Advance to next frame's address
	currentAddress++;
	
	// Debug stuff to check the note chart/bin
	//FntPrint("Address = %08x\n", cdData[8]);
	//FntPrint("Address = %08x\n", currentAddress);
	//FntPrint("Address value as char = %d\n", (unsigned char)*currentAddress);
	
	// Update arrowNotes
	for ( i = 0; i < NUM_NOTES; i++ ) {
		// Move notes, frameDiff is usually 2
		arrowNotes[i].arrowSprite.y += frameDiff;
		if ( arrowNotes[i].active == 1 ) {
			// Hue shift through colors
			if (arrowNotes[i].arrowSprite.r == 128) {
				if (arrowNotes[i].arrowSprite.b > desaturation) {
					arrowNotes[i].arrowSprite.b = arrowNotes[i].arrowSprite.b - 4;
				} else {
					arrowNotes[i].arrowSprite.g = arrowNotes[i].arrowSprite.g + 4;
					if (arrowNotes[i].arrowSprite.g == 128) {
					arrowNotes[i].arrowSprite.r = arrowNotes[i].arrowSprite.r - 4;
					}
				}
			} else if (arrowNotes[i].arrowSprite.g == 128) {
				if (arrowNotes[i].arrowSprite.r > desaturation) {
					arrowNotes[i].arrowSprite.r = arrowNotes[i].arrowSprite.r - 4;
				} else {
					arrowNotes[i].arrowSprite.b = arrowNotes[i].arrowSprite.b + 4;
					if (arrowNotes[i].arrowSprite.b == 128) {
					arrowNotes[i].arrowSprite.g = arrowNotes[i].arrowSprite.g - 4;
					}
				}
			} else {
				if (arrowNotes[i].arrowSprite.g > desaturation) {
					arrowNotes[i].arrowSprite.g = arrowNotes[i].arrowSprite.g - 4;
				} else {
					arrowNotes[i].arrowSprite.r = arrowNotes[i].arrowSprite.r + 4;
					if (arrowNotes[i].arrowSprite.r == 128) {
					arrowNotes[i].arrowSprite.b = arrowNotes[i].arrowSprite.b - 4;
					}
				}
			}
			// Check for hit if note in range of goal line
			if ( arrowNotes[i].arrowSprite.y > 83 && arrowNotes[i].arrowSprite.y < 102 ) {
				// Use the note's sprite UV to determine if it's left/up/down/right and do local hit effects
				switch (arrowNotes[i].arrowSprite.u){
					case 0:
						if ( padCheckPressed(Pad1Left) || padCheckPressed(Pad1Square) ) {
							int j = 0;
							arrowNotes[i].active = 0;
							arrowNotes[i].hit = 1;
							arrowNotes[i].arrowSprite.attribute=(0<<24)+(0<<6)+(1<<28)+(1<<30);
							// Spawn/update effects based on chosen effect type
							if ( effectType > 2 ) {
								beam1.r = arrowNotes[i].arrowSprite.r;
								beam1.g = arrowNotes[i].arrowSprite.g;
								beam1.b = arrowNotes[i].arrowSprite.b;
								beam1.w = 20;
								beam1.x = -40;
							}
							if ( effectType == 2 || effectType == 5 ) {
								diamonds[currentDiamond].active = 1;
								for ( j = 0; j < 4; j++ ) {
									diamonds[currentDiamond].lines[j].r = arrowNotes[i].arrowSprite.r;
									diamonds[currentDiamond].lines[j].g = arrowNotes[i].arrowSprite.g;
									diamonds[currentDiamond].lines[j].b = arrowNotes[i].arrowSprite.b;
									diamonds[currentDiamond].lines[j].x0 = -30;
									diamonds[currentDiamond].lines[j].x1 = -30;
									diamonds[currentDiamond].lines[j].y0 = GOAL_Y;
									diamonds[currentDiamond].lines[j].y1 = GOAL_Y;
								}
								if ( currentDiamond == 7 ) {
									currentDiamond = 0;
								} else {
									currentDiamond++;
								}
							}
							if ( effectType == 1 || effectType == 4 ) {
								ripples[currentRipple].active = 1;
								ripples[currentRipple].rippleSprite.r = arrowNotes[i].arrowSprite.r;
								ripples[currentRipple].rippleSprite.g = arrowNotes[i].arrowSprite.g;
								ripples[currentRipple].rippleSprite.b = arrowNotes[i].arrowSprite.b;
								ripples[currentRipple].rippleSprite.scalex = 360;
								ripples[currentRipple].rippleSprite.scaley = 360;
								ripples[currentRipple].rippleSprite.x = -30;
								if ( currentRipple == 7 ) {
									currentRipple = 0;
								} else {
									currentRipple++;
								}
							}
							arrowNotes[i].arrowSprite.r = 250;
							arrowNotes[i].arrowSprite.g = 250;
							arrowNotes[i].arrowSprite.b = 250;
							noteHit = 1;
						}
						break;
						
					case 16:
						if ( padCheckPressed(Pad1Down) || padCheckPressed(Pad1Cross) ) {
							int j = 0;
							arrowNotes[i].active = 0;
							arrowNotes[i].hit = 1;
							arrowNotes[i].arrowSprite.attribute=(0<<24)+(0<<6)+(1<<28)+(1<<30);
							if ( effectType > 2 ) {
								beam2.r = arrowNotes[i].arrowSprite.r;
								beam2.g = arrowNotes[i].arrowSprite.g; 
								beam2.b = arrowNotes[i].arrowSprite.b; 
								beam2.w = 20;
								beam2.x = -20;
							}
							if ( effectType == 2 || effectType == 5 ) {
								diamonds[currentDiamond].active = 1;
								for ( j = 0; j < 4; j++ ) {
									diamonds[currentDiamond].lines[j].r = arrowNotes[i].arrowSprite.r;
									diamonds[currentDiamond].lines[j].g = arrowNotes[i].arrowSprite.g;
									diamonds[currentDiamond].lines[j].b = arrowNotes[i].arrowSprite.b;
									diamonds[currentDiamond].lines[j].x0 = -10;
									diamonds[currentDiamond].lines[j].x1 = -10;
									diamonds[currentDiamond].lines[j].y0 = GOAL_Y;
									diamonds[currentDiamond].lines[j].y1 = GOAL_Y;
								}
								if ( currentDiamond == 7 ) {
									currentDiamond = 0;
								} else {
									currentDiamond++;
								}
							}
							if ( effectType == 1 || effectType == 4 ) {
								ripples[currentRipple].active = 1;
								ripples[currentRipple].rippleSprite.r = arrowNotes[i].arrowSprite.r;
								ripples[currentRipple].rippleSprite.g = arrowNotes[i].arrowSprite.g;
								ripples[currentRipple].rippleSprite.b = arrowNotes[i].arrowSprite.b;
								ripples[currentRipple].rippleSprite.scalex = 360;
								ripples[currentRipple].rippleSprite.scaley = 360;
								ripples[currentRipple].rippleSprite.x = -10;
								if ( currentRipple == 7 ) {
									currentRipple = 0;
								} else {
									currentRipple++;
								}
							}
							arrowNotes[i].arrowSprite.r = 250;
							arrowNotes[i].arrowSprite.g = 250;
							arrowNotes[i].arrowSprite.b = 250;
							noteHit = 1;
						}
						break;	
						
					case 32:
						if ( padCheckPressed(Pad1Up) || padCheckPressed(Pad1Triangle) ) {
							int j = 0;
							arrowNotes[i].active = 0;
							arrowNotes[i].hit = 1;
							arrowNotes[i].arrowSprite.attribute=(0<<24)+(0<<6)+(1<<28)+(1<<30);
							if ( effectType > 2 ) {
								beam3.r = arrowNotes[i].arrowSprite.r;
								beam3.g = arrowNotes[i].arrowSprite.g;
								beam3.b = arrowNotes[i].arrowSprite.b;
								beam3.w = 20;
								beam3.x = 0;
							}
							if ( effectType == 2 || effectType == 5 ) {
								diamonds[currentDiamond].active = 1;
								for ( j = 0; j < 4; j++ ) {
									diamonds[currentDiamond].lines[j].r = arrowNotes[i].arrowSprite.r;
									diamonds[currentDiamond].lines[j].g = arrowNotes[i].arrowSprite.g;
									diamonds[currentDiamond].lines[j].b = arrowNotes[i].arrowSprite.b;
									diamonds[currentDiamond].lines[j].x0 = 10;
									diamonds[currentDiamond].lines[j].x1 = 10;
									diamonds[currentDiamond].lines[j].y0 = GOAL_Y;
									diamonds[currentDiamond].lines[j].y1 = GOAL_Y;
								}
								if ( currentDiamond == 7 ) {
									currentDiamond = 0;
								} else {
									currentDiamond++;
								}
							}
							if ( effectType == 1 || effectType == 4 ) {
								ripples[currentRipple].active = 1;
								ripples[currentRipple].rippleSprite.r = arrowNotes[i].arrowSprite.r;
								ripples[currentRipple].rippleSprite.g = arrowNotes[i].arrowSprite.g;
								ripples[currentRipple].rippleSprite.b = arrowNotes[i].arrowSprite.b;
								ripples[currentRipple].rippleSprite.scalex = 360;
								ripples[currentRipple].rippleSprite.scaley = 360;
								ripples[currentRipple].rippleSprite.x = 10;
								if ( currentRipple == 7 ) {
									currentRipple = 0;
								} else {
									currentRipple++;
								}
							}
							arrowNotes[i].arrowSprite.r = 250;
							arrowNotes[i].arrowSprite.g = 250;
							arrowNotes[i].arrowSprite.b = 250;
							noteHit = 1;
						}
						break;
						
					case 48:
						if ( padCheckPressed(Pad1Right) || padCheckPressed(Pad1Circle) ) {
							int j = 0;
							arrowNotes[i].active = 0;
							arrowNotes[i].hit = 1;
							arrowNotes[i].arrowSprite.attribute=(0<<24)+(0<<6)+(1<<28)+(1<<30);
							if ( effectType > 2 ) {
								beam4.r = arrowNotes[i].arrowSprite.r;
								beam4.g = arrowNotes[i].arrowSprite.g; 
								beam4.b = arrowNotes[i].arrowSprite.b; 
								beam4.w = 20;
								beam4.x = 20;
							}
							if ( effectType == 2 || effectType == 5 ) {
								diamonds[currentDiamond].active = 1;
								for ( j = 0; j < 4; j++ ) {
									diamonds[currentDiamond].lines[j].r = arrowNotes[i].arrowSprite.r;
									diamonds[currentDiamond].lines[j].g = arrowNotes[i].arrowSprite.g;
									diamonds[currentDiamond].lines[j].b = arrowNotes[i].arrowSprite.b;
									diamonds[currentDiamond].lines[j].x0 = 30;
									diamonds[currentDiamond].lines[j].x1 = 30;
									diamonds[currentDiamond].lines[j].y0 = GOAL_Y;
									diamonds[currentDiamond].lines[j].y1 = GOAL_Y;
								}
								if ( currentDiamond == 7 ) {
									currentDiamond = 0;
								} else {
									currentDiamond++;
								}
							}
							if ( effectType == 1 || effectType == 4 ) {
								ripples[currentRipple].active = 1;
								ripples[currentRipple].rippleSprite.r = 128;
								ripples[currentRipple].rippleSprite.g = 128;
								ripples[currentRipple].rippleSprite.b = 128;
								ripples[currentRipple].rippleSprite.scalex = 360;
								ripples[currentRipple].rippleSprite.scaley = 360;
								ripples[currentRipple].rippleSprite.x = 30;
								if ( currentRipple == 7 ) {
									currentRipple = 0;
								} else {
									currentRipple++;
								}
							}
							arrowNotes[i].arrowSprite.r = 250;
							arrowNotes[i].arrowSprite.g = 250;
							arrowNotes[i].arrowSprite.b = 250;
							noteHit = 1;
						}
						break;
				}
			}
			// If player missed this note, mark inactive and flag that there was a note missed
			if ( arrowNotes[i].arrowSprite.y > 100 ) {
				arrowNotes[i].active = 0;
				noteMissed = 1;
			}
		} else {
			// If note is marked inactive and hit, flare and fade it
			if ( arrowNotes[i].hit == 1 ) {
				arrowNotes[i].arrowSprite.scalex = arrowNotes[i].arrowSprite.scalex + 100;
				arrowNotes[i].arrowSprite.scaley = arrowNotes[i].arrowSprite.scalex;
				arrowNotes[i].arrowSprite.r = arrowNotes[i].arrowSprite.r >> 1;
				arrowNotes[i].arrowSprite.g = arrowNotes[i].arrowSprite.g >> 1;
				arrowNotes[i].arrowSprite.b = arrowNotes[i].arrowSprite.b >> 1;
			}
		}
	}
	
	// Global hit/miss effects
	if (noteHit) {
		// If player hit, increase saturation by decreasing desat. must be done in same intervals as hueshift above, so 4. the minimum value for desaturation is just art preference.
		if (desaturation >= 80){
			desaturation -= 4;
		}
	} 
	
	if (noteMissed) {
		// If player missed, increase desat. Same idea as above.
		if (desaturation < 120){
			desaturation += 4;
		}
	}
	
	// Scale down beams if they were activated
	if (beam1.w > 0) {
		beam1.w-=2;
		beam1.x++;
	}
	if (beam2.w > 0) {
		beam2.w-=2;
		beam2.x++;
	}
	if (beam3.w > 0) {
		beam3.w-=2;
		beam3.x++;
	}
	if (beam4.w > 0) {
		beam4.w-=2;
		beam4.x++;
	}
	
	// Update diamonds
	for ( i = 0; i < 8; i++ ) {
		if ( diamonds[i].active == 1 ) {
			int j;
			if ( diamonds[i].lines[0].y0 > 120 ) {
				diamonds[i].active = 0;
				continue;
			}
			diamonds[i].lines[0].y0++;
			diamonds[i].lines[0].x1++;
			diamonds[i].lines[1].x0++;
			diamonds[i].lines[1].y1--;
			diamonds[i].lines[2].y0--;
			diamonds[i].lines[2].x1--;
			diamonds[i].lines[3].x0--;
			diamonds[i].lines[3].y1++;
			for ( j = 0; j < 4; j++ ) {
				if ( diamonds[i].lines[j].r < 4 ) {
				diamonds[i].lines[j].r = 0;
				} else {
				diamonds[i].lines[j].r -= 4;
				}
				if ( diamonds[i].lines[j].g < 4 ) {
				diamonds[i].lines[j].g = 0;
				} else {
				diamonds[i].lines[j].g -= 4;
				}
				if ( diamonds[i].lines[j].b < 4 ) {
				diamonds[i].lines[j].b = 0;
				} else {
				diamonds[i].lines[j].b -= 4;
				}
			}
		}
	}
	
	// Update ripples
	for ( i = 0; i < 8; i++ ) {
		if ( ripples[i].active == 1 ) {
			if ( ripples[i].rippleSprite.scalex > 4000 ) {
				ripples[i].active = 0;
				continue;
			}
			ripples[i].rippleSprite.scalex+=60;
			ripples[i].rippleSprite.scaley+=60;
			if ( ripples[i].rippleSprite.r < 4 ) {
			ripples[i].rippleSprite.r = 0;
			} else {
			ripples[i].rippleSprite.r -= 3;
			}
			if ( ripples[i].rippleSprite.g < 4 ) {
			ripples[i].rippleSprite.g = 3;
			} else {
			ripples[i].rippleSprite.g -= 3;
			}
			if ( ripples[i].rippleSprite.b < 4 ) {
			ripples[i].rippleSprite.b = 3;
			} else {
			ripples[i].rippleSprite.b -= 3;
			}
		}
	}
	
	return 0;
}


void CamUpdate () {
	
	// 4096/30fps = ~136.5 seconds
	VECTOR newMoveVec;
	
	// Advance/loop position index [if statement mostly for debug purposes, loops cine index]
	if ( APos == 35 ) {
			BPos = 0;
	} else {
		BPos = APos + 1;
	}
	
	// If cineSpeed[APos] > 0, lerp cam, else wiggle cam in place at APos
	if ( cineSpeed[APos] > 0 ) {
		lerpTime += cineSpeed[APos];
		if (lerpTime < 4096 ) {
			// Just do the lerp
			Camera.position.vx = LERP(cinePos[APos].vx, cinePos[BPos].vx, lerpTime);
			Camera.position.vy = LERP(cinePos[APos].vy, cinePos[BPos].vy, lerpTime);
			Camera.position.vz = LERP(cinePos[APos].vz, cinePos[BPos].vz, lerpTime);
			Camera.rotation.vx = LERP(cineRot[APos].vx, cineRot[BPos].vx, lerpTime);
			Camera.rotation.vy = LERP(cineRot[APos].vy, cineRot[BPos].vy, lerpTime);
			Camera.rotation.vz = LERP(cineRot[APos].vz, cineRot[BPos].vz, lerpTime);
		} else {
			// Finish the lerp and target the next position
			Camera.position.vx = cinePos[BPos].vx;
			Camera.position.vy = cinePos[BPos].vy;
			Camera.position.vz = cinePos[BPos].vz;
			Camera.rotation.vx = cineRot[BPos].vx;
			Camera.rotation.vy = cineRot[BPos].vy;
			Camera.rotation.vz = cineRot[BPos].vz;
			lerpTime = 0;
			if ( APos == 35 ) {
				APos = 0;
			} else {
				APos += 2;
			}
		}
	} else {
		// Wiggle cam in place at APos
		int tempX;
		int tempY;
		lerpTime += 10;
		// This combo of offsets will give us 2 figure 8's from 0-4096
		tempX = rsin(lerpTime<<1);
		tempY = rsin(lerpTime<<2);
		// Reduce tempX and tempY to a very small number per frame, adjust the bitshift to adjust how far the camera moves when wiggling
		newMoveVec = TranslateCam(Camera.rotation, (tempX >> 8), (tempY >> 8), 0);
		Camera.position.vx = (newMoveVec.vx + cinePos[APos].vx);
		Camera.position.vy = (newMoveVec.vy + cinePos[APos].vy);
		Camera.position.vz = (newMoveVec.vz + cinePos[APos].vz);
		Camera.rotation.vx = cineRot[APos].vx;
		Camera.rotation.vy = cineRot[APos].vy;
		Camera.rotation.vz = cineRot[APos].vz;
		if ( lerpTime > 4096 ) {
			lerpTime = 0;
			if ( APos == 35 ) {
				APos = 0;
			} else {
				APos++;
			}
		}
	}
}

// Bitshift comparison, returns 0 or 1 to determine which notes to activate from chart line.
// Should probably just be done inline, I had just learned what a bitshift was when I did this.
char ReadChart (unsigned char memLoc, char whichBit) {
	memLoc = memLoc << whichBit;
	//FntPrint("memLoc = %d\n", memLoc);
	return (memLoc & 10000000);
}

void LoadingScreen() {
	clear_display();
	FntPrint("\n\n\n\n\n\n\n\n\n\n\n    Bambi's Knees / Bambi's Shaking\n\n");
	Display();
}

void EndingScreen() {

	static int creditTimer;
	
	// Get rid of the goal line
	noteField.y = 500;
	
	// Run the credits
	if ( creditTimer < 90) {
	FntPrint("\n\n\n\n\n\n\n\n\n         I am made of blue sky\n\n\n");
	}
	if ( creditTimer >= 90 && creditTimer < 180) {
	FntPrint("\n\n\n\n\n\n\n\n\n         I am made of blue sky\n\n\n");
	FntPrint("             And hard rock\n\n\n");
	}
	if ( creditTimer >= 180 && creditTimer < 300) {
	FntPrint("\n\n\n\n\n\n\n\n\n         I am made of blue sky\n\n\n");
	FntPrint("             And hard rock\n\n\n");
	FntPrint("   And I will live this way forever\n\n\n");
	}
	if ( creditTimer > 330 && creditTimer < 450 ) {
	FntPrint("\n\n\n\n\n\n\n\n\n              Thanks to:\n\n\n");
	FntPrint("            @chained_tan -\n\n        character's base model");
	}
	if ( creditTimer > 480 && creditTimer < 630 ) {
	FntPrint("\n\n\n\n\n\n\n\n\n              Thanks to:\n\n\n");
	FntPrint("          MBGames, lameguy64,\n         and PSXDev Discord -\n\n       example projects, tools,\n           and assistance");
	}
	if ( creditTimer > 660 && creditTimer < 780 ) {
	FntPrint("\n\n\n\n\n\n\n\n\n              Thanks to:\n\n\n");
	FntPrint("    My bandmates from The Taubmen -\n\n          drums and trumpet");
	}
	if ( creditTimer > 810 && creditTimer < 1000 ) {
	FntPrint("\n\n\n\n\n\n\n\n\n           Everything else:\n\n\n\n");
	FntPrint("           @hardrockbluesky");
	}
	
	creditTimer++;
	
	if ( creditTimer > 1000 ) {
		creditTimer = 0;
		// Tell the main loop we've finished a playthrough, pause the CD drive [ see Audio.c for more on that ]
		playing = 0;
		DsCommand(0x09, NULL, NULL, 500);
	}
}
