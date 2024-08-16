#pragma once
/*/
	GLUE - (open)GL - Utilities - Extra
		This aims to abstract functions in freeGLUT to allow for easier programming.

		LAST UPDATED 23/07/24
	Author - Robin Nowlan 2024
*/

#include <freeglut.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265359
#define SHAPE_RESOLUTION 30
#define COLOR_FALLOFF 0.5

/// <summary>
/// Pass colours more easily
/// </summary>
struct Colour {
	float r;
	float g;
	float b;
};

/// <summary>
/// Stores x and y positions
/// </summary>
struct Location {
	int x;
	int y;
};

/*
//Colour constants for easy use
const struct Colour RED = { 1.0f, 0.0f, 0.0f };
const struct Colour GREEN = { 0.0f, 1.0f, 0.0f };
const struct Colour BLUE = { 0.0f, 0.0f, 1.0f };
*/

/// <summary>
/// Draws a circle with a gradient to simulate 3D
/// </summary>
/// <param name="l">Location of the center of the circle</param>
/// <param name="r">Radius of the circle</param>
/// <param name="c">Colour of the circle</param>
void GLUE_CIRCLE(struct Location l, int r, struct Colour c);

/// <summary>
/// Returns a point between two locations
/// </summary>
/// <param name="l1">First location</param>
/// <param name="l2">Second Location</param>
/// <returns>Returns a mid-point location</returns>
struct Location GLUE_LineCenter(struct Location l1, struct Location l2);

/// <summary>
/// !!! WIP DO NOT USE !!! 
/// Returns the center of a polygon array
/// </summary>
/// <param name="locations">Array of all polygon points</param>
/// <returns>Returns a mid-point location</returns>
struct Location GLUE_PolygonCenter(struct Location locations[]);

/// <summary>
/// 
/// </summary>
/// <param name="first"></param>
/// <param name="second"></param>
/// <returns></returns>
float GLUE_LocationDistances(struct Location first, struct Location second);

/// <summary>
/// Displays text on the screen
/// </summary>
/// <param name="str">Text to be written</param>
/// <param name="l">Location of the bottom left corner of the text</param>
/// <param name="c">Color of the text</param>
void GLUE_Text(char str[], struct Location l, struct Colour c);
