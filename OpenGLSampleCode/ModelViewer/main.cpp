#include <stdio.h>           // Standard C/C++ Input-Object
#include <math.h>            // Math Functions
#include <windows.h>         // Standard Header For MSWindows Applications
#include <gl/glut.h>            // The GL Utility Toolkit (GLUT) Header
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


struct vector3d
{
	float X, Y, Z;

	inline vector3d(void) {}
	inline vector3d(const float x, const float y, const float z)
	{
		X = x; Y = y; Z = z;
	}

	inline vector3d operator + (const vector3d& A) const
	{
		return vector3d(X + A.X, Y + A.Y, Z + A.Z);
	}

	inline vector3d operator + (const float A) const
	{
		return vector3d(X + A, Y + A, Z + A);
	}

	inline float Dot(const vector3d& A) const
	{
		return A.X * X + A.Y * Y + A.Z * Z;
	}
};

float Lerp(float a, float b, float w)
{
	return a + w * (b - a);
}

const float PI = 3.1415926535897932384626433832795028;


// Global Variables
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
			



// Function Prototypes
void render(void);
void reshape(int w, int h);
bool init(void);

void initAnimation();
void initLights(void);
void initGeneralLight();
void initSunLight();
void initPlaneLight(void);

void updateTime();

void keyboard(unsigned char key, int x, int y);
void special_keys(int a_keys, int x, int y);

void positionCamera(void);
GLuint loadTexture(const char* filepath);

void createSphere(float radius = 1.0f, int slices = 60, int stacks = 60, float r = 1.0f, float g = 0.0f, float b = 0.0f, float alpha = 1.0f);
void createStrechedSphere(float radius = 1.0f, int slices = 60, int stacks = 60, float r = 0.8f, float g = 0.8f, float b = 0.0f, float alpha = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.5f);

void createPlanet(float planetRadius = 2.0f, int slices = 60, int stacks = 60, float diskInner = 2.5f, float diskOuter = 3.0f,GLuint textureID = planetTextureID);
void createMoon(float moonRadius = 1.0f, int slices = 60, int stacks = 60, GLuint textureID = moonTextureID);
void createSun(float maxRadius = 5.0f, float decrement = 0.5f, GLuint textureID = sunTextureID);

void drawAxis(float length = 3.0f);


void updateTime() {
	static bool firstTime = true;

	if (firstTime) {
		lastTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		firstTime = false;
	}

	currentTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	deltaTime = currentTime - lastTime;							//time passed since last frame in seconds
	lastTime = currentTime;

	if (deltaTime > 0.1f) deltaTime = 0.1f;
}

// Initialize animation timing so that the first frame has a valid time reference
void initAnimation() {
	lastTime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;			
	currentTime = lastTime;
}

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

	float sunX = orbitRadius * cos(sunOrbitAngle * PI / 180.0f);
	float sunY = orbitRadius * sin(sunOrbitAngle * PI / 180.0f);
	float sunZ = 0.0f;

	GLfloat lightPos[] = { sunX, sunY, sunZ, 1.0f };	
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

	const float minIntensity = 0.2f;
	const float maxIntensity = 2.0f;

	float timeFactor = (cos(sunOrbitAngle * PI / 180.0f) + 1) / 2;

	float sunLightIntensity = Lerp(minIntensity, maxIntensity, timeFactor) ;

	GLfloat lightDiffuse[] = { sunLightIntensity, sunLightIntensity * 0.9f, sunLightIntensity * 0.7f,1.0f };		
	GLfloat lightAmbient[] = {0.2f * sunLightIntensity,	0.2f * sunLightIntensity, 0.1f * sunLightIntensity,	1.0f};
	GLfloat lightSpecular[] = {1.0f,1.0f,0.8f,1.0f};

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

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

	float planeX = planeOrbitRadius * cosf(planeOrbitAngle * PI / 180.0f);
	float planeY = planeOrbitRadius * sinf(planeOrbitAngle * PI / 180.0f);
	float planeZ = 0.5f;  // Plane is above the orbital plane

	GLfloat lightPos[] = { planeX, planeY, planeZ, 1.0f };
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos);

	// Direction from plane DOWN toward the origin (planet)
	float dirX = -planeX;  // Toward center X
	float dirY = -planeY;  // Toward center Y  
	float dirZ = -planeZ;  // DOWN toward Z=0 (orbital plane)

	// Normalize the direction
	float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
	if (len > 1e-6f) {
		dirX /= len;
		dirY /= len;
		dirZ /= len;
	}

	GLfloat spotDir[] = { dirX, dirY, dirZ };
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);

	// Flashlight parameters - narrow beam pointing down
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0f);    // Moderate beam width
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 30.0f);  // Concentrated beam

	// Bright flashlight colors
	GLfloat lightDiffuse[] = { 1.2f, 1.2f, 1.5f, 1.0f };  // Bright blue-white
	GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.15f, 1.0f };  // Low ambient
	GLfloat lightSpecular[] = { 1.5f, 1.5f, 2.0f, 1.0f };  // Bright specular

	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);

	// Reduced attenuation so the flashlight reaches the planet
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.2f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.005f);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0001f);
}

GLuint loadTexture(const char* filepath) {
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;

	// Flip texture vertically (OpenGL expects 0,0 at bottom-left)
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

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		printf("Texture loaded successfully: %s (%dx%d, %d channels)\n", filepath, width, height, nrChannels);
	}
	else {
		printf("Failed to load texture: %s\n", filepath);
	}

	stbi_image_free(data);
	return textureID;
}

void positionCamera() {

		glMatrixMode(GL_PROJECTION);     

		glLoadIdentity();                // Reset The Projection Matrix
		gluPerspective(45.0f, aspect, 0.1, 100.0);
		gluLookAt(0.0f, 0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix

	
}

void backgroundChange() {

	vector3d dayColor = vector3d(0.3647f, 0.8f, 0.9098f);    // Sky blue
	vector3d nightColor = vector3d(0.0078f, 0.0196f, 0.1098f); // Dark blue

	float timeFactor = (cos(sunOrbitAngle * PI / 180.0f)+1)/2; 

	float colorX = Lerp(nightColor.X, dayColor.X, timeFactor);
	float colorY = Lerp(nightColor.Y, dayColor.Y, timeFactor);
	float colorZ = Lerp(nightColor.Z, dayColor.Z, timeFactor);
	

	glClearColor(colorX, colorY, colorZ,1.0f);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	// Calculate The Aspect Ratio And Set The Clipping Volume
	if (h == 0) h = 1;
	aspect = (float)w/(float)h;
	positionCamera();
	glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix
	initLights();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // Escape
		exit(0);
		break;
	case 't': // Increase plane speed smoothly
		planeSpeed += 0.5f;
		if (planeSpeed > 10.0f) {
			planeSpeed = 10.0f;
			printf("Max plane speed reached\n");
		}
		else {
			printf("Plane speed: %.1f\n", planeSpeed);
		}
		break;
	case 'y': // Decrease plane speed smoothly
		planeSpeed -= 0.5f;
		if (planeSpeed < 0.5f) {
			planeSpeed = 0.5f;
			printf("Min plane speed reached\n");
		}
		else {
			printf("Plane speed: %.1f\n", planeSpeed);
		}
		break;
	case 'o': // Increase orbit speed smoothly
		orbitSpeed += 0.5f;
		if (orbitSpeed > 10.0f) {
			orbitSpeed = 10.0f;
			printf("Max orbit speed reached\n");
		}
		else {
			printf("Orbit speed: %.1f\n", orbitSpeed);
		}
		break;
	case 'p': // Decrease orbit speed smoothly
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

void special_keys(int a_keys, int x, int y)
{
    switch (a_keys) {
      
        case GLUT_KEY_F1:
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

void drawAxis(float length) {
	glDisable(GL_LIGHTING); // Draw axes without lighting for clear colors

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// X axis - Red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(length, 0.0f, 0.0f);

	// Y axis - Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, length, 0.0f);

	// Z axis - Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, length);
	glEnd();

	glLineWidth(1.0f);
	glEnable(GL_LIGHTING); // Restore lighting
}

void createSphere(float radius, int slices , int stacks,float r, float g , float b , float alpha)
{
	glPushMatrix();
	glColor4f(r, g, b, alpha);
	GLUquadric* sphere = gluNewQuadric();
	gluSphere(sphere, radius, slices, stacks);
	gluDeleteQuadric(sphere);
	glPopMatrix();
}

void createTexturedSphere(float radius, int slices, int stacks, GLuint textureID) {
	if (textureID != 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set material properties that work well with textures AND lighting
		GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };   // Higher ambient for better visibility
		GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };   // Good diffuse reflection
		GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // Some specular for highlights
		GLfloat shininess[] = { 20.0f };

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		// Make sure texture modulates with material colors
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		GLUquadric* sphere = gluNewQuadric();
		gluQuadricTexture(sphere, GL_TRUE);
		gluQuadricNormals(sphere, GLU_SMOOTH);
		gluSphere(sphere, radius, slices, stacks);
		gluDeleteQuadric(sphere);

		glDisable(GL_TEXTURE_2D);
	}
	else {
		// Fallback with good lighting properties
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

void createPlanet(float planetRadius, int slices, int stacks, float diskInner, float diskOuter, GLuint textureID)
{
	glPushMatrix();

	// Draw the planet - SOLID, no blending
	glDisable(GL_BLEND);  // Planet should be solid
	glDepthMask(GL_TRUE); // Enable depth writing

	glRotated(90, 1, 0, 0); // Rotate planet so texture aligns properly
	createTexturedSphere(planetRadius, slices, stacks, textureID);

	glPopMatrix();

	// Draw the ring
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);  // Make ring visible from both sides

	GLfloat ringAmbient[] = { 0.4f, 0.3f, 0.1f, 0.5f };
	GLfloat ringDiffuse[] = { 0.8f, 0.5f, 0.0f, 0.5f };
	GLfloat ringSpecular[] = { 0.5f, 0.5f, 0.5f, 0.5f };
	GLfloat shininess[] = { 32.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ringAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ringDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ringSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glColor4f(0.8f, 0.5f, 0.0f, 0.5f);
	glRotated(70, 1, 0, 0);

	GLUquadric* ring = gluNewQuadric();
	gluDisk(ring, diskInner, diskOuter, slices, stacks);
	gluDeleteQuadric(ring);

	if (b_culling) {
		glEnable(GL_CULL_FACE);
	}
	glDisable(GL_BLEND);

}

void createMoon(float moonRadius , int slices, int stacks, static GLuint textureID)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	createTexturedSphere(moonRadius, slices, stacks, textureID);
	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void createSun(float maxRadius, float decrement, GLuint textureID) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	createSphere(maxRadius, 60, 60, 1.0f, 0.9f, 0.0f, 0.15f);
	createSphere(maxRadius - decrement, 60, 60, 1.0f, 0.8f, 0.0f, 0.25f);
	createSphere(maxRadius - (decrement * 2), 60, 60, 1.0f, 0.7f, 0.0f, 0.4f);

	createTexturedSphere(maxRadius - (decrement * 3), 60, 60, textureID);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void createStrechedSphere( float radius, int slices,int stacks,float r,float g,float b,float alpha,float scaleX,float scaleY,float scaleZ) {
    glPushMatrix();
    glColor4f(r, g, b, alpha);
    glScalef(scaleX, scaleY, scaleZ); 
    GLUquadric* sphere = gluNewQuadric();
    gluSphere(sphere, radius, slices, stacks);
    gluDeleteQuadric(sphere);
    glPopMatrix();
}

void createPlane() {

    // Main body:
	glPushMatrix();
    createStrechedSphere( 0.3f, 60, 60,		0.2f, 0.2f, 1.0f, 1.0f,		1.0f, 3.0f, 1.0f);
	glPopMatrix();

	// Hood
	glPushMatrix();
	glTranslatef(0.3f, 0.0f, 0.0f);
	createStrechedSphere(0.3f, 60, 60,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 2.0f, 1.2f);
	glPopMatrix();


	//Propeller 1
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f);
	createSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(PropellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.1f, 20, 20,		0.5f, 0.5f, 0.5f, 0.8f,		3.0f, 0.5f, 1.0f);
	glPopMatrix();	

	//Propeller 2
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f);
	createSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(PropellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.1f, 20, 20,		0.5f, 0.5f, 0.5f, 0.8f,		3.0f, 0.5f, 1.0f);
	glPopMatrix();

	//Propeller Front
	glPushMatrix();
	glTranslatef(0.0f, 0.9, 0.0f);
	createSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(PropellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.1f, 20, 20, 0.5f, 0.5f, 0.5f, 0.8f, 5.0f, 0.5f, 1.0f);
	glPopMatrix();
   
}

bool init(void)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);             // Pixel Storage Mode To Byte Alignment
	glEnable(GL_TEXTURE_2D);                           // Enable Texture Mapping 
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);			   // Gray Background (CHANGED)
	glClearDepth(1.0f);								   // Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);							   // The Type Of Depth Testing To Do
	glEnable(GL_DEPTH_TEST);						   // Enables Depth Testing
	glShadeModel(GL_SMOOTH);						   // Enable Smooth Shading

	initLights();

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





	glEnable(GL_COLOR_MATERIAL);					   // Enable Material Coloring
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Hint for nice perspective interpolation
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	// Set the color tracking for both faces for both the ambient and diffuse components


	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);                               //Counter Clock Wise definition of the front and back side of faces
	glCullFace(GL_BACK);                               //Hide the back side

	return true;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);											// GLUT Initializtion
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE); 

	if (g_gamemode) {
		glutGameModeString("1024x768:32");            
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
			glutEnterGameMode();                     // Enter Full Screen
		else
			g_gamemode = false;                     // Cannot Enter Game Mode, Switch To Windowed
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

	initAnimation();								// Initialize animation timing


	glutSetCursor(GLUT_CURSOR_NONE);
	glutDisplayFunc(render);                     
	glutReshapeFunc(reshape);                    
	glutKeyboardFunc(keyboard);                  
	glutSpecialFunc(special_keys);               
	
	glutIdleFunc(NULL);                        	 
	glutMainLoop();                              
	return 0;
}

// Our Rendering Is Done Here
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

