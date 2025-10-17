#include <stdio.h>							// Standard C/C++ Input-Object
#include <math.h>							// Math Functions
#include <windows.h>					    // Standard Header For MSWindows Applications
#include <gl/glut.h>						// The GL Utility Toolkit (GLUT) Header
#include <time.h>							// Time Functions
#include </../Users/Ioanna/Documents/Ucy/Epl426-OpenGl/OpenGLSampleCode/ModelViewer/Vector3.h>		// Vector3 Header

#define STB_IMAGE_IMPLEMENTATION			
#include "stb_image.h"						// STB Image Header


const float PI = 3.1415926535897932384626433832795028;
const float minIntensity = 0.2f;
const float maxIntensity = 2.0f;

// ------------ Global Variables -------------
bool b_culling = false;

bool g_gamemode;
bool g_fullscreen;
float aspect = 1;


bool movingPlanetFlag = true;
float planetRotationAngle = 0.0f;
float planetRotationSpeed = 30.0f;
static GLuint planetTextureID = 0;



bool movingPlaneFlag = true;
float planeOrbitAngle = 0.0f;
float planeOrbitRadius = 3.0f;
float planeSpeed = 1.0f;
float planeBaseSpeed = 50.0f;
bool planeLightOn = true;


float PropellerAngle = 0.0f;
float PropellerSpeed = 720.0f;

bool movingOrbitFlag = true;
float sunOrbitAngle = 0.0f;   
float orbitBaseSpeed = 15.0f;
float orbitSpeed = 0.5f;
float orbitRadius = 10.0f;
float sunLightIntensity = 5.0f;
static GLuint sunTextureID = 0;
static GLuint moonTextureID = 0;

// Time management variables
float lastTime = 0.0f;
float currentTime = 0.0f;
float deltaTime = 0.0f;
			
// Camera variables
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 30.0f;
float zoomLevel = 30.0f;


// ------------Function Prototypes-------------


//Helper functions
float Lerp(float min, float max, float param);
float clamp(float value, float min, float max);
float smoothstep(float edge0, float edge1, float x);


//Lighting functions
void initLights(void);
void initGeneralLight();
void initSunLight();
void initPlaneLight(void);

//Texture functions
void initTextures();
GLuint loadTexture(const char* filepath);

//Display coordination
bool init(void);
void render(void);
void reshape(int w, int h);
void updateTime();
void positionCamera();
void backgroundChange();

//Input handling functions
void keyboard(unsigned char key, int x, int y);
void special_keys(int a_keys, int x, int y);

//Sphere creation functions
void createSphere(float radius = 1.0f, int slices = 60, int stacks = 60, float r = 1.0f, float g = 0.0f, float b = 0.0f, float alpha = 1.0f);
void createTexturedSphere(float radius, int slices, int stacks, GLuint textureID);
void createStrechedSphere(float radius = 1.0f, int slices = 60, int stacks = 60, float r = 0.8f, float g = 0.8f, float b = 0.0f, float alpha = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.5f);

//Object creation functions
void createPlanet(float planetRadius = 2.0f, int slices = 60, int stacks = 60, float diskInner = 2.5f, float diskOuter = 3.0f, GLuint textureID = planetTextureID);
void createMoon(float moonRadius = 1.0f, int slices = 60, int stacks = 60, GLuint textureID = moonTextureID);
void createSun(float maxRadius = 5.0f, float decrement = 0.5f, GLuint textureID = sunTextureID);
void createPlane();


int main(int argc, char** argv)
{
	glutInit(&argc, argv);											// GLUT Initializtion
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE); 

	if (g_gamemode) {
		glutGameModeString("1024x768:32");            
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
			glutEnterGameMode();									// Enter Full Screen
		else
			g_gamemode = false;										// Cannot Enter Game Mode, Switch To Windowed
	}
	if (!g_gamemode) {
		glutInitWindowPosition(100, 100); 
		glutInitWindowSize(500, 500); 
		glutCreateWindow("EPL426 - Assignment1"); 
	}
	if (!init()) {                                   
		fprintf(stderr,"Initialization failed.");
		return -1;
	}

	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(render);                     
	glutReshapeFunc(reshape);                    
	glutKeyboardFunc(keyboard);                  
	glutSpecialFunc(special_keys);               
	glutIdleFunc(NULL);  

	glutMainLoop();                              
	return 0;
}


// ------------Function Implementations-------------


// Initialization function
bool init(void)
{
	glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping 
	glClearDepth(1.0f);											// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
	glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
	glShadeModel(GL_SMOOTH);									// Enable Smooth Shading

	initLights();
	initTextures();

	glEnable(GL_COLOR_MATERIAL);								// Enable Material Coloring
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	// Set the color tracking for both faces for both the ambient and diffuse components
	glEnable(GL_NORMALIZE);										// Enable automatic normalization of normals
	glFrontFace(GL_CCW);										//Counter Clock Wise definition of the front and back side of faces

	return true;
}

// Render function
void render(void)   
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateTime();

	if (b_culling == true)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glLoadIdentity();

	// Update lighting every frame
	positionCamera();
	initSunLight();
	if(planeLightOn) initPlaneLight();
	backgroundChange();


#pragma region Planet

	if (movingPlanetFlag) {
		planetRotationAngle += planetRotationSpeed * deltaTime;	// degrees per second
		if (planetRotationAngle > 360.0f) planetRotationAngle -= 360.0f;
	}

	glPushMatrix();
	glRotated(planetRotationAngle, 0, 1, 0);
	glColor3f(1.0f, 1.0f, 1.0f);  // Reset color to white

	createPlanet();
	glPopMatrix();

#pragma endregion

#pragma region Plane

	// Plane orbit
	if (movingPlaneFlag) {
		planeOrbitAngle += planeBaseSpeed * planeSpeed * deltaTime; // degrees per second
		if (planeOrbitAngle > 360.0f) planeOrbitAngle -= 360.0f;

		// Propeller rotation
		PropellerAngle += PropellerSpeed * planeSpeed * deltaTime;
		if (PropellerAngle > 360.0f) PropellerAngle -= 360.0f;
	}

	glPushMatrix();
	glRotated(planeOrbitAngle, 0, 0, 1);
	glTranslated(planeOrbitRadius, 0.0f, 0.0f);
	createPlane();
	glPopMatrix();

	#pragma endregion

#pragma region Orbit

	if (movingOrbitFlag) {
		sunOrbitAngle += orbitBaseSpeed * orbitSpeed * deltaTime; // degrees per second
		if (sunOrbitAngle > 360.0f) sunOrbitAngle -= 360.0f;
	}

	// Draw Sun
	glPushMatrix();
	glRotated(sunOrbitAngle, 0, 0, 1);
	glTranslatef(orbitRadius, 0.0f, 0.0f);
	glDisable(GL_LIGHTING);
	createSun();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// Draw Moon 
	glPushMatrix();
	glRotated(sunOrbitAngle + 180.0f, 0, 0, 1);
	glTranslatef(orbitRadius, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);  // Reset color to white
	glDisable(GL_LIGHTING);
	createMoon();
	glEnable(GL_LIGHTING);


	glPopMatrix();

#pragma endregion 

	//drawAxis();
	glutSwapBuffers();
	glutPostRedisplay();
}

//  Linear interpolation function
float Lerp(float min, float max, float param)
{
	return min + param * (max - min);
}

// Clamp function to restrict value within a range
float clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

// Smoothstep function for smooth transitions
float smoothstep(float edge0, float edge1, float x) {
	float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);		// Normalize x to [0, 1]
	return t * t * (3.0f - 2.0f * t);								// Smoothstep formula
}

//Initialise Lighting
void initLights(void) {

	initGeneralLight();
	initSunLight();
	initPlaneLight();

}

void initGeneralLight() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat lightPos[] = { 0.0f, 10.0f, 10.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	GLfloat diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

void initSunLight() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	//Determine suns current position on world cordinates
	float sunX = orbitRadius * cos(sunOrbitAngle * PI / 180.0f);
	float sunY = orbitRadius * sin(sunOrbitAngle * PI / 180.0f);
	float sunZ = 0.0f;

	GLfloat lightPos[] = { sunX, sunY, sunZ, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

	float timeFactor = (cos(sunOrbitAngle * PI / 180.0f) + 1) / 2;
	float sunLightIntensity = Lerp(minIntensity, maxIntensity, timeFactor);

	GLfloat lightDiffuse[] = { sunLightIntensity, sunLightIntensity * 0.9f, sunLightIntensity * 0.7f,1.0f };
	GLfloat lightAmbient[] = { 0.2f * sunLightIntensity,	0.2f * sunLightIntensity, 0.1f * sunLightIntensity,	1.0f };
	GLfloat lightSpecular[] = { 1.0f,1.0f,0.8f,1.0f };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

	// Attenuation settings for sun light fade with distance
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.001f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0001f);
}

void initPlaneLight() {

	if (!planeLightOn) {
		glDisable(GL_LIGHT2);
		return;
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);

	// Determine plane's current position in world coordinates
	float planeX = planeOrbitRadius * cosf(planeOrbitAngle * PI / 180.0f);
	float planeY = planeOrbitRadius * sinf(planeOrbitAngle * PI / 180.0f);
	float planeZ = 0.5f;

	GLfloat lightPos[] = { planeX, planeY, planeZ, 1.0f };
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos);

	// Direction from plane DOWN toward the origin 
	float dirX = -planeX;
	float dirY = -planeY;
	float dirZ = -planeZ;

	// Normalize the direction
	float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);		//Magnitude
	if (len > 1e-6f) {
		//Normalization
		dirX /= len;
		dirY /= len;
		dirZ /= len;
	}

	GLfloat spotDir[] = { dirX, dirY, dirZ };
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);

	//Narrow beam pointing down
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0f);    // Moderate beam width
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 30.0f);  // Concentrated beam

	GLfloat lightDiffuse[] = { 1.2f, 1.2f, 1.5f, 1.0f };
	GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.15f, 1.0f };
	GLfloat lightSpecular[] = { 1.5f, 1.5f, 2.0f, 1.0f };

	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);

	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.2f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.005f);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0001f);
}

// Initialize Textures
void initTextures() {

	sunTextureID = loadTexture("8k_sun.jpg");
	if (sunTextureID == 0) {
		printf("Failed to load sun texture! Using default color.\n");
	}

	moonTextureID = loadTexture("8k_moon.jpg");
	if (moonTextureID == 0) {
		printf("Failed to load moon texture! Using default color.\n");
	}

	planetTextureID = loadTexture("8k_jupiter.jpg");
	if (moonTextureID == 0) {
		printf("Failed to load planet texture! Using default color.\n");
	}
}

// Load texture from file and create OpenGL texture object
GLuint loadTexture(const char* filepath) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;		// Image dimensions and number of channels(RGB, RGBA, etc)

	// Flip texture vertically 
	stbi_set_flip_vertically_on_load(true);

	// Load image data
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);														//Make the texture active
		gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);		// Generate mipmaps (multiple resolutions of the texture)

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);									// Horizontal (U) coordinate wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);									// Vertical (V) coordinate wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);					//For smooth downscaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);								//For smooth upscaling

		printf("Texture loaded successfully: %s (%dx%d, %d channels)\n", filepath, width, height, nrChannels);
	}
	else {
		printf("Failed to load texture: %s\n", filepath);
	}

	stbi_image_free(data);		// Free image memory after uploading to GPU
	return textureID;
}

// Reshape function to handle window resizing
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	// Calculate The Aspect Ratio And Set The Clipping Volume
	if (h == 0) h = 1;
	aspect = (float)w / (float)h;
	positionCamera();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	initLights();
}

//Time tracking for smooth animation and frame-rate independent movement
void updateTime() {
	static bool firstTime = true;

	if (firstTime) {
		lastTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		firstTime = false;
		currentTime = lastTime;
		deltaTime = 0.0f;
		return;
	}

	currentTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	deltaTime = currentTime - lastTime;								//time passed since last frame in seconds
	lastTime = currentTime;

	if (deltaTime > 0.1f) deltaTime = 0.1f;							//clamp to max 0.1sec 
}

// Position the camera based on current camera variables
void positionCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);		// Set up a perspective projection
	gluLookAt(cameraX, cameraY, cameraZ, cameraX, cameraY, 0.0f, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Change background color based on sun position (day-night cycle)
void backgroundChange() {

	Vector3 dayColor = Vector3(0.3647f, 0.8f, 0.9098f);
	Vector3 nightColor = Vector3(0.0078f, 0.0196f, 0.1098f);

	float timeFactor = (cos(sunOrbitAngle * PI / 180.0f) + 1) / 2;

	float colorX = Lerp(nightColor[0], dayColor[0], timeFactor);
	float colorY = Lerp(nightColor[1], dayColor[1], timeFactor);
	float colorZ = Lerp(nightColor[2], dayColor[2], timeFactor);


	glClearColor(colorX, colorY, colorZ, 1.0f);
}

// Keyboard input handling
void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 27: // Escape
		exit(0);
		break;

	case 't': // Increase plane speed 
		planeSpeed += 0.5f;
		if (planeSpeed > 10.0f) {
			planeSpeed = 10.0f;
			printf("Max plane speed reached\n");
		}
		else {
			printf("Plane speed: %.1f\n", planeSpeed);
		}
		break;

	case 'y': // Decrease plane speed 
		planeSpeed -= 0.5f;
		if (planeSpeed < 0.5f) {
			planeSpeed = 0.5f;
			printf("Min plane speed reached\n");
		}
		else {
			printf("Plane speed: %.1f\n", planeSpeed);
		}
		break;

	case 'o': // Increase orbit speed 
		orbitSpeed += 0.5f;
		if (orbitSpeed > 10.0f) {
			orbitSpeed = 10.0f;
			printf("Max orbit speed reached\n");
		}
		else {
			printf("Orbit speed: %.1f\n", orbitSpeed);
		}
		break;

	case 'p': // Decrease orbit speed 
		orbitSpeed -= 0.5f;
		if (orbitSpeed < 0.5f) {
			orbitSpeed = 0.5f;
			printf("Min orbit speed reached\n");
		}
		else {
			printf("Orbit speed: %.1f\n", orbitSpeed);
		}
		break;

	case 'r': // Reset all speeds to default
		planeSpeed = 1.0f;
		orbitSpeed = 0.5f;
		printf("Speeds reset to default\n");
		break;

	case 'm': // Toggle planet rotation
		movingPlanetFlag = !movingPlanetFlag;
		printf("Planet rotation: %s\n", movingPlanetFlag ? "ON" : "OFF");
		break;

	case 'l':
		planeLightOn = !planeLightOn;
		if (planeLightOn) {
			glEnable(GL_LIGHT2);
			printf("Plane light: ON\n");
		}
		else {
			glDisable(GL_LIGHT2);
			printf("Plane light: OFF\n");
		}
		break;

	case 'c': // Reset camera position
		cameraX = 0.0f;
		cameraY = 0.0f;
		zoomLevel = 30.0f;
		cameraZ = zoomLevel;
		break;

	case '+': // Zoom in
	case '=':
		printf("Zooming in\n");
		zoomLevel -= 1.0f;
		if (zoomLevel < 5.0f) zoomLevel = 5.0f;
		cameraZ = zoomLevel;
		break;

	case '-': // Zoom out
	case '_':
		zoomLevel += 1.0f;
		if (zoomLevel > 80.0f) zoomLevel = 80.0f;
		cameraZ = zoomLevel;
		break;

	case ' ': // Toggle all animations
		movingPlanetFlag = !movingPlanetFlag;
		movingPlaneFlag = !movingPlaneFlag;
		movingOrbitFlag = !movingOrbitFlag;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// Special keys input handling
void special_keys(int a_keys, int x, int y)
{
	float moveSpeed = 0.5f;

	switch (a_keys) {
	case GLUT_KEY_UP:		// Move camera up
		cameraY += moveSpeed;
		break;
	case GLUT_KEY_DOWN:		// Move camera down
		cameraY -= moveSpeed;
		break;
	case GLUT_KEY_LEFT:		//Move camera left
		cameraX -= moveSpeed;
		break;
	case GLUT_KEY_RIGHT:	// Move camera right
		cameraX += moveSpeed;
		break;

	case GLUT_KEY_F1:		// Toggle fullscreen
		if (!g_gamemode) {
			g_fullscreen = !g_fullscreen;
			if (g_fullscreen) glutFullScreen();
			else glutReshapeWindow(500, 500);
		}
		break;
	default:
		break;

	}
	positionCamera();
	glutPostRedisplay();
}

//Create a colored sphere
void createSphere(float radius, int slices, int stacks, float r, float g, float b, float alpha)
{
	glPushMatrix();
	glColor4f(r, g, b, alpha);
	GLUquadric* sphere = gluNewQuadric();
	gluSphere(sphere, radius, slices, stacks);
	gluDeleteQuadric(sphere);
	glPopMatrix();
}

//Create a textured sphere
void createTexturedSphere(float radius, int slices, int stacks, GLuint textureID) {

	if (textureID != 0) {
		glEnable(GL_TEXTURE_2D);							// Enable texturing
		glBindTexture(GL_TEXTURE_2D, textureID);			// Select the texture

		//Material properties
		GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
		GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
		GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		GLfloat shininess[] = { 20.0f };

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	//Texture modulates with material colors

		GLUquadric* sphere = gluNewQuadric();
		gluQuadricTexture(sphere, GL_TRUE);								// Enable texture coordinates
		gluQuadricNormals(sphere, GLU_SMOOTH);							// Smooth normals for lighting

		gluSphere(sphere, radius, slices, stacks);
		gluDeleteQuadric(sphere);

		glDisable(GL_TEXTURE_2D);
	}
	else {
		// Fallback
		GLfloat ambient[] = { 0.3f, 0.3f, 0.1f, 1.0f };
		GLfloat diffuse[] = { 0.8f, 0.8f, 0.2f, 1.0f };
		GLfloat specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		GLfloat shininess[] = { 32.0f };

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		createSphere(radius, slices, stacks, 0.8f, 0.8f, 0.2f, 1.0f);
	}
}

//Create a stretched colored sphere
void createStrechedSphere(float radius, int slices, int stacks, float r, float g, float b, float alpha, float scaleX, float scaleY, float scaleZ) {
	glPushMatrix();
	glColor4f(r, g, b, alpha);
	glScalef(scaleX, scaleY, scaleZ);
	GLUquadric* sphere = gluNewQuadric();
	gluSphere(sphere, radius, slices, stacks);
	gluDeleteQuadric(sphere);
	glPopMatrix();
}

// Create a planet with a ring
void createPlanet(float planetRadius, int slices, int stacks, float diskInner, float diskOuter, GLuint textureID)
{
	glPushMatrix();

	// Draw the planet
	glDisable(GL_BLEND);					//Disable blending for opaque planet
	glDepthMask(GL_TRUE);

	glRotated(90, 1, 0, 0);					// Rotate planet so texture aligns properly
	createTexturedSphere(planetRadius, slices, stacks, textureID);

	glPopMatrix();

	// Draw the ring
	glEnable(GL_BLEND);						// Enable blending for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);				// Make ring visible from both sides

	GLfloat ringAmbient[] = { 0.4f, 0.3f, 0.1f, 0.5f };
	GLfloat ringDiffuse[] = { 0.8f, 0.5f, 0.0f, 0.5f };
	GLfloat ringSpecular[] = { 0.5f, 0.5f, 0.5f, 0.5f };
	GLfloat shininess[] = { 32.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ringAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ringDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ringSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glColor4f(0.8f, 0.5f, 0.0f, 0.5f);
	glRotated(70, 1, 0, 0);					// Tilt the ring

	GLUquadric* ring = gluNewQuadric();
	gluDisk(ring, diskInner, diskOuter, slices, stacks);
	gluDeleteQuadric(ring);

	if (b_culling) {
		glEnable(GL_CULL_FACE);
	}
	glDisable(GL_BLEND);

}

// Create a moon with texture
void createMoon(float moonRadius, int slices, int stacks, static GLuint textureID)
{
	createTexturedSphere(moonRadius, slices, stacks, textureID);
}

//void createSun(float maxRadius, float decrement, GLuint textureID) {
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDepthMask(GL_FALSE);
//
//	createSphere(maxRadius, 60, 60, 1.0f, 0.9f, 0.0f, 0.15f);
//	createSphere(maxRadius - decrement, 60, 60, 1.0f, 0.8f, 0.0f, 0.25f);
//	createSphere(maxRadius - (decrement * 2), 60, 60, 1.0f, 0.7f, 0.0f, 0.4f);
//
//	createTexturedSphere(maxRadius - (decrement * 3), 60, 60, textureID);
//
//	glDepthMask(GL_TRUE);
//	glDisable(GL_BLEND);
//}

// Create a sun with layered spheres and smooth alpha transitions
void createSun(float maxRadius, float decrement, GLuint textureID) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	float timeFactor = (cos(sunOrbitAngle * PI / 180.0f) + 1) / 2;		//Finds a value between 0 and 1 based on sunOrbitAngle
	float intensity = Lerp(minIntensity, maxIntensity, timeFactor);		// Maps timeFactor to intensity range

	// transition ranges with overlap 
	float layer3_start = 0.2f;	 //Inner
	float layer3_end = 1.1f;

	float layer2_start = 0.65f;  //Middle
	float layer2_end = 1.55f;

	float layer1_start = 1.1f;	 //Outer 
	float layer1_end = 2.0f;

	// Calculate smooth alpha values with overlap
	float alpha3 = smoothstep(layer3_start, layer3_end, intensity);
	float alpha2 = smoothstep(layer2_start, layer2_end, intensity);
	float alpha1 = smoothstep(layer1_start, layer1_end, intensity);


	createTexturedSphere(maxRadius - (decrement * 3), 60, 60, textureID);

	// Create spheres with smooth alpha transitions
	if (alpha3 > 0.01f) {
		createSphere(maxRadius - (decrement * 2), 60, 60, 1.0f, 0.7f, 0.0f, 0.4f * alpha3);
	}
	if (alpha2 > 0.01f) {
		createSphere(maxRadius - decrement, 60, 60, 1.0f, 0.8f, 0.0f, 0.25f * alpha2);
	}
	if (alpha1 > 0.01f) {
		createSphere(maxRadius, 60, 60, 1.0f, 0.9f, 0.0f, 0.15f * alpha1);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

// Create a simple plane model
void createPlane() {

	GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHT2);		// Check if light 2 (plane light) is enabled
	glDisable(GL_LIGHT2);

	// Main body:
	glPushMatrix();
	createStrechedSphere(0.3f, 60, 60, 0.2f, 0.2f, 1.0f, 1.0f, 1.0f, 3.0f, 1.0f);
	glPopMatrix();

	// Hood
	glPushMatrix();
	glTranslatef(0.3f, 0.0f, 0.0f);
	createStrechedSphere(0.3f, 60, 60, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.2f);
	glPopMatrix();


	//Propeller 1
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f);
	createSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(PropellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 0.8f, 3.0f, 0.5f, 1.0f);
	glPopMatrix();

	//Propeller 2
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f);
	createSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(PropellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 0.8f, 3.0f, 0.5f, 1.0f);
	glPopMatrix();

	//Propeller Front
	glPushMatrix();
	glTranslatef(0.0f, 0.9, 0.0f);
	createSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(PropellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 0.8f, 5.0f, 0.5f, 1.0f);
	glPopMatrix();

	if (lightingWasEnabled) {
		glEnable(GL_LIGHT2);
	}

}