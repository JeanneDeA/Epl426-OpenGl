#include <stdio.h>           // Standard C/C++ Input-Output
#include <math.h>            // Math Functions
#include <windows.h>         // Standard Header For MSWindows Applications
#include <gl/glut.h>            // The GL Utility Toolkit (GLUT) Header
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// Global Variables
bool b_culling = false;

bool movingPlanetFlag = true;
float anglePlanet = 0.0f;

bool movingPlaneFlag = false;
float planeSpeed = 0.7f;


//===============================================================================================

bool g_gamemode;				
bool g_fullscreen;				

float aspect = 1;

const float PI = 3.1415926535897932384626433832795028;
const float epsilon = 0.001;
float angle = 0;

void render(void);
void initLights(void);
bool init(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special_keys(int a_keys, int x, int y);
void positionCamera(void);

void createSphere(float radius = 1.0f, int slices = 60, int stacks = 60, float r = 1.0f, float g = 0.0f, float b = 0.0f, float alpha = 1.0f);
void createStrechedSphere(float radius = 1.0f, int slices = 60, int stacks = 60, float r = 0.8f, float g = 0.8f, float b = 0.0f, float alpha = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.5f);

void createPlanet(float planetRadius = 2.0f, int slices = 60, int stacks = 60, float diskInner = 2.5f, float diskOuter = 3.0f);
void createMoon(float moonRadius = 0.5f, int slices = 60, int stacks = 60);
void createSun(float maxRadius = 4.0f, float decrement = 0.8f);

void drawAxis(float length = 3.0f);


void initLights(void) {
    glEnable(GL_LIGHTING);

    // Enable and configure GL_LIGHT0
    glEnable(GL_LIGHT0);

    // Set light position (directional light from above and in front)
    GLfloat lightPos[] = { 0.0f, 0.0f, 30.0f, 1.0f }; 
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Set diffuse and ambient color for the light
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Optionally, set global ambient light
    GLfloat globalAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

bool init(void)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);             // Pixel Storage Mode To Byte Alignment
	glEnable(GL_TEXTURE_2D);                           // Enable Texture Mapping 
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);			   // Gray Background (CHANGED)
    glClearDepth(1.0f);								   // Depth Buffer Setup
    glDepthFunc(GL_LEQUAL);							   // The Type Of Depth Testing To Do
    glEnable(GL_DEPTH_TEST);						   // Enables Depth Testing
    glShadeModel(GL_SMOOTH);						   // Enable Smooth Shading
	initLights();
	glEnable(GL_COLOR_MATERIAL);					   // Enable Material Coloring
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Hint for nice perspective interpolation
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	// Set the color tracking for both faces for both the ambient and diffuse components
	
	//// Set Specular
	//GLfloat matSpec[] = { 0.1, 0.1,0.1,1 };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	//GLfloat shininess[] = { 64 };
	//glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);                               //Counter Clock Wise definition of the front and back side of faces
	glCullFace(GL_BACK);                               //Hide the back side
	
	return true;
}

float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 30.0f; // Default Z
float cameraStep = 1.0f;

void positionCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1, 100.0);
    gluLookAt(cameraX, cameraY, cameraZ,   // Camera position
              0.0f, 0.0f, 0.0f,           // Look at the origin
              0.0f, 1.0f, 0.0f);          // Up vector
    glMatrixMode(GL_MODELVIEW);
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

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: // Escape
            exit(0);
            break;
        case 'w': // Move camera forward (decrease Z)
            cameraZ -= cameraStep;
            break;
        case 's': // Move camera backward (increase Z)
            cameraZ += cameraStep;
            break;
        case 'a': // Move camera left (decrease X)
            cameraX -= cameraStep;
            break;
        case 'd': // Move camera right (increase X)
            cameraX += cameraStep;
            break;
        case 'q': // Move camera up (increase Y)
            cameraY += cameraStep;
            break;
        case 'e': // Move camera down (decrease Y)
            cameraY -= cameraStep;
            break;
        default:
            break;
    }
    positionCamera();
    glutPostRedisplay();
}

void special_keys(int a_keys, int x, int y)
{
    switch (a_keys) {
        case GLUT_KEY_UP:
            cameraY += cameraStep;
            break;
        case GLUT_KEY_DOWN:
            cameraY -= cameraStep;
            break;
        case GLUT_KEY_LEFT:
            cameraX -= cameraStep;
            break;
        case GLUT_KEY_RIGHT:
            cameraX += cameraStep;
            break;
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

void createPlanet(float planetRadius , int slices , int stacks , float diskInner, float diskOuter)
{
    glPushMatrix();

    // Draw the planet using createSphere
    createSphere(planetRadius, slices, stacks, 0.8f, 0.8f, 0.0f, 0.8f);

    // Draw the ring
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.8f, 0.5f, 0.0f, 0.5f);
    glRotated(70, 1, 0, 0);

    GLUquadric* ring = gluNewQuadric();
    gluDisk(ring, diskInner, diskOuter, slices, stacks);
    gluDeleteQuadric(ring);
    glDisable(GL_BLEND);

    glPopMatrix();
}

void createMoon(float moonRadius , int slices, int stacks )
{
    createSphere(moonRadius, slices, stacks, 0.5f, 0.5f, 0.5f, 1.0f);
}

void createSun(float maxRadius, float decrement) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // Disable depth writing

    createSphere(maxRadius, 60, 60, 1.0f, 0.9f, 0.0f, 0.15f);
    createSphere(maxRadius-decrement, 60, 60, 1.0f, 0.8f, 0.0f, 0.25f);
    createSphere(maxRadius - (decrement *2), 60, 60, 1.0f, 0.7f, 0.0f, 0.4f);
    createSphere(maxRadius - (decrement *3), 60, 60, 1.0f, 0.6f, 0.0f, 1.0f);

	glDepthMask(GLU_TRUE); // Enable depth writing
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
    createStrechedSphere( 0.5f, 60, 60,		0.2f, 0.2f, 1.0f, 1.0f,		1.0f, 3.0f, 1.0f);
	glPopMatrix();

	// Hood
	glPushMatrix();
	glTranslatef(0.5f, 0.0f, 0.0f);
	createStrechedSphere(0.5f, 60, 60,		1.0f, 1.0f, 1.0f, 1.0f,		1.0f, 2.0f, 1.2f);
	glPopMatrix();

	float propellerAngle = anglePlanet * planeSpeed;

	//Propeller 1
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f);
	createSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(propellerAngle, 0.0f, 0.0f, 1.0f);
	createStrechedSphere(0.3f, 20, 20,		0.5f, 0.5f, 0.5f, 0.8f,		3.0f, 0.5f, 1.0f);
	glPopMatrix();	

	//Propeller 1
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f);
	createSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(propellerAngle, 0.0f, 0.0f, 1.0f);
	createStrechedSphere(0.3f, 20, 20,		0.5f, 0.5f, 0.5f, 0.8f,		3.0f, 0.5f, 1.0f);
	glPopMatrix();

	//Propeller Front
	glPushMatrix();
	glTranslatef(0.0f, 0.5f *3.0, 0.0f);
	createSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(propellerAngle, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 0.8f, 3.0f, 0.5f, 1.0f);
	glPopMatrix();
   
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);                           // GLUT Initializtion
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE); // (CHANGED)|

	if (g_gamemode) {
		glutGameModeString("1024x768:32");            // Select 1024x768 In 32bpp Mode
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
			glutEnterGameMode();                     // Enter Full Screen
		else
			g_gamemode = false;                     // Cannot Enter Game Mode, Switch To Windowed
	}
	if (!g_gamemode) {
		glutInitWindowPosition(100, 100); // Window Position
		glutInitWindowSize(500, 500); // Window Size If We Start In Windowed Mode
		glutCreateWindow("EPL426"); // Window Title
	}
	if (!init()) {                                   // Our Initialization
		fprintf(stderr,"Initialization failed.");
		return -1;
	}

	glutDisplayFunc(render);                     // Register The Display Function
	glutReshapeFunc(reshape);                    // Register The Reshape Handler
	glutKeyboardFunc(keyboard);                  // Register The Keyboard Handler
	glutSpecialFunc(special_keys);               // Register Special Keys Handler
	
	glutIdleFunc(NULL);                        	 // We Do Rendering In Idle Time
	glutMainLoop();                              // Go To GLUT Main Loop
	return 0;
}

// Our Rendering Is Done Here
void render(void)   
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	if (b_culling == true)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glLoadIdentity();


#pragma region Planet

	if (movingPlanetFlag) {
		anglePlanet += 0.1f;
		glPushMatrix();
		glRotated(anglePlanet, 0, 1, 0);
		createPlanet();
 	   	glPopMatrix();
	}
	else {
		createPlanet();
	}
	
#pragma endregion

#pragma region Orbit
	float orbitRadius = 8.0f;
	float orbitSpeed = 0.5f;
	float orbitAngle = anglePlanet * orbitSpeed;

	glPushMatrix();
	glRotated(orbitAngle, 0, 0, 1);
	glTranslatef(orbitRadius, 0.0f, 0.0f);
	createMoon();
	glPopMatrix();

	glPushMatrix();
	glRotated(orbitAngle +180.0f, 0 , 0, 1);
	glTranslatef(orbitRadius, 0.0f, 0.0f);
	createSun();
	glPopMatrix();


#pragma endregion 

#pragma region Plane
	float planeOrbitRadius = 4.0f;
	float planeAngle = anglePlanet * planeSpeed;

	glPushMatrix();
	glRotated(planeAngle, 0, 0, 1);
	glTranslated(planeOrbitRadius, 0.0f, 0.0f);
	createPlane();
	glPopMatrix();


#pragma endregion

	//drawAxis(); // Draw axes at the end

    glutSwapBuffers();
	glutPostRedisplay(); 
}

