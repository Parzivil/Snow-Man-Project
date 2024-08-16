/******************************************************************************
*
* Animation v1.0 (23/02/2021)
*
* This template provides a basic FPS-limited render loop for an animated scene.
*
******************************************************************************/
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "GLUE.h"

#define TARGET_FPS 60 // Target frame rate (number of Frames Per Second).

//Declare functions
void init(void);
void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void main(int argc, char** argv);
void think(void);
void GenerateParticles(void);
void reGenParticle(struct Particle p);
void drawSnowman(struct Location loc);

const unsigned int FRAME_TIME = 1000 / TARGET_FPS; // Ideal time each frame should be displayed for (in milliseconds).
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;
unsigned int frameStartTime = 0; // Time we started preparing the current frame (in milliseconds since GLUT was initialized).

char Snowing[1] = { 'y' };

unsigned int screenWidth = 1000;
unsigned int screenHeight = 800;
unsigned int frameCount = 0; //Frame count

int floorRandScale = 0;

//Snowman offset from default location
int snowmanXOffset = 0; //X offset 
int snowmanYOffset = 0; //Y offset

boolean showText = TRUE; //If the debug text is shown

//********************** Scene Colours **********************//
//Text Colour
const struct Colour textColour = { 0.0f, 0.0f, 0.0f };

//Tree Colours
const struct Colour trunk = { 0.28f, 0.21f, 0.11f };
struct Colour setLeaf = { 0.18f, 0.54f, 0.25f };
struct Colour leaf = { 0.18f, 0.54f, 0.25f };

//Snow Colour
struct Colour snowColour = { 1.0f, 1.0f, 1.0f };

//Snowman Colours 
const struct Colour noseColour = { 1, 0, 0 };
const struct Colour eyeColour = { 0.3, 0.3, 0.4 };
const struct Colour bodyColour = { 1.0f, 1.0f, 1.0f };

//Sky Colours
const struct Colour skyDarkColour = { 0.3f, 0.4f, 1.0f };
const struct Colour skyLightColour = { 0.7f, 0.7f, 1.0f };

//Floor Colours
const struct Colour floorDarkColour = { 0.55f, 0.6f, 0.7f };
const struct Colour floorLightColour = { 1, 1, 1 };

void init(void)
{
	// set background color to be black
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// set the drawing to be blue
	glColor3f(0.0f, 0.0f, 1.0f);
	// set the dot size to be 4 by 4 pixels
	glPointSize(10.0);
	// set to projection mode
	glMatrixMode(GL_PROJECTION);
	// load the identity matrix
	glLoadIdentity();
	// set window mode to 2D orthographica and set the window size
	gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);

	GenerateParticles();
	floorRandScale = GLUE_RAND(50);
}

void main(int argc, char** argv)
{
	srand(time(NULL)); //Set random seed
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow("Animation");
	// Set up the scene.
	init();
	// Disable key repeat (keyPressed or specialKeyPressed will only be calledonce when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);
	// Record when we started rendering the very first frame (which should happenafter we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);
	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

void display(void)
{
	struct Location snowmanLocation = { screenWidth / 2 + snowmanXOffset, screenHeight / 3 + snowmanYOffset }; //Location the snow man is reletive too
	struct Location tree1Location = { screenWidth / 2 - 200, screenHeight / 3 }; //Location the snow man is reletive too
	struct Location tree2Location = { screenWidth / 2 + 300, screenHeight / 3 - 200 }; //Location the snow man is reletive too

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw background
	glBegin(GL_POLYGON);
	GLUE_COLOUR(skyDarkColour);
	glVertex2f(0, screenHeight);
	glVertex2f(screenWidth, screenHeight);
	GLUE_COLOUR(skyLightColour);
	glVertex2f(screenWidth, 0);
	glVertex2f(0, 0);
	glEnd();

	//Draw Floor
	glBegin(GL_POLYGON);
	GLUE_COLOUR(floorDarkColour);
	glVertex2f(100, screenHeight / 3);
	glVertex2f(screenWidth - (100+ floorRandScale), screenHeight / 3);
	GLUE_COLOUR(floorLightColour);
	glVertex2f(screenWidth + (100 + floorRandScale), 0);
	glVertex2f(-100, 0);
	glEnd();

	GLUE_Tree(tree1Location, trunk, leaf, 10);

	if (snowmanLocation.y < (tree2Location.y + 110)) {
		//Snowman in front of tree
		GLUE_Tree(tree2Location, trunk, leaf, 15);
		drawSnowman(snowmanLocation);
	}
	else {
		//Snow man behind tree
		drawSnowman(snowmanLocation);
		GLUE_Tree(tree2Location, trunk, leaf, 15);
	}

	//*** Particle Drawing ***///
	for (int i = 0; i < MAX_PARTICLES; i++)	GLUE_DrawParticle(&GLUE_Particles[i]);

	//*** Text Drawing  ***//
	struct Location debugTextLocation = { 10, screenHeight - 20 };	 //Text location

	if (showText) {
		//Diagnostics text
		GLUE_Text("Diagnostics: ", debugTextLocation, textColour);
		GLUE_Text(GLUE_combineString(GLUE_stringValuei("- Particles : ", getParticleCount(screenHeight)), GLUE_combineString(" of ", GLUE_toStringi(MAX_PARTICLES))), GLUE_OffsetLocation(&debugTextLocation, 0, -20), textColour);
		GLUE_Text(GLUE_combineString("- Frame : ", GLUE_toStringi(frameCount)), GLUE_OffsetLocation(&debugTextLocation, 0, -40), textColour); //Frame count
		GLUE_Text(GLUE_stringValuei("- Target Frame Rate: ", TARGET_FPS), GLUE_OffsetLocation(&debugTextLocation, 0, -60), textColour); //Frame rate
		GLUE_Text(GLUE_combineString("- Snowing: ", Snowing), GLUE_OffsetLocation(&debugTextLocation, 0, -80), textColour); //Snowing state

		//Control text
		GLUE_Text("Scene Controls: ", GLUE_OffsetLocation(&debugTextLocation, 0, -100), textColour);
		GLUE_Text("- s: control snow ", GLUE_OffsetLocation(&debugTextLocation, 0, -120), textColour);
		GLUE_Text("- q/esc: quit ", GLUE_OffsetLocation(&debugTextLocation, 0, -140), textColour);
		GLUE_Text("- i: move snowman up ", GLUE_OffsetLocation(&debugTextLocation, 0, -160), textColour);
		GLUE_Text("- k: move snowman down ", GLUE_OffsetLocation(&debugTextLocation, 0, -180), textColour);
		GLUE_Text("- j: move snowman left ", GLUE_OffsetLocation(&debugTextLocation, 0, -200), textColour);
		GLUE_Text("- l: move snowman right ", GLUE_OffsetLocation(&debugTextLocation, 0, -220), textColour);
		GLUE_Text("- h: hide/show text ", GLUE_OffsetLocation(&debugTextLocation, 0, -240), textColour);
		GLUE_Text("Made by Robin N - 2024 ", GLUE_OffsetLocation(&debugTextLocation, 0, (-1 * screenHeight) + 30), textColour);
	}
	else {
		GLUE_Text(" - h: hide/show text ", GLUE_OffsetLocation(&debugTextLocation, 0, 0), textColour);
	}

	glutSwapBuffers(); // Send all output to display
}

/*
Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
	screenWidth = width;
	screenHeight = h;
}
/*
Called each time a character key (e.g. a letter, number, or symbol) is
pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{


	switch (tolower(key)) {

		//Toggle snow
	case KEY_S:
		if (Snowing[0] == 'y') Snowing[0] = 'n';
		else Snowing[0] = 'y';
		break;

		//Move the snowman left
	case KEY_J:
		if (snowmanXOffset > -380) snowmanXOffset -= 10;
		break;

		//Move the snowman right
	case KEY_L:
		if (snowmanXOffset < 380) snowmanXOffset += 10;
		break;

		//Move the snowman up
	case KEY_I:
		if (snowmanYOffset < 50) snowmanYOffset += 10;
		break;

		//Move the snowman down
	case KEY_K:
		if (snowmanYOffset > -260) snowmanYOffset -= 10;
		break;

		//Show/hide the text
	case KEY_H:
		if (showText) showText = FALSE;
		else showText = TRUE;
		break;

		//Quit the program
	case KEY_EXIT:
		exit(0);
		break;

	case KEY_Q:
		exit(0);
		break;
	}
}

void idle(void)
{
	// Wait until it's time to render the next frame.
	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) -
		frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
			// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}
	// Begin processing the next frame.
	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.
	think(); // Update our simulated world before the next call to display().
	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

void think(void)
{
	frameCount++;
	for (int i = 0; i < MAX_PARTICLES; i++) {
		GLUE_MoveParticle(&GLUE_Particles[i]);

		//WIP Getting the snow flakes to fall depending on their size "aka depth" 
		//I am sure there is a formula I can figure out for this...
		float scaler = (30 / screenHeight) * GLUE_Particles[i].size + 10;

		if (GLUE_Particles[i].location.y <= 0 && Snowing[0] == 'y') {
			reGenParticle(GLUE_Particles[i]);
		}
		else if (GLUE_Particles[i].location.y <= 0 && Snowing[0] == 'n') {
			GLUE_Particles[i].state = DEAD;
			GLUE_Particles[i].location.y = -50;
			GLUE_Particles[i].velocity.vx = 0;
			GLUE_Particles[i].velocity.vy = 0;
		}
	}
	if (Snowing[0] == 'y') {
		if (leaf.r < 1) leaf.r += 0.001f;
		if (leaf.g < 1) leaf.g += 0.001f;
		if (leaf.b < 1) leaf.b += 0.001f;
	}
	else {
		if (leaf.r > setLeaf.r) leaf.r -= 0.001f;
		if (leaf.g > setLeaf.g)leaf.g -= 0.001f;
		if (leaf.b > setLeaf.b)leaf.b -= 0.001f;
	}
}

/// <summary>
/// Intiates the max number of particles 
/// </summary>
/// <param name=""></param>
void GenerateParticles(void) {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		struct Velocity particleVelocity = { 0.0f, GLUE_RAND(1000) / 100 };
		int randX = GLUE_RAND(screenWidth--);
		int randY = GLUE_RAND(screenHeight--) + screenHeight;
		struct Location particleLocation = { randX, randY };
		struct Particle p = { particleLocation, snowColour, particleVelocity, particleVelocity.vy, ALIVE, i };

		GLUE_AddParticle(p);
	}
}

/// <summary>
/// Regenerates a specific particle
/// </summary>
/// <param name="p"></param>
void reGenParticle(struct Particle p) {
	struct Velocity particleVelocity = { 0.0f, GLUE_RAND(1000) / 100 };
	int randX = GLUE_RAND(screenWidth);
	int randY = GLUE_RAND(screenHeight) + screenHeight;
	struct Location particleLocation = { randX, randY };
	struct Particle newP = { particleLocation, snowColour, particleVelocity, particleVelocity.vy, ALIVE, p.id };

	GLUE_AddParticle(newP); //Replace a particle in the array with a new particle
}

/// <summary>
/// Draws the snowman Character
/// </summary>
/// <param name="snowmanLocation"></param>
void drawSnowman(struct Location snowmanLocation) {
	//scale = -2.8 log(x + 100) + 8.17966
	float scalar = -2.8 * log10(snowmanLocation.y + 100) + 8.17996; //Magic formula for scaling the snowman

	//Set the sizes of the body parts
	const int BODY_SIZE = 70 * scalar;
	const int HEAD_SIZE = 50 * scalar;
	const int NOSE_SIZE = 7 * scalar;
	const int EYE_SIZE = 5 * scalar;

	GLUE_CIRCLE(snowmanLocation, BODY_SIZE, bodyColour); //Body
	GLUE_CIRCLE(GLUE_OffsetLocation(&snowmanLocation, 0, 105 * scalar), HEAD_SIZE, bodyColour); //Head
	GLUE_CIRCLE(GLUE_OffsetLocation(&snowmanLocation, 0, 105 * scalar), NOSE_SIZE, noseColour); //Nose

	GLUE_CIRCLE(GLUE_OffsetLocation(&snowmanLocation, 15 + EYE_SIZE * scalar, 125 * scalar), EYE_SIZE, eyeColour); //Right Eye
	GLUE_CIRCLE(GLUE_OffsetLocation(&snowmanLocation, -15 - EYE_SIZE * scalar, 125 * scalar), EYE_SIZE, eyeColour); //Left Eye
}
