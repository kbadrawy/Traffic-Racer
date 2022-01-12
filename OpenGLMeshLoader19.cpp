#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include<time.h>
#include <iostream>
#include<string> 

int WIDTH = 1280;
int HEIGHT = 720;
bool win = false;
GLuint tex;
GLuint tex2;
char title[] = "3D Model Loader Sample";
static long font = (long)GLUT_BITMAP_TIMES_ROMAN_24;
// 3D Projection Options
GLdouble fovy =45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;
bool lost = false;
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
int justHit = 0;
bool obstacle1 = true;
bool obstacle2 = true;
bool obstacle3 = true;
bool obstacle4 = true;
bool obstacle5 = true;
bool obstacle6 = true;
bool obstacle7 = true;
bool obstacle8 = true;

bool powered = false;
float score = 0;
int lives = 3;
bool power1 = true;
bool power2 = true;
int lane = 1; // can be 0 for left, 1 for middle, 2 for right
float playerZ = 0;
int speed = 0; //-1 slowing, 0 neutral, 1 speeding
float playerX = 0;  
float playerX2 = 0;
int passX = 0;
int passY = 0;
int currCamera = 0; // 0 third person , 1 first
int level = 0;
class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;
	Camera(float eyeX = 0, float eyeY = 6, float eyeZ = 15,
		float centerX = 0, float centerY = 5, float centerZ = 10,
		float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

void passM(int x, int y)//passive motion function takes 2 parameters the x and y positions of the mouse
						//it allows the user to make use of the mouse motion without clicking on the mouse buttons
{
	passX = x;//it moves the red square center to the mouse location.
	passY = y;
	glutPostRedisplay();
}

// Model Variables
Model_3DS model_house;
Model_3DS car; 
Model_3DS car2; 
Model_3DS gasPowerup; 
Model_3DS blockade; 
Model_3DS candyPowerup; 
Model_3DS cone; 
Model_3DS deadTree; 
Model_3DS regularTree; 
Model_3DS ghost; 
Model_3DS pumpkin; 
Model_3DS skull; 
Model_3DS fence; 
Model_3DS road; 
Model_3DS ground;
Model_3DS model_tree;
Model_3DS halloweenHouse;
Model_3DS cityHouse;
Model_3DS targetHouse;
// Textures
GLTexture tex_ground; 
GLTexture tex_ground2;
GLTexture text;
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	GLfloat lightIntensity[] = { 0.3, 0.3, 0.3, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightIntensity); // Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
	glEnable(GL_LIGHT0);

}
//=======================================================================
// Material Configuration Function
//======================================================================
// unsolved
void InitMaterial()
{
	// Enable Material Tracking
	//glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void push() {
	glPushMatrix();
}
void pop() {
	glPopMatrix();
}

//malhash lazma
void drawWall(double thickness) {
	glPushMatrix();
	glColor3d(1, 1, 1);
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}
//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	camera.look();
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

void collision() {
	if (lane == 1) {
		if (playerZ - 5 <= -50 && playerZ + 10 >= -50 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle1) {
					PlaySound(TEXT("Sounds/cone.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle1) {
					PlaySound(TEXT("Sounds/cartoonSplat.wav"), NULL, SND_ASYNC);
				}
				obstacle1 = false;
				justHit = 100;
				lives--;
			}
		}
		if (playerZ - 5 <= -350 && playerZ + 10 >= -350 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle2) {
					PlaySound(TEXT("Sounds/carCrash.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle2) {
					PlaySound(TEXT("Sounds/ghost.wav"), NULL, SND_ASYNC);
				}
				obstacle2 = false;
				lives--; justHit = 100;
			}
		}
		if (playerZ - 5 <= -450 && playerZ + 10 >= -450 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {

				if (level == 0 && obstacle3) {
					PlaySound(TEXT("Sounds/cone.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle3) {
					PlaySound(TEXT("Sounds/cartoonSplat.wav"), NULL, SND_ASYNC);
				}
				obstacle3 = false;

				lives--; justHit = 100;
			}
		}
		if (playerZ - 5 <= -750 && playerZ + 10 >= -750 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle4) {
					PlaySound(TEXT("Sounds/carCrash.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle4) {
					PlaySound(TEXT("Sounds/ghost.wav"), NULL, SND_ASYNC);
				}
				obstacle4 = false;
				lives--; justHit = 100;
			}
		}


	}
	if (lane == 0) {
		if (playerZ - 5 <= -200 && playerZ + 10 >= -200) {
			power1 = false;

			if (!powered) {
				PlaySound(TEXT("Sounds/carRev.wav"), NULL, SND_ASYNC);
			}
			powered = true;
		}
		if (playerZ - 5 <= -650 && playerZ + 10 >= -650 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle5) {
					PlaySound(TEXT("Sounds/cone.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle5) {
					PlaySound(TEXT("Sounds/cartoonSplat.wav"), NULL, SND_ASYNC);
				}
				obstacle5 = false;
				lives--; justHit = 100;
			}
		}
		if (playerZ - 5 <= -250 && playerZ + 10 >= -250 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle6) {
					PlaySound(TEXT("Sounds/carCrash.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle6) {
					PlaySound(TEXT("Sounds/ghost.wav"), NULL, SND_ASYNC);
				}
				obstacle6 = false;
				lives--; justHit = 100;
			}
		}
		//powerups


	}
	if (lane == 2) {
		if (playerZ - 5 <= -500 && playerZ + 10 >= -500) {
			power2 = false;
			if (!powered) {
				PlaySound(TEXT("Sounds/carRev.wav"), NULL, SND_ASYNC);
			}
			powered = true;
		}
		if (playerZ - 5 <= -150 && playerZ + 10 >= -150 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle7) {
					PlaySound(TEXT("Sounds/cone.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle7) {
					PlaySound(TEXT("Sounds/cartoonSplat.wav"), NULL, SND_ASYNC);
				}
				obstacle7 = false;
				lives--; justHit = 100;
			}
		}
		if (playerZ - 5 <= -550 && playerZ + 10 >= -550 && justHit == 0) {
			if (powered) {
				justHit = 100;
				powered = false;
			}
			else {
				if (level == 0 && obstacle8) {
					PlaySound(TEXT("Sounds/carCrash.wav"), NULL, SND_ASYNC);
				}
				else if (level == 1 && obstacle8) {
					PlaySound(TEXT("Sounds/ghost.wav"), NULL, SND_ASYNC);
				}
				obstacle8 = false;
				lives--; justHit = 100;
			}
		}
		//powerups


	}
}
//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround() // ard level 2
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	//glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	//glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void RenderGround2()// ard level 1
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void drawCube(float posX, float posY, float posZ, float scX, float scY, float scZ, bool draw, int type) {
	glPushMatrix();
	if (type == 1) {
		cone.visible = draw;
		glTranslated(posX, posY - 1, posZ);
		glScaled(0.03, 0.03, 0.03);

		cone.Draw();
	}
	else {
		blockade.visible = draw;
		glTranslated(posX, posY - 1, posZ);
		glScaled(1, 3, 1);
		blockade.Draw();
	}
	glPopMatrix();
}
void drawSphere(float posX, float posY, float posZ, bool draw) {
	glPushMatrix();
	gasPowerup.visible = draw;
	glTranslated(posX, posY, posZ);
	glRotated(-90, 0, 1, 0);
	glScaled(0.035, 0.035, 0.035);
	gasPowerup.Draw();
	glPopMatrix();
}

void drawCube2(float posX, float posY, float posZ, float scX, float scY, float scZ, bool draw, int type) {
	glPushMatrix();
	if (type == 1) {
		pumpkin.visible = draw;
		glTranslated(posX, posY, posZ);
		glScaled(0.02, 0.02, 0.02);
		pumpkin.Draw();
	}
	else {
		ghost.visible = draw;
		glTranslated(posX, posY, posZ);
		glTranslated(0, 1, 0);
		glScaled(0.01, 0.015, 0.015);
		ghost.Draw();
	}
	glPopMatrix();
}
void drawSphere2(float posX, float posY, float posZ, bool draw) {
	glPushMatrix();
	candyPowerup.visible = draw;
	glTranslated(posX, posY - 1, posZ);
	glRotated(-90, 0, 1, 0);
	glRotated(-20, 1, 0, 0);
	glScaled(0.05, 0.05, 0.05);
	candyPowerup.Draw();
	glPopMatrix();
}
//=======================================================================
// Display Function
//=======================================================================
void floatToString(char* destStr, int precision, float val)
{
	sprintf(destStr, "%f", val);
	destStr[precision] = '\0';
}
void writeBitmapString(void* font, char* string)
{
	char* c;
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}
static char theStringBuffer[10];

void actM(int button, int state, int x, int y)//mouse function takes 4 parameters: button: which button has been clicked (GLUT_RIGHT_BUTTON or GLUT_LEFT_BUTTON),
											//state wether the button is clicked or released (GLUT_UP or GLUT_DOWN)
											// x and y are the position of the mouse cursor
{
	if (currCamera == 1) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//if the left button has been clicked then translate the square to the mouse position
		{
			speed = 1;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)//if the left button has been clicked then translate the square to the mouse position
		{
			speed = 0;
		}

		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//if the right button has been clicked, translate  the square to the origin (0,0)
		{
			speed = -1;
		}
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)//if the right button has been clicked, translate  the square to the origin (0,0)
		{
			speed = 0;
		}
	}
	glutPostRedisplay();//redisplay to update the screen with the new paraeters
}

void myDisplay(void)
{
	if (!lost) {
		if (level == 0) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(1, 1, 1);
			collision();
			//LIGHTSETTINGS
			GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1 };
			GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
			glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
			glLightfv(GL_LIGHT1, GL_AMBIENT, lightIntensity);

			//SPOTLIGHT
			push();
			if (lane == 0)
				glTranslated(-4, 0, playerZ);
			else if (lane == 1)
				glTranslated(0, 0, playerZ);
			else
				glTranslated(4, 0, playerZ);

			GLfloat lightPos2[] = { 0, 10, 10, 1 }; // Spotlight position.
			GLfloat spotDirection2[] = { 0, -0.4, -1 }; // Spotlight direction.   
			  // Light property vectors.
			GLfloat lightAmb2[] = { 0, 0.0, 0.0, 1.0 };
			GLfloat lightDifAndSpec2[] = { 1, 1, 1, 1.0 };
			//GLfloat globAmb2[] = { 0.05, 0.05, 0.05, 1.0 };

			// Light properties.
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb2);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec2);
			glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec2);
			// Spotlight properties including position.
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);
			glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15);
			glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection2);
			glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2);
			pop();

			// Draw Ground	
			glPushMatrix();
			//glScalef(1, 1, 50);
			for (int i = 0; i < 200; i++) {
				push();
				glTranslatef(0, 0, -(i * 10));
				glScalef(1, 1, 0.5);
				road.Draw();

				pop();
			}
			//road.Draw();
			for (int i = 0; i < 25; i++) {
				push();
				if (i % 2 == 0) {
					glTranslatef(20, 0, -(i * 80));
					glRotatef(250, 0, 1, 0);
				}
				else {
					glTranslatef(-30, 0, -(i * 80));
					glRotatef(-45, 0, 1, 0);
				}
				glScalef(2, 2, 2);
				model_house.Draw();
				pop();
			}

			push();
			glTranslated(0, -0.1, 0);
			glScaled(50, 50, 50);
			RenderGround2();
			pop();

			glPopMatrix();

			push();
			glTranslatef(6, 0, -865);
			glRotatef(270, 0, 1, 0);
			glScalef(2, 2, 2);
			targetHouse.Draw();
			pop();

			glPushMatrix();
			if (lane == 0) {
				glTranslatef(0, 0, playerZ);
			}
			if (lane == 1) {
				glTranslatef(4, 0, playerZ);
			}
			if (lane == 2) {
				glTranslatef(8, 0, playerZ);
			}
			glScalef(2, 2, 2);
			glRotatef(90, 0, 1, 0);
			
			if (!powered)
				car.Draw(); //assume car length goes from 0 to -5
			else
				car2.Draw();
			glPopMatrix();
			push(); //powerups
			//glColor3f(1, 1, 1);
			drawSphere(-4, 2, -200, power1);
			drawSphere(4, 2, -500, power2);
			//drawWall(0.0001);
			pop();

			push(); //obstacles
			drawCube(0, 2, -50, 1, 1, 1, obstacle1, 1);
			drawCube(0, 2, -350, 1, 1, 1, obstacle2, 2);
			drawCube(0, 2, -450, 1, 1, 1, obstacle3, 1);
			drawCube(0, 2, -750, 1, 1, 1, obstacle4, 2);
			//obstacles in lane 0
			drawCube(-4, 2, -650, 1, 1, 1, obstacle5, 1);
			drawCube(-4, 2, -250, 1, 1, 1, obstacle6, 2);
			//obstacles in lane 2
			drawCube(4, 2, -150, 1, 1, 1, obstacle7, 1);
			drawCube(4, 2, -550, 1, 1, 1, obstacle8, 2);
			pop();
			push();
			push();
			glDisable(GL_LIGHTING);
			floatToString(theStringBuffer, 6, score);

			glColor3f(1.0, 1.0, 1.0);
			if (currCamera == 0) {

				glRasterPos3f(-8, 7, playerZ);
			}
			else {
				if (lane == 0)
					glRasterPos3f(-10, 4, playerZ - 10);
				else if (lane == 1) {
					glRasterPos3f(-6, 4, playerZ - 10);
				}
				else {
					glRasterPos3f(-2, 4, playerZ - 10);
				}
			}

			glScaled(5, 5, 5);
			writeBitmapString((void*)font, "SCORE: ");
			writeBitmapString((void*)font, theStringBuffer);
			glEnable(GL_LIGHTING);

			pop();
			push();
			glDisable(GL_LIGHTING);
			floatToString(theStringBuffer, 6, lives);

			glColor3f(1.0, 1.0, 1.0);
			if (currCamera == 0) {

				glRasterPos3f(-8, 6.5, playerZ);
			}
			else {
				if (lane == 0)
					glRasterPos3f(-10, 3.5, playerZ - 10);
				else if (lane == 1) {
					glRasterPos3f(-6, 3.5, playerZ - 10);
				}
				else {
					glRasterPos3f(-2, 3.5, playerZ - 10);
				}
			}
			glScaled(5, 5, 5);

			writeBitmapString((void*)font, "Lives: ");
			writeBitmapString((void*)font, theStringBuffer);
			glEnable(GL_LIGHTING);
			pop();
			pop();

			glPushMatrix();
			// unsolved 
			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(50, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 1000, 1000, 1000);
			gluDeleteQuadric(qobj);

			glPopMatrix();
			glutSwapBuffers();
		}
		else {

			//level2
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(1, 1, 1);

			collision();
			//LIGHTSETTINGS
			GLfloat lightIntensity[] = { 0.5, 0.3, 0.3, 1 };
			GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
			glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
			glLightfv(GL_LIGHT1, GL_AMBIENT, lightIntensity);

			//SPOTLIGHT
			push();
			if (lane == 0)
				glTranslated(-4, 0, playerZ);
			else if (lane == 1)
				glTranslated(0, 0, playerZ);
			else
				glTranslated(4, 0, playerZ);

			GLfloat lightPos2[] = { 0, 10, 10, 1 }; // Spotlight position.
			GLfloat spotDirection2[] = { 0, -0.4, -1 }; // Spotlight direction.   
			  // Light property vectors.
			GLfloat lightAmb2[] = { 0, 0.0, 0.0, 1.0 };
			GLfloat lightDifAndSpec2[] = { 1, 1, 1, 1.0 };
			//GLfloat globAmb2[] = { 0.05, 0.05, 0.05, 1.0 };

			// Light properties.
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb2);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec2);
			glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec2);
			// Spotlight properties including position.
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);
			glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15);
			glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection2);
			glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2);
			pop();
	
			glPushMatrix();
			for (int i = 0; i < 200; i++) {
				push();
				glTranslatef(0, 0, -(i * 10));
				glScalef(1, 1, 0.5);
				road.Draw();
				pop();
			}
			glPopMatrix();
			for (int i = 0; i < 25; i++) {
				push();
				if (i % 2 == 0)
					glTranslatef(-10, 1, -(i * 80));
				else
					glTranslatef(25, 1, -(i * 80));
				glScaled(5, 5, 5);
				glRotated(90, 0, 0, 1);
				glRotated(-90, 0, 1, 0);
				deadTree.Draw();
				pop();
			}

			push();
			glTranslated(0, -0.1, 0);
			glScaled(50, 50, 50);
			RenderGround();
			pop();

			glPushMatrix();
			//glTranslatef(0, 0, 0);

			//glScalef(0.01,0.01,0.01);
			if (lane == 0) {
				glTranslatef(0, 0, playerZ);
			}
			if (lane == 1) {
				glTranslatef(4, 0, playerZ);
			}
			if (lane == 2) {
				glTranslatef(8, 0, playerZ);
			}
			glScalef(2, 2, 2);
			glRotatef(90, 0, 1, 0);
	
			if (!powered)
				car.Draw(); //assume car length goes from 0 to -5
			else
				car2.Draw();
			glPopMatrix();

			push(); //powerups
			//glColor3f(1, 1, 1);
			drawSphere2(-4, 2, -200, power1);

			drawSphere2(4, 2, -500, power2);
			//drawWall(0.0001);
			pop();

			push();
			glTranslated(0, 0, -865);
			//glRotatef(90.f, 1, 0, 0);
			glScalef(0.5, 0.5, 0.5);
			halloweenHouse.Draw();
			pop();

			push(); //obstacles
			drawCube2(0, 2, -50, 1, 1, 1, obstacle1, 1);
			drawCube2(0, 2, -350, 1, 1, 1, obstacle2, 2);
			drawCube2(0, 2, -450, 1, 1, 1, obstacle3, 1);
			drawCube2(0, 2, -750, 1, 1, 1, obstacle4, 2);
			//obstacles in lane 0
			drawCube2(-4, 2, -650, 1, 1, 1, obstacle5, 1);
			drawCube2(-4, 2, -250, 1, 1, 1, obstacle6, 2);

			//obstacles in lane 2
			drawCube2(4, 2, -150, 1, 1, 1, obstacle7, 1);
			drawCube2(4, 2, -550, 1, 1, 1, obstacle8, 2);

			pop();

			//WRITING
			push();

			push();
			glDisable(GL_LIGHTING);
			floatToString(theStringBuffer, 6, score);

			glColor3f(1.0, 1.0, 1.0);
			if (currCamera == 0) {

				glRasterPos3f(-8, 7, playerZ);
			}
			else {
				if (lane == 0)
					glRasterPos3f(-10, 4, playerZ - 10);
				else if (lane == 1) {
					glRasterPos3f(-6, 4, playerZ - 10);
				}
				else {
					glRasterPos3f(-2, 4, playerZ - 10);
				}
			}
			glScaled(5, 5, 5);

			writeBitmapString((void*)font, "SCORE: ");
			writeBitmapString((void*)font, theStringBuffer);
			glEnable(GL_LIGHTING);

			pop();
			push();
			glDisable(GL_LIGHTING);
			floatToString(theStringBuffer, 6, lives);

			glColor3f(1.0, 1.0, 1.0);
			if (currCamera == 0) {

				glRasterPos3f(-8, 6.5, playerZ);
			}
			else {
				if (lane == 0)
					glRasterPos3f(-10, 3.5, playerZ - 10);
				else if (lane == 1) {
					glRasterPos3f(-6, 3.5, playerZ - 10);
				}
				else {
					glRasterPos3f(-2, 3.5, playerZ - 10);
				}
			}
			glScaled(5, 5, 5);

			writeBitmapString((void*)font, "Lives: ");
			writeBitmapString((void*)font, theStringBuffer);
			glEnable(GL_LIGHTING);
			pop();
			pop();

			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(50, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex2);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 1000, 1000, 1000);
			gluDeleteQuadric(qobj);
			glPopMatrix();
			glutSwapBuffers();
		}
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1, 1, 1);

		push();
		glDisable(GL_LIGHTING);

		glColor3f(1.0, 1.0, 1.0);
		if (currCamera == 0) {

			glRasterPos3f(0, 0, playerZ - 5);
		}
		else {
			if (lane == 0)
				glRasterPos3f(0, 0, playerZ - 10);
			else if (lane == 1) {
				glRasterPos3f(0, 0, playerZ - 10);
			}
			else {
				glRasterPos3f(0, 0, playerZ - 10);
			}
		}
		glScaled(5, 5, 5);

		writeBitmapString((void*)font, "GAME OVER");
		glEnable(GL_LIGHTING);
		pop();
	}
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case '1': {

		camera.eye.x = 0;
		camera.eye.y = 6;
		camera.eye.z = 15 + (playerZ);
		camera.center.x = 0;
		camera.center.y = 5;
		camera.center.z = 10 + (playerZ);
		camera.up.x = 0;
		camera.up.y = 1;
		camera.up.z = 0;
		glLoadIdentity();	//Clear Model_View Matrix
		camera.look();
		currCamera = 0;
	}
			break;
	case '2': {
		camera.eye.x = 0;
		camera.eye.y = 3;
		camera.eye.z = 0 + (playerZ);
		camera.center.x = 0;
		camera.center.y = 2;
		camera.center.z = -5 + (playerZ);
		camera.up.x = 0;
		camera.up.y = 1;
		camera.up.z = 0;
		glLoadIdentity();	//Clear Model_View Matrix
		camera.look();
		currCamera = 1;
	}
			break;
		
	case 'a': {
		if (lane > 0) {
			lane--;
		}
	}
			break;
	case 'd': {
		if (lane < 2) {
			lane++;
		}
	}break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	if (currCamera == 0) {
		if (button == 'w')
			speed = 1;
		if (button == 's')
			speed = -1;
	}
	glutPostRedisplay();
}


void keyUp(unsigned char k, int x, int y)//keyboard up function is called whenever the keyboard key is released
{
	if (k == 'w' || k == 's')
		speed = 0;
	glutPostRedisplay();//redisplay to update the screen with the changed
}

//=======================================================================
// Reshape Function
//=======================================================================

void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}
	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	model_tree.Load("Models/tree/Tree1.3ds");
	model_house.Load("Models/house/Casa 29.3ds");
	targetHouse.Load("Models/game/casa 4.3ds");
	halloweenHouse.Load("models/game/VilageHouse_3DS.3DS");
	car.Load("Models/Game/Crysler_new_yorker_1980.3DS");
	car2.Load("Models/Game2/Crysler_new_yorker_1980.3DS");
	gasPowerup.Load("Models/Game/gasContain.3DS");
	blockade.Load("Models/Game/RoadBlockade_01.3DS");
	candyPowerup.Load("Models/Game/CandyCane3DS.3DS");
	cone.Load("Models/Game/cone_3ds.3DS");
	deadTree.Load("Models/Game/deadtree.3DS");
	regularTree.Load("Models/Game/Tree1.3DS");
	ghost.Load("Models/Game/ghost01.3DS");
	pumpkin.Load("Models/Game/Pumpkin3DS.3DS");
	skull.Load("Models/Game/skull.3DS");
	fence.Load("Models/Game/fence_01_3ds.3DS");
	road.Load("Models/Game/HwRoad.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_ground2.Load("Textures/Asphalt.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);

	loadBMP(&tex2, "Textures/night sky.bmp", true);
}

void Anim()
{
	if (currCamera == 0) {
		if (passX < 430) {
			lane = 0;
			playerX = 0;
			playerX2 = -4;
		}
		else if (passX < 830) {
			lane = 1;
			playerX = 4; playerX2 = 0;
		}
		else {
			lane = 2;
			playerX = 8; playerX2 = 4;
		}


	}
	if (currCamera == 1) {
		if (lane == 1) {
			camera.center.x = 0;
			camera.eye.x = 0;

			glLoadIdentity();	//Clear Model_View Matrix
			camera.look();

		}
		if (lane == 2) {
			camera.center.x = 4;
			camera.eye.x = 4;

			glLoadIdentity();	//Clear Model_View Matrix
			camera.look();
		}
		if (lane == 0) {
			camera.center.x = -4;
			camera.eye.x = -4;

			glLoadIdentity();	//Clear Model_View Matrix
			camera.look();
		}
	}

	glutPostRedisplay();
}

void timer(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
	if (lives > 0)
	{
		if (playerZ >= -850)
		{
			//movement
			if (speed == 0) {
				score += 1;
				playerZ -= 0.5;
				camera.eye.z -= 0.5;
				camera.center.z -= 0.5;
			}
			else if (speed == 1) {
				score += 3;
				playerZ -= 1;
				camera.eye.z -= 1;
				camera.center.z -= 1;
			}
			else if (speed == -1) {
				score += 0.25;
				playerZ -= 0.25;
				camera.eye.z -= 0.25;
				camera.center.z -= 0.25;
			}
		}
		else {
			if (level == 0) {
				justHit = 0;

				obstacle1 = true;
				obstacle2 = true;
				obstacle3 = true;
				obstacle4 = true;
				obstacle5 = true;
				obstacle6 = true;
				obstacle7 = true;
				obstacle8 = true;
				PlaySound(TEXT("Sounds/levelComplete.wav"), NULL, SND_ASYNC);
				powered = false;
				score = 0;
				lives = 3;
				power1 = true;
				power2 = true;
				lane = 1; // can be 0 for left, 1 for middle, 2 for right
				playerZ = 0;
				speed = 0; //-1 slowing, 0 neutral, 1 speeding
				playerX = 0;
				level = 1;
				currCamera = 0;
				camera.eye.y = 6;
				camera.eye.z = 15;
				camera.center.x = 0;
				camera.center.y = 5;
				camera.center.z = 10;
				camera.up.x = 0;
				camera.up.y = 1;
				camera.up.z = 0;
				glLoadIdentity();	//Clear Model_View Matrix
				camera.look();
			}
			else {

				if (!win) {
					std::cout << "win";
					PlaySound(TEXT("Sounds/win.wav"), NULL, SND_ASYNC);
				}
				win = true;
			}
		}
		glLoadIdentity();	//Clear Model_View Matrix
		camera.look();
		if (justHit > 0) {
			std::cout << "justHit: " << justHit << "\n";;
			justHit -= 2;
		}
	}
	else {
		//char* p0s[20];
		//sprintf((char)p0s, "GAME OVER");
		lost = true;
		std::cout << lost;

	}

	glutPostRedisplay();						// redraw 		
	glutTimerFunc(16.77, timer, 0);					//recall the time function after 1000 ms and pass a zero value as an input to the time func.
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 150);
	glutIdleFunc(Anim);
	glutCreateWindow(title);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutKeyboardUpFunc(keyUp);
	glutPassiveMotionFunc(passM);
	glutTimerFunc(0, timer, 0);
	glutReshapeFunc(myReshape);
	myInit();
	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glutMainLoop();
}