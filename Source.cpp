#include<stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;


// Global Variables
float deltaAngle = 0.0, deltaMove = 0, deltaMoves = 0;
float lx = 0, lz = -1;
float x = 0, z = 12.5, angle = 0;
float car_position = -1;
float dangle = 0, dx = 0,dy = 0;
float ax = 0, ay = 0, az = 0, anglex = 0, angley = 0;
GLuint grassTexture;
GLuint roadTextue;
GLuint buildingTexture1;
GLuint buildingTexture2;
GLuint buildingTexture3;
GLuint buildingTexture4;
// ----------------


// ---- Colors ---------
#define Black 0,0,0
#define White 1,1,1
#define Blue 0,0,1
#define Green 0,1,0
#define Gray 0.7,0.7,0.7
#define Pink 1,0.5,0.5
#define Red 1,0,0
#define Sky 0.5, 0.8, 1
// ----------------------


// Function Prototypes
void display();
void initGL();
void drawLand();
void drawGrass();
void drawBuildings();
void drawRoads();
void CAR();
void Doors();
void reshape( int, int);
void specialKeyPress( int, int, int);
void specialReleaseKey( int, int, int);
void keyspressed( unsigned char, int, int);
void releaseKey( int, int, int);
void computePositionFB( float);
void computePositionLR(float);
void computeDir( float );
GLuint loadBMP(const char *);
// --------------------


// ------ Main ---------
int main(int arg, char ** args)
{
	glutInit(&arg, args);
	initGL();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutSpecialFunc(specialKeyPress);
	glutSpecialUpFunc(specialReleaseKey);
	glutKeyboardFunc(keyspressed);
	glutMainLoop();
	return 0;
}
// ---------------------

// ------- INIT ---------
void initGL()
{
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 450);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Project 2");
	glClearColor(Sky, 0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); // allow depth buffering

	// Fixed high ram usage by loading texture once
	// https://stackoverflow.com/questions/11217121/how-to-manage-memory-with-texture-in-opengl
	grassTexture = loadBMP("textures/grass.bmp");
	roadTextue = loadBMP("textures/road.bmp");
	buildingTexture1 = loadBMP("textures/building1.bmp");
	buildingTexture2 = loadBMP("textures/building2.bmp");
	buildingTexture3 = loadBMP("textures/building3.bmp");
	buildingTexture4 = loadBMP("textures/building4.bmp");

	// glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
	// GLfloat diffuseLightColor0[] = {0.5, 0.5, 0.5, 0};
    // GLfloat specularLightColor0[] = {0.5, 0.5, 0.5, 1};
    // GLfloat lightPos0[] = {10, 10, 5, 0};
    // // glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightColor0);
    // glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor0);
    // glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
}

void display()
{
	if (deltaMove)
		computePositionFB(deltaMove);
	if (deltaMoves)
		computePositionLR(deltaMoves);
	if (deltaAngle)
		computeDir(deltaAngle);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	 // Set camera
	 // Parameters:
	 // (eye[x,y,z] , center[x,y,z], up[x,y,z])
	gluLookAt(x, 1, z, x + lx, 1, z + lz, 0, 1, 0);

	drawLand();

	glPushMatrix();
	glColor3f(Red);
	glScalef(0.4, 0.4, 0.7);
	glTranslatef((GLfloat)car_position, 1.3, 6);
	CAR();
	glPopMatrix();


	glPushMatrix();
	glColor3f(Pink);
	glScalef(0.3, 0.3, 0.6);
	glTranslatef(15, 1.3, 12);
	glRotatef(180, 0, 1, 0);
	CAR();
	glPopMatrix();




	glutSwapBuffers();
}

void reshape(int w, int h)
{
	if (h == 0)
		h = 1;
	float ratio = w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0, ratio, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

// -------------- Texture Loader -------------
GLuint loadBMP(const char * imagepath)
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;
	unsigned char * data;

	FILE * file = fopen(imagepath,"rb");
	if (!file)
	{
		printf("Error opening file.\n");
		return 0;
	}
	if ( fread(header, 1, 54, file)!=54 )
	{
	    printf("Not a BMP file\n");
	    return false;
	}
	if ( header[0]!='B' || header[1]!='M' )
	{
	    printf("Not a BMP file\n");
	    return 0;
	}
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);
	if (dataPos==0)      dataPos=54;
	data = new unsigned char [imageSize];

	fread(data,1,imageSize,file);

	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(data);
	return textureID;
}
// -------------------------



// ------ Keyboard ---------
void specialKeyPress(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT: deltaMoves = 0.9; break;
		case GLUT_KEY_RIGHT: deltaMoves = -0.9; break;
		case GLUT_KEY_DOWN: deltaMove = -0.9; break;
		case GLUT_KEY_UP: deltaMove = 0.9; break;
		case GLUT_KEY_PAGE_UP: deltaAngle = -0.01; break;
		case GLUT_KEY_PAGE_DOWN: deltaAngle = 0.01;;  break;
		case GLUT_KEY_F1: dangle = 45;dx = 0.5;dy = 1.5; break;
		case GLUT_KEY_F2: dangle = 0;dx = 0;dy = 0; break;
	}
}
void keyspressed(unsigned char key, int x, int y){

	switch (key)
	{
	case 'A': car_position += 0.5; break;
	case 'a': car_position += 0.5; break;
	case 'D':car_position -= 0.5; break;
	case 'd':car_position -= 0.5; break;
	default:
		break;
	}
}
void specialReleaseKey(int key, int x, int y)
{
	switch (key)
	{

	case GLUT_KEY_LEFT: deltaMoves = 0; break;
	case GLUT_KEY_RIGHT: deltaMoves = 0; break;
	case GLUT_KEY_DOWN: deltaMove = 0; break;
	case GLUT_KEY_UP: deltaMove = 0; break;
	case GLUT_KEY_PAGE_UP: deltaAngle = 0; break;
	case GLUT_KEY_PAGE_DOWN: deltaAngle = 0;;  break;
	}
}
// -----------------------------



// -------- Camera Movement -------
void computePositionFB(float deltaMove)
{
	x += deltaMove * lx * 0.1;
	z += deltaMove * lz * 0.1;
}
void computePositionLR(float deltaMove)
{
	x += deltaMove * lz * 0.1;
	z += deltaMove * lx * 0.1;
}
void computeDir(float deltaAngle)
{
	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}
// ------------------------------- //






// ------------- CAR -------------
void CAR()
{
	///////////////////////////////////////top
	GLdouble equ[4] = { 0,1,0,1 };
	glPushMatrix();
	glPushMatrix();
	glClipPlane(GL_CLIP_PLANE0, equ);
	glEnable(GL_CLIP_PLANE0);
	glColor3f(Black);
	glScalef(4, 1, 1);

	glutSolidSphere(.5, 100, 100);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
	////////////////////////bottom back
	GLdouble equ1[4] = { 0,-1,0,0 };
	GLdouble equ2[4] = { -1,0,0,0 };
	glPushMatrix();
	glColor3f(Red);
	glClipPlane(GL_CLIP_PLANE0, equ1);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ2);
	glEnable(GL_CLIP_PLANE1);
	// glColor3f(0, 0, 0);
	glScalef(6, 1.5, 1);
	glutSolidCube(1);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
	glPopMatrix();
	///////////////////////////////////bottom front
	GLdouble equ3[4] = { 0,1,0,0 };
	GLdouble equ4[4] = { 1,0,0,0 };
	glPushMatrix();
	glColor3f(Red);
	glTranslatef(1.5, -0.7, 0);
	glClipPlane(GL_CLIP_PLANE0, equ3);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ4);
	glEnable(GL_CLIP_PLANE1);
	// glColor3f(0, 0, 0);
	glScalef(3, 1, .6);
	glutSolidSphere(0.9, 8, 5);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	GLdouble equ5[4] = { 0,-1,0,0 };
	GLdouble equ6[4] = { 1,0,0,0 };
	glPushMatrix();
	glColor3f(Black);
	glClipPlane(GL_CLIP_PLANE0, equ5);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ6);
	glEnable(GL_CLIP_PLANE1);
	// glColor3f(0, 0, 0);
	glScalef(3, 1.5, 1);
	glutSolidCube(1);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	////////////////////////////////////////////////////tires
	////////////////////////////////right side tires
	glPushMatrix();

	glTranslatef(2, -0.7, .5);
	glColor3f(Black);
	glScalef(1, 1, .2);
	glutSolidSphere(.5, 10, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2, -0.7, .5);
	glScalef(1, 1, .2);
	glutSolidSphere(.5, 10, 5);
	glPopMatrix();
	/////////////////////////////left side tires
	glPushMatrix();
	glTranslatef(2, -0.7, -.5);
	// glColor3f(1, 1, 1);
	glScalef(1, 1, .2);
	glutSolidSphere(.5, 10, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2, -0.7, -.5);
	// glColor3f(1, 1, 1);
	glScalef(1, 1, .2);
	glutSolidSphere(.5, 10, 5);
	glPopMatrix();
	/////////////////////////////////////////////////// doors function call
	glPushMatrix();
	glTranslatef(dx,dy,0);
	glRotatef(dangle,0,0,-1);
	Doors();
	glPopMatrix();
	////////////////////////////////////////////////end
	glPopMatrix();
}

void Doors() {
	/////////////////////////////////////////////////////////doors
	///////////////////////////////////////left doors
	GLdouble equ7[4] = { 1,0,0,0 };
	GLdouble equ8[4] = { 0,-1,0,0 };
	glPushMatrix();
	glTranslatef(0, 0, .5);
	glClipPlane(GL_CLIP_PLANE0, equ7);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ8);
	glEnable(GL_CLIP_PLANE1);
	glColor3f(Red);
	glScalef(3, 1.5, .1);
	glutSolidCube(1);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
	//////////////////////////mirror part
	GLdouble equ9[4] = { 0,1,0,0 };
	GLdouble equ10[4] = { 1,0,0,0 };
	GLdouble equ11[5] = { 0,0,1,0 };
	glPushMatrix();
	glTranslatef(0, 0, .05);
	glClipPlane(GL_CLIP_PLANE0, equ9);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ10);
	glEnable(GL_CLIP_PLANE1);
	glClipPlane(GL_CLIP_PLANE2, equ11);
	glEnable(GL_CLIP_PLANE2);
	glColor3f(1, 0, 0);
	glScalef(4, 1, 1);
	glutSolidSphere(.5, 100, 100);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
	///////////////////////////////////////right doors
	GLdouble equ12[4] = { 1,0,0,0 };
	GLdouble equ13[4] = { 0,-1,0,0 };
	glPushMatrix();
	glTranslatef(0, 0, -.5);
	glClipPlane(GL_CLIP_PLANE0, equ12);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ13);
	glEnable(GL_CLIP_PLANE1);
	glColor3f(1, 0, 0);
	glScalef(3, 1.5, .1);
	glutSolidCube(1);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
	//////////////////////////mirror part
	GLdouble equ14[4] = { 0,1,0,0 };
	GLdouble equ15[4] = { 1,0,0,0 };
	GLdouble equ16[5] = { 0,0,-1,0 };
	glPushMatrix();
	glTranslatef(0, 0, -.05);
	glClipPlane(GL_CLIP_PLANE0, equ14);
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE1, equ15);
	glEnable(GL_CLIP_PLANE1);
	glClipPlane(GL_CLIP_PLANE2, equ16);
	glEnable(GL_CLIP_PLANE2);
	glColor3f(1, 0, 0);
	glScalef(4, 1, 1);
	glutSolidSphere(.5, 100, 100);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
}
// -----------------------------------------------//




// ------------------------- World ------------------------ //

void drawLand()
{
	drawGrass();
	drawRoads();
	drawBuildings();
}

void drawGrass()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glBegin(GL_QUADS);
		glColor3f(0, 0.9, 0);
		glTexCoord2f(-100, -100); glVertex3f(-100, 0, -100);
		glTexCoord2f(-100, 100); glVertex3f(-100, 0, 100);
		glTexCoord2f(100, 100); glVertex3f(100, 0, 100);
		glTexCoord2f(100, -100); glVertex3f(100, 0, -100);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawRoads()
{
	// Road 1 (Bottom/Near)

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, roadTextue);

		glBegin(GL_POLYGON);
			glColor3f(Gray);
			glTexCoord2f(-20, 8);	glVertex3f(-20, 0.01, 8);
			glTexCoord2f(-20, 6);	glVertex3f(-20, 0.01, 6);
			glTexCoord2f(20, 6);	glVertex3f( 20, 0.01, 6);
			glTexCoord2f(20, 8); glVertex3f( 20, 0.01, 8);
		glEnd();

	// Road 2 (Far)

		glBegin(GL_POLYGON);
			glTexCoord2f(-20, 5); glVertex3f(-20, 0.01, 5);
			glTexCoord2f(-20, 3); glVertex3f(-20, 0.01, 3);
			glTexCoord2f(20, 3);  glVertex3f( 20, 0.01, 3);
			glTexCoord2f(20, 5);  glVertex3f( 20, 0.01, 5);
		glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawBuildings()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, buildingTexture1);

		glPushMatrix();
		glTranslatef(-4, 2, -7);
		glScalef(1, 5, 1);
		glutSolidCube(1);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, buildingTexture2);

		glPushMatrix();
			glColor3f(Gray);
			glTranslatef(1, 2, -7);
			glScalef(1.5, 9, 1.5);
			glutSolidCube(1);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, buildingTexture3);

		glPushMatrix();
			glColor3f(Blue);
			glTranslatef(4, 1, -7);
			glScalef(0.6, 4, 0.6);
			glutSolidCube(1);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, buildingTexture1);

		glPushMatrix();
			glColor3f(Pink);
			glTranslatef(2, 1, -8);
			glScalef(0.7, 4, 0.7);
			glutSolidCube(1);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, buildingTexture3);

		glPushMatrix();
			glColor3f(Gray);
			glTranslatef(-6, 2, -10);
			glScalef(1.5, 10, 1.5);
			glutSolidCube(1);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);


	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);


}






