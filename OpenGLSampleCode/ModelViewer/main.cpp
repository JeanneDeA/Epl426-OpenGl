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

bool movingPlaneFlag = true;
float anglePlane = 0.0f;
float planeSpeed = 1.0f;
float planeMove = 0;



bool movingOrbitFlag = true;
float orbitRadius = 8.0f;
float orbitAngle = 0.0f;
float orbitMove = 0;
float orbitSpeed = 0.5f;

float sunLightIntensity = 1.0f;


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
void initGeneralLight();
void initSunLight();



void initLights(void) {

	initGeneralLight();
	initSunLight();

}
 
void initGeneralLight() {
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	GLfloat lightPos[] = { 0.0f, 0.0f, 30.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	/*GLfloat globalAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);*/
}
 
void initSunLight() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);


	float orbitAngle = anglePlanet * orbitSpeed;


	float sunX = orbitRadius * cos(orbitAngle * PI / 180.0f);
	float sunY = orbitRadius * sin(orbitAngle * PI / 180.0f);
	float sunZ = 0.0f;

	GLfloat lightPos[] = { sunX, sunY, sunZ, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

	GLfloat matSpec[] = { 0.1, 0.1,0.1,1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	GLfloat shininess[] = { 64 };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	GLfloat lightOn[4] = { 1,1,1,1 };
	GLfloat lightAmbientOn[4] = { 0.1,0.1,0.1,1 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientOn);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOn);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightOn);


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
	

	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);                               //Counter Clock Wise definition of the front and back side of faces
	glCullFace(GL_BACK);                               //Hide the back side
	
	return true;
}

void positionCamera() {

		glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix

		glLoadIdentity();                // Reset The Projection Matrix
		gluPerspective(45.0f, aspect, 0.1, 100.0);
		gluLookAt(0.0f, 0.0f, 30.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		//gluLookAt(10.0f,- 10.0f, 60.0f, 10.0f, -10.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		//camera transformations go here
		glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix

	
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
		case 't':
			planeSpeed += 0.1f;
			if (planeSpeed > 5.0f) {
				printf("Max speed reached\n");
				planeSpeed = 1.0f;
			}
			break;
		case 'y':
			planeSpeed -= 0.1f;
			if (planeSpeed < 0.0f) {
				printf("Min speed reached\n");
				planeSpeed = 1.0f;
			}
			break;
		case 'o':
			orbitSpeed += 0.1f;
			if (orbitSpeed > 3.0f) {
				printf("Max orbit speed reached\n");
				orbitSpeed = 0.5f;
			}
			break;

		case 'p':
			orbitSpeed -= 0.1f;
			if (orbitSpeed < 0.0f) {
				printf("Min orbit speed reached\n");
				orbitSpeed = 0.5f;
			}
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


	//Propeller 1
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f);
	createSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(planeMove, 0.0f, 0.0f, 1.0f);
	createStrechedSphere(0.3f, 20, 20,		0.5f, 0.5f, 0.5f, 0.8f,		3.0f, 0.5f, 1.0f);
	glPopMatrix();	

	//Propeller 2
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f);
	createSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(planeMove, 0.0f, 0.0f, 1.0f);
	createStrechedSphere(0.3f, 20, 20,		0.5f, 0.5f, 0.5f, 0.8f,		3.0f, 0.5f, 1.0f);
	glPopMatrix();

	//Propeller Front
	glPushMatrix();
	glTranslatef(0.0f, 0.5f *3.0, 0.0f);
	createSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 1.0f);
	glRotatef(planeMove, 0.0f, 1.0f, 0.0f);
	createStrechedSphere(0.3f, 20, 20, 0.5f, 0.5f, 0.5f, 0.8f, 5.0f, 0.5f, 1.0f);
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

	glutSetCursor(GLUT_CURSOR_NONE);
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
	if (movingOrbitFlag) {
		orbitAngle += 0.1f;	
		orbitMove = orbitAngle * orbitSpeed;
	}

	glPushMatrix();
	glRotated(orbitMove, 0, 0, 1);
	glTranslatef(orbitRadius, 0.0f, 0.0f);
	glDisable(GL_LIGHTING);
	createSun();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	glPushMatrix();
	glRotated(orbitMove +180.0f, 0 , 0, 1);
	glTranslatef(orbitRadius, 0.0f, 0.0f);
	createMoon();
	glPopMatrix();


#pragma endregion 

#pragma region Plane
	float planeOrbitRadius = 4.0f;

	if (movingPlaneFlag) {
		anglePlane += 0.1f;
		planeMove = anglePlane * planeSpeed;
	}


	glPushMatrix();
	glRotated(planeMove, 0, 0, 1);
	glTranslated(planeOrbitRadius, 0.0f, 0.0f);
	createPlane();
	glPopMatrix();


#pragma endregion

	drawAxis(); // Draw axes at the end

    glutSwapBuffers();
	glutPostRedisplay(); 
}

