#include "GLUE.h"


struct Colour FallOffMultiplyer = { 0.7f, 0.6f, 0.3f };

void GLUE_COLOUR(struct Colour c) {
	glColor3f(c.r, c.g, c.b); //Sets the GL colour using the colour struct
}

void GLUE_CIRCLE(struct Location l, int r, struct Colour c) {
	glBegin(GL_TRIANGLE_FAN); //Set drawing mode to triangle fan
	glColor3f(c.r, c.g, c.b); //Set center colour
	glVertex2f(l.x, l.y); //Draw center point

	//Calculate the colour of the outter points using the falloff conststant
	glColor3f(c.r - (COLOR_FALLOFF * FallOffMultiplyer.r), c.g - (COLOR_FALLOFF * FallOffMultiplyer.g), c.b - (COLOR_FALLOFF * FallOffMultiplyer.b));

	//Loop for the number of points that make up the circle
	for (int i = 0; i <= SHAPE_RESOLUTION; i++) {
		double angle = (2 * PI / SHAPE_RESOLUTION) * i; //Calculate the current angle

		float px = r * cos(angle); //Convert polar point to cartisian x
		float py = r * sin(angle); //Convert polar point to cartisian y

		glVertex2f(px + l.x, py + l.y); //Set vertex point
	}
	glEnd(); //Finish drawing the circle
}

struct Location GLUE_LineCenter(struct Location firstLocation, struct Location secondLocation) {
	struct Location middle = { (firstLocation.x + secondLocation.x) / 2, (firstLocation.y + secondLocation.y) / 2, };
	return middle;
}

struct Location GLUE_PolygonCenter(struct Location locations[]) {
	int xSum = 0;
	int ySum = 0;

	int count = sizeof(locations) / sizeof(struct Location);

	for (int i = 0; i < count; i++) {
		xSum += locations[i].x;

		ySum += locations[i].y;
	}

	struct Location middle = { xSum / count, ySum / count };
	return middle;
}

float GLUE_LocationDistances(struct Location first, struct Location second) {
	return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2));
}

void GLUE_Text(char str[], struct Location l, struct Colour c) {
	glColor3f(c.r, c.g, c.b); //Set text colour
	for (int i = 0; i < strlen(str); i++) { //Loop through each character
		glRasterPos2f(l.x, l.y); //Set position of the character
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]); //Draw the character

		l.x += 10;//Shift the character 10px to the right
	}
}

char* GLUE_toStringf(double num) {
	char* str = (char*)malloc(50 * sizeof(char)); //Assign memory location for the converted value

	//Ensure that the string is not null
	if (str == NULL) return NULL; // Allocation failed, return NULL

	sprintf_s(str, 50, "%f", num); //Convert float to string using printf

	return str; //Return the new string value
}

char* GLUE_toStringi(double num) {
	char* str = (char*)malloc(50 * sizeof(char)); //Assign memory location for the converted value

	//Ensure that the string is not null
	if (str == NULL) return NULL; // Allocation failed, return NULL

	sprintf_s(str, 50, "%.0f", num); //Convert integer to string using printf

	return str; //Return the new string value
}

char* GLUE_combineString(char str1[], char str2[]) {
	// Calculate the length of the first string
	int len1 = 0;
	while (str1[len1] != '\0') {
		len1++;
	}

	// Calculate the length of the second string
	int len2 = 0;
	while (str2[len2] != '\0') {
		len2++;
	}

	// Allocate memory for the combined string (including null terminator)
	char* combinedStr = (char*)malloc((len1 + len2 + 1) * sizeof(char));

	// Copy characters from the first string
	for (int i = 0; i < len1; i++) {
		combinedStr[i] = str1[i];
	}

	// Copy characters from the second string
	for (int i = 0; i < len2; i++) {
		combinedStr[len1 + i] = str2[i];
	}

	// Add the null terminator
	combinedStr[len1 + len2] = '\0';

	return combinedStr;
}

char* GLUE_stringValuef(char title[], double num) {
	return GLUE_combineString(title, GLUE_toStringf(num));
}

char* GLUE_stringValuei(char title[], double num) {
	return GLUE_combineString(title, GLUE_toStringi(num));
}

int GLUE_RAND(int maxVal) {
	return (int)(((double)(maxVal + 1) / RAND_MAX) * rand()); //Clamp rand value betwen 0 and randMax
}

int GLUE_RAND_MIN_MAX(int minVal, int maxVal) {
	return (int)(((double)rand()) * (maxVal - minVal) / (RAND_MAX)+minVal);
}

//Redraw the particle
void GLUE_DrawParticle(struct Particle* p) {
	GLUE_CIRCLE(p->location, p->size, p->colour);
}

//Move the particle acoording to its velocity
void GLUE_MoveParticle(struct Particle* p) {
	p->location.x -= p->velocity.vx;
	p->location.y -= p->velocity.vy;
}

void GLUE_AddParticle(struct Particle p) {
	int hash = p.id % MAX_PARTICLES; //Use a hash function to set its array location
	GLUE_Particles[hash] = p;
}

void GLUE_MakeParticle(struct Location location,
	struct Colour colour,
	struct Velocity velocity,
	float size,
	enum ParticleState state,
	int id) {

	//Create a new particle
	struct Particle p = { location, colour, velocity,size, state, id };

	int hash = p.id % MAX_PARTICLES; //Use a hash function to set its array location
	GLUE_Particles[hash] = p; //Set the array position depending on hash location
}

//Function to offset a previously set location
struct Location GLUE_OffsetLocation(struct Location* loc, int x, int y) {
	struct Location newLocation = { loc->x + x, loc->y + y };

	return newLocation;
}

void GLUE_Line(struct Location start, struct Location end, struct Colour startColour, struct Colour endColour, int thickness) {
	glBegin(GL_QUADS);


	// Calculate direction vector
	float dx = end.x - start.x;
	float dy = end.y - start.y;
	float length = sqrt(dx * dx + dy * dy);

	// Calculate perpendicular vector (normal)
	float nx = -dy / length * thickness / 2;
	float ny = dx / length * thickness / 2;

	GLUE_COLOUR(startColour); //Set the line colour
	// Draw the thick line as a quadrilateral
	glVertex2f(start.x + nx, start.y + ny);
	glVertex2f(start.x - nx, start.y - ny);

	GLUE_COLOUR(endColour); //Set the line colour
	glVertex2f(end.x - nx, end.y - ny);
	glVertex2f(end.x + nx, end.y + ny);

	glEnd();
}


void GLUE_Tree(struct Location baseLocation, struct Colour baseColour, struct Colour tipColour, int depth) {
	int maxSize = 500; //Max size of tree

	//Begin branch recursion, 
	GLUE_TreeBranch(baseLocation, baseColour, tipColour, maxSize - baseLocation.y, depth, 0.01 * (300 - baseLocation.y), (90) * (PI / 180));
}

static void GLUE_TreeBranch(struct Location startPoint, struct Colour baseColour, struct Colour tipColour, float length, int depth, int itter, float angle) {
	//Ensure that recursion has a limit
	if (itter <= depth) {
		//Move recursivly here
		length *= TREE_BRANCH_SCALAR; //+ GLUE_RAND_MIN_MAX(-1* TREE_VARIABILITY, TREE_VARIABILITY); //Shrink the length of the branch
		length += itter;

		struct Colour sectionColour; //Create a new colour for this specific branch

		//Calculat the colour of the branch section
		sectionColour.r = baseColour.r + (tipColour.r - baseColour.r) / (depth - itter);
		sectionColour.g = baseColour.g + (tipColour.g - baseColour.g) / (depth - itter);
		sectionColour.b = baseColour.b + (tipColour.b - baseColour.b) / (depth - itter);

		//Calculate the end point of the branch to draw to
		struct Location endPoint = GLUE_OffsetLocation(&startPoint, length * cos(angle), length * sin(angle));

		//Draw the branch line with reletive thickness
		GLUE_Line(startPoint, endPoint, baseColour, sectionColour, TREE_THICKNESS_SCALAR * (depth - itter));
		GLUE_TreeBranch(endPoint, sectionColour, tipColour, length, depth, itter + 1, angle + (TREE_ANGLE * (PI / 180))); //Recursivly draw Right branch
		GLUE_TreeBranch(endPoint, sectionColour, tipColour, length, depth, itter + 1, angle - (TREE_ANGLE * (PI / 180))); //Recursivly draw Left Branch
	}
	//Otherwise the tree has reached max depth
}

int getParticleCount(int screenHeight) {
	int count = 0;
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (GLUE_Particles[i].state == ALIVE) count++;
	}
	return count;
}