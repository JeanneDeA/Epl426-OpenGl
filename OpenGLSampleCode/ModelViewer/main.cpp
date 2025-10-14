#include <stdio.h>           // Standard C/C++ Input-Output
#include <math.h>            // Math Functions
#include <windows.h>         // Standard Header For MSWindows Applications
#include <gl/glut.h>            // The GL Utility Toolkit (GLUT) Header
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// Global Variables
bool g_gamemode;				// GLUT GameMode ON/OFF
bool g_fullscreen;				// Fullscreen Mode ON/OFF (When g_gamemode Is OFF)
bool b_culling = false;			// Culling Enabled is Mandatory for this assignment do not change
float aspect = 1;

const float PI = 3.1415926535897932384626433832795028;
const float epsilon = 0.001;

//Function Prototypes
void render(void);
void initLights(void);
bool init(void);
void reshape(int w,int h);
void keyboard(unsigned char key, int x, int y);
void special_keys(int a_keys, int x, int y);


float angle = 0;


void initLights(void) {

	glEnable(GL_LIGHT0);	
	//glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);							   // Enable Lighting
	//glDisable(GL_LIGHTING);

	GLfloat lightpos[4] = { 0,0,10,0 };
	glLightfv(GL_LIGHT1, GL_POSITION, lightpos);
}


// Our GL Specific Initializations. Returns true On Success, false On Fail.
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

void positionCamera(){

	glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix

	glLoadIdentity();                // Reset The Projection Matrix
	gluPerspective(45.0f, aspect, 0.1, 100.0);
	gluLookAt(0.0f,0.0f,30.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f);
	//gluLookAt(10.0f,- 10.0f, 60.0f, 10.0f, -10.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	
	//camera transformations go here
	glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix

}

// Our Reshaping Handler (Required Even In Fullscreen-Only Modes)
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

// Our Keyboard Handler (Normal Keys)
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:        // When Escape Is Pressed...
			exit(0);    // Exit The Program
			break;      // Ready For Next Case
		default:
		break;
	}
	glutPostRedisplay();
}

// Our Keyboard Handler For Special Keys (Like Arrow Keys And Function Keys)
void special_keys(int a_keys, int x, int y)
{

	switch (a_keys) {
		case GLUT_KEY_F1:
			// We Can Switch Between Windowed Mode And Fullscreen Mode Only
			if (!g_gamemode) {
				g_fullscreen = !g_fullscreen;       // Toggle g_fullscreen Flag
				if (g_fullscreen) glutFullScreen(); // We Went In Fullscreen Mode
				else glutReshapeWindow(500, 500);   // We Went In Windowed Mode
			}
		break;
		case GLUT_KEY_UP:

			break;
		case GLUT_KEY_DOWN:

			break;
		case GLUT_KEY_LEFT:
			break;
		case GLUT_KEY_RIGHT:
			break;
		default:
			;
	}

	glutPostRedisplay();
}


// Main Function For Bringing It All Together.
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	// Do we have culling enabled?
	if (b_culling == true)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glLoadIdentity();

	glutSolidSphere(5, 20, 20);

    // Swap The Buffers To Make Our Rendering Visible
    glutSwapBuffers();
	glutPostRedisplay(); //animation
}