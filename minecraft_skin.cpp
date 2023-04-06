#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include "gsrc.h"
#include "minecraft_skin.h"

#define PI 3.141592654 
#define WIN_POSX    400
#define WIN_POSY    400
#define WIN_WIDTH   400
#define WIN_HEIGHT  300
#define WIN_DEPTH   400

// move control data
static double animate_rate = 5.0f;
static double angle; 
static double t_prev; // previoud time elasped
static int scale = 1;
static bool isLifting = false, walking = false, moveBack = false;// control wether the character is walking, moveBack controls the direction of leg

enum Dir {
	FORWARD, BACKWARD,TURNLEFT,TURNRIGHT
};
int dir = FORWARD;

// body data
static float bodywidth = 50, bodyheight = 75;
static float shoulderwidth = 25, shoulderheight = 15;
static float armwidth = 25, armheight = 70;
static float legwidth = bodywidth / 2, legheight = 75;
static float beltwidth = bodywidth, beltheight = 10;
static float shoewidth = legwidth, shoeheight = 10;
static float thickness = 25, headL = 50;

// Camera properties.
static float cameraFovy = 45.0; // gluPerspective from camera: field of view angle.
static float cameraNearPlane = 0.1; // gluPerspective from camera: near plane.
static float cameraFarPlane = 1000.0; // gluPerspective from camera: far plane.
static float cameraPos[] = { 0.0, 0.0, WIN_DEPTH }; // Camera position.
static float cameraLookAt[] = { 0.0, 0.0, 0.0 }; // gluLookAt from camera: point looked at (i.e., center).
static float cameraUp[] = { 0.0, 1.0, 0.0 }; // gluLookAt from camear: up direction.

// light properties
static float ka = 1, kd = 1, ks = 1, matShiness = 50;
static float lightAmb[] = { 1.0, 1.0, 1.0, 1.0 }; // Light ambient values.
static float lightDifAndSpec[] = { 0.8, 0.8, 0.8, 1.0 }; // Light diffuse and specular values.
static GLfloat ambientCoeff[] = { ka, ka, ka, 1.0 }; // coefficient of ambient refelction
static GLfloat diffuseCoeff[] = { kd, kd, kd, 1.0 };// coefficient of diffuse refelction
static GLfloat specularCoeff[] = { ks, ks, ks, 1.0 };// coefficient of specular refelction

static float lightFarPlane = 35.0; // gluPerspective from light: far plane.
static float lightFovy = 120.0; // gluPerspective from light: field of view angle.
static float lightAspect = 1.0; // gluPerspective from light: aspect ratio.
static float lightNearPlane = 1.0; // gluPerspective from light: near plane.

static float lightPos[] = { 0, 50, 0.0}; // Light position.
static float lightLookAt[] = { 0.0, 0.0, 0.0 }; // gluLookAt from light: point looked at (i.e., center).
static float lightUp[] = { 1.0, 0.0, 0.0 }; // gluLookAt from light: up direction.

// Material properties.
static float matSpec[] = { 1.0, 1.0, 1.0, 1.0 }; // Material specular values.
static float matShine[] = { 50.0 }; // Material shininess.

// transformation matrix
static float cameraProjMat[16]; // Camera's projection transformation matrix.  
static float cameraViewMat[16]; // Camera's viewing transformation matrix.  
static float lightProjMat[16]; // Light's projection transformation matrix.
static float lightViewMat[16]; // Light's viewing transformation matrix.  



// calculate the center (use plus)
float calCenter(int obj1Len, int pbj2Len) {
	return (obj1Len + pbj2Len) / 2 * scale;
}

// calculate the center (use minus)
float calCenterSub(int obj1Len, int pbj2Len) {
	return (obj1Len - pbj2Len) / 2 * scale;
}

// Listen for direction, arm movements and speed
void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
	case 'W':
		dir = FORWARD;
		walking = true;
		break;
	case 's':
	case 'S':
		dir = BACKWARD;
		walking = true;
		break;
	case 'a':
	case 'A':
		dir = TURNLEFT;
		walking = true;
		break;
	case 'd':
	case 'D':
		dir = TURNRIGHT;
		walking = true;
		break;
	case ' ':
		isLifting = !isLifting;
		break;
	}
	//glutPostRedisplay();
}


// Listen for direction, arm movements and speed
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
	case ' ': // avoid invalid choices
		break;
	case 'w':
	case 'W':
		walking = false;
		break;
	case 's':
	case 'S':
		walking = false;
		break;
	case 'a':
	case 'A':
		walking = false;
		break;
	case 'd':
	case 'D':
		walking = false;
		break;
	}
	//glutPostRedisplay();
}

void animate(void) {
	if (!walking) return; // if no key down, don't move
	double t; // record time elapsed
	double rangeAngle = 10.0f; // maximum degree
	double maxAngle = rangeAngle * 2;
	double totalTime = 1000.0;
	t = animate_rate;
	double angleTrans = 0;

	angleTrans = t / totalTime;
	// moveBack controls the direction of leg
	if (angle <= rangeAngle && !moveBack) {
		angle += maxAngle * angleTrans;
		if (angle >= rangeAngle) {
			moveBack = true;
			angle = rangeAngle;
		}
	}
	else if (angle >= -rangeAngle && moveBack) {
		angle -= maxAngle * angleTrans;
		if (angle <= -rangeAngle) {
			moveBack = false;
			angle = -rangeAngle;
		}
	}

	// stop at swing_angle
	glutPostRedisplay();
}


// draw eyes: 2 pixels wide (white and blue) by 1 pixels high, located on the face in columns 2-3 and 6-7, rows 5 
// lefteye should follow anticlockwise
void draw_eye(void) {
	glBegin(GL_QUADS);
	// the white of the ey
	glColor3ub(255, 255, 255);
	glVertex3f(-0.375f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(-0.250f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(-0.250f * headL, 0, 0.51f * headL);
	glVertex3f(-0.375f * headL, 0, 0.51f * headL);
	// eyeball
	glColor3ub(0, 0, 255);
	glVertex3f(-0.250f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(-0.125f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(-0.125f * headL, 0, 0.51f * headL);
	glVertex3f(-0.250f * headL, 0, 0.51f * headL);

	// the white of the ey
	glColor3ub(255, 255, 255);
	glVertex3f(0.375f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(0.250f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(0.250f * headL, 0, 0.51f * headL);
	glVertex3f(0.375f * headL, 0, 0.51f * headL);
	// eyeball
	glColor3ub(0, 0, 255);
	glVertex3f(0.250f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(0.125f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(0.125f * headL, 0, 0.51f * headL);
	glVertex3f(0.250f * headL, 0, 0.51f * headL);
	glEnd();
}

void draw_nose(void) {
	glBegin(GL_QUADS);
	glColor3ub(139, 69, 19);
	glVertex3f(-0.125f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(-0.125f * headL, -0.250f * headL, 0.51f * headL);
	glVertex3f(0.125f * headL, -0.250f * headL, 0.51f * headL);
	glVertex3f(0.125f * headL, -0.125f * headL, 0.51f * headL);
	glEnd();
}

void draw_mouse(void) {
	glBegin(GL_QUADS);
	glColor3ub(160, 82, 45);
	glVertex3f(-0.250f * headL, -0.250f * headL, 0.51f * headL);
	glVertex3f(-0.250f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(0.250f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(0.250f * headL, -0.250f * headL, 0.51f * headL);

	glColor3ub(0, 0, 0);
	glVertex3f(-0.125f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(-0.125f * headL, -0.51f * headL, 0.51f * headL);
	glVertex3f(0.125f * headL, -0.51f * headL, 0.51f * headL);
	glVertex3f(0.125f * headL, -0.375f * headL, 0.51f * headL);
	glEnd();
}

void draw_hair(void) {
	glBegin(GL_QUADS);
	// front hair
	glColor3ub(51, 25, 0);
	glVertex3f(-0.51f * headL, 0.250f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0.250f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0.51f * headL, 0.51f * headL);
	glVertex3f(-0.51f * headL, 0.51f * headL, 0.51f * headL);

	// side hair , draw a trapezoid
	glVertex3f(0.51f * headL, 0.51f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0 * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0 * headL, 0 * headL);
	glVertex3f(0.51f * headL, 0.51f * headL, 0 * headL);

	glVertex3f(0.51f * headL, 0.51f * headL, 0 * headL);
	glVertex3f(0.51f * headL, -0.51f * headL, 0 * headL);
	glVertex3f(0.51f * headL, -0.51f * headL, -0.51f * headL);
	glVertex3f(0.51f * headL, 0.51f * headL, -0.51f * headL);

	// right side hair
	glVertex3f(-0.51f * headL, 0.51f * headL, 0.51f * headL);
	glVertex3f(-0.51f * headL, 0 * headL, 0.51f * headL);
	glVertex3f(-0.51f * headL, 0 * headL, 0 * headL);
	glVertex3f(-0.51f * headL, 0.51f * headL, 0 * headL);

	glVertex3f(-0.51f * headL, 0.51f * headL, 0 * headL);
	glVertex3f(-0.51f * headL, -0.51f * headL, 0 * headL);
	glVertex3f(-0.51f * headL, -0.51f * headL, -0.51f * headL);
	glVertex3f(-0.51f * headL, 0.51f * headL, -0.51f * headL);

	//back hair
	glVertex3f(-0.51f * headL, 0.51f * headL, -0.51f * headL);
	glVertex3f(-0.51f * headL, -0.51f * headL, -0.51f * headL);
	glVertex3f(0.51f * headL, -0.51f * headL, -0.51f * headL);
	glVertex3f(0.51f * headL, 0.51f * headL, -0.51f * headL);

	// top hair
	glVertex3f(-0.51f * headL, 0.51f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0.51f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0.51f * headL, -0.51f * headL);
	glVertex3f(-0.51f * headL, 0.51f * headL, -0.51f * headL);

	glEnd();
}

// draw collar of the character
void draw_collar(void) {
	glBegin(GL_QUADS);
	glColor3ub(205, 133, 63);
	glVertex3f(-0.5f * bodywidth, 0.5f * bodyheight, 0.51f * thickness);
	glVertex3f(-0.5f * bodywidth, 0.4f * bodyheight, 0.51f * thickness);
	glVertex3f(0.5f * bodywidth, 0.4f * bodyheight, 0.51f * thickness);
	glVertex3f(0.5f * bodywidth, 0.5f * bodyheight, 0.51f * thickness);

	glVertex3f(-0.250f * bodywidth, 0.4f * bodyheight, 0.51f * thickness);
	glVertex3f(-0.250f * bodywidth, 0.3f * bodyheight, 0.51f * thickness);
	glVertex3f(0.250f * bodywidth, 0.3f * bodyheight, 0.51f * thickness);
	glVertex3f(0.250f * bodywidth, 0.4f * bodyheight, 0.51f * thickness);
	glEnd();
}

void draw_scar(void) {
	glBegin(GL_QUADS);
	// left scar
	glColor3ub(139, 69, 19);
	glVertex3f(-0.51f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(-0.51f * headL, -0.51f * headL, 0.51f * headL);
	glVertex3f(-0.375f * headL, -0.51f * headL, 0.51f * headL);
	glVertex3f(-0.375f * headL, -0.375f * headL, 0.51f * headL);

	glColor3ub(139, 50, 43);
	glVertex3f(-0.51f * headL, -0.250f * headL, 0.51f * headL);
	glVertex3f(-0.51f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(-0.375f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(-0.375f * headL, -0.250f * headL, 0.51f * headL);

	glColor3ub(139, 45, 43);
	glVertex3f(-0.51f * headL, -0.125f * headL, 0.51f * headL);
	glVertex3f(-0.51f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(-0.375f * headL, -0.375f * headL, 0.51f * headL);
	glVertex3f(-0.375f * headL, -0.125f * headL, 0.51f * headL);

	// top scar
	glColor3ub(139, 45, 20);
	glVertex3f(0.250f * headL, 0.250f * headL, 0.51f * headL);
	glVertex3f(0.250f * headL, 0.125f * headL, 0.51f * headL);
	glVertex3f(0.375f * headL, 0.125f * headL, 0.51f * headL);
	glVertex3f(0.375f * headL, 0.250f * headL, 0.51f * headL);

	glColor3ub(139, 45, 10);
	glVertex3f(0.375f * headL, 0.250f * headL, 0.51f * headL);
	glVertex3f(0.375f * headL, 0.125f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0.125f * headL, 0.51f * headL);
	glVertex3f(0.51f * headL, 0.250f * headL, 0.51f * headL);
	glEnd();
}


void draw_shirt_scar(void) {
	glBegin(GL_QUADS);
	glColor3ub(0, 144, 205);
	glVertex3f(-0.3f * bodywidth, 0.2f * bodyheight, 0.51f * thickness);
	glVertex3f(-0.3f * bodywidth, -0.1f * bodyheight, 0.51f * thickness);
	glVertex3f(-0.2f * bodywidth, -0.1f * bodyheight, 0.51f * thickness);
	glVertex3f(-0.2f * bodywidth, 0.2f * bodyheight, 0.51f * thickness);


	glVertex3f(0.31f * bodywidth, 0.3f * bodyheight, 0.51f * thickness);
	glVertex3f(0.31f * bodywidth, -0.5f * bodyheight, 0.51f * thickness);
	glVertex3f(0.51f * bodywidth, -0.5f * bodyheight, 0.51f * thickness);
	glVertex3f(0.51f * bodywidth, 0.2f * bodyheight, 0.51f * thickness);

	glVertex3f(-0.4f * bodywidth, -0.1f * bodyheight, 0.51f * thickness);
	glVertex3f(-0.4f * bodywidth, -0.4f * bodyheight, 0.51f * thickness);
	glVertex3f(0.2f * bodywidth, -0.4f * bodyheight, 0.51f * thickness);
	glVertex3f(0.2f * bodywidth, -0.1f * bodyheight, 0.51f * thickness);
	glEnd();
}

void draw_palm(void) {
	glBegin(GL_QUADS);
	glColor3ub(205, 103, 33);
	glVertex3f(-0.4f * armwidth, -0.3f * armheight, 0.51f * thickness);
	glVertex3f(-0.4f * armwidth, -0.5f * armheight, 0.51f * thickness);
	glVertex3f(0.4f * armwidth, -0.5f * armheight, 0.51f * thickness);
	glVertex3f(0.4f * armwidth, -0.3f * armheight, 0.51f * thickness);
	glEnd();
}


void draw_knee(void) {
	glBegin(GL_QUADS);
	glColor3ub(0, 34, 20);
	glVertex3f(-0.2f * legwidth, -0.1f * legheight, 0.51f * thickness);
	glVertex3f(-0.2f * legwidth, -0.2f * legheight, 0.51f * thickness);
	glVertex3f(0.2f * legwidth, -0.2f * legheight, 0.51f * thickness);
	glVertex3f(0.2f * legwidth, -0.1f * legheight, 0.51f * thickness);

	glVertex3f(-0.51f * legwidth, -0.1f * legheight, -0.51f * thickness);
	glVertex3f(-0.51f * legwidth, -0.2f * legheight, -0.51f * thickness);
	glVertex3f(0.51f * legwidth, -0.2f * legheight, -0.51f * thickness);
	glVertex3f(0.51f * legwidth, -0.1f * legheight, -0.51f * thickness);
	glEnd();
}

void cube(void) {
	// draw a cube(size = scale ), centers at (0, 0, 0)
	glPushMatrix();
	glutSolidCube(scale);
	glPopMatrix();
}
// color Steve : Steve: hair(110, 73, 40)£¬skin(224, 172, 105)£¬thirt(60, 125, 201)£¬pants(44, 62, 95)£¬shoes(26, 26, 26)
void draw_body(void) {
	draw_collar();
	draw_shirt_scar();

	glPushMatrix();
	// azure 60*70*30 azure cube
	glPushMatrix();
	glColor3ub(0, 154, 205);
	glScalef(bodywidth, bodyheight, thickness);
	cube();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, calCenter(-bodyheight, -beltheight), 0);
	glColor3f(0.20f, 0.40f, 0.70f);
	glScalef(beltwidth, beltheight, thickness);
	cube();
	glPopMatrix();

	glPopMatrix();
}
void draw_arm(void) {
	// draw left/right upper arm
	glPushMatrix();
	glPushMatrix();
	// rotate arm with keyboard space
	if (isLifting) {
		glRotatef(90.0f, -1.0, 0, 0);
	}
	glColor3ub(0, 154, 205);
	glScalef(shoulderwidth, shoulderheight, thickness);
	cube();
	glPopMatrix();
	
	// draw left/right lower arm
	glPushMatrix();
	if (isLifting) {
		glRotatef(90.0f, -1.0, 0, 0);
	}
	glTranslatef(0, calCenter(-shoulderheight, -armheight), 0); // M lower <- upper 
	draw_palm();
	glColor3ub(205,133,63);
	glScalef(armwidth, armheight, thickness);
	cube();
	
	glPopMatrix();

	glPopMatrix();
}
void draw_leg(void) {
	glPushMatrix();
	draw_knee();
	// draw upper leg
	glPushMatrix();
	// add rotate animation
	glColor3f(0.17f, 0.24f, 0.37f);
	glScalef(legwidth, legheight, thickness);
	cube();
	glPopMatrix();
	
	// draw shoe
	glPushMatrix();
	glTranslated(0, calCenter(-shoeheight, -legheight), 0);
	glColor3f(0.10f, 0.10f, 0.10);
	glScalef(shoewidth, shoeheight, thickness);
	cube();
	glPopMatrix();

	glPopMatrix();
}




void draw_head(void) {
// brown, 60*60*60 cube, origin point at top center
	glPushMatrix();
	// GLfloat face[8][2] = { {} };
	glColor3ub(205, 133, 63);
	glScalef(headL, headL, headL);
	cube();
	glPopMatrix();

	draw_eye();
	draw_nose();
	draw_mouse();
	draw_hair();
	draw_scar();
	glPopMatrix();
}

void draw_figure(void) {
	// draw the skin
	// control the direction of movement
	switch (dir)
	{
	case TURNLEFT:
		glRotatef(-90.0f, 0, 1.0, 0);
		break;
	case TURNRIGHT:
		glRotatef(90.0f, 0, 1.0, 0);
		break;
	case BACKWARD:
		glRotatef(180.0f, 0, 1.0, 0);
		break;
	default:
		break;
	}
	draw_body();

	glPushMatrix();
	glTranslatef(0, calCenter(bodyheight, headL), 0); // Mhead <- body
	draw_head();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-calCenter(bodywidth, shoulderwidth), calCenterSub(bodyheight, shoulderheight), 0); // M leftarm <- body
	if (!isLifting) glRotatef(angle, 1.0, 0, 0);
	draw_arm();
	glPopMatrix();

	glPushMatrix();
	glTranslated(calCenter(bodywidth, shoulderwidth), calCenterSub(bodyheight, shoulderheight), 0);// Mrightarm <- body
	if (!isLifting) glRotatef(angle, -1.0, 0, 0);
	glScaled(-1, 1, 1);
	draw_arm();
	glPopMatrix();

	glPushMatrix();
	// move motion
	glRotatef(angle, -1.0, 0, 0);
	glTranslated(-calCenterSub(bodywidth, legwidth), -calCenter(legheight, bodyheight) - beltheight, 0); // Mleft_leg <- body
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 1.0, 0, 0);

	glTranslated(calCenterSub(bodywidth, legwidth), -calCenter(legheight, bodyheight) - beltheight, 0); // Mright_leg <- body
	glScaled(-1, 1, 1);
	draw_leg();
	glPopMatrix();
}

void draw_scene(void) {

	// enable Z buffer method for visibility
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// clear display color buffer and set color to white for lighting
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// load viewing and projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraProjMat);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cameraViewMat);

	// multipole model_view_matrix, include rotation user-interface
	glMultMatrixf(gsrc_getmo());

	draw_figure();

	glutSwapBuffers();
}


void enableLightingAndText(void) {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);

	// set up the light intensity as (R, G, B)
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec);
	// setup a point light source position
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	glEnable(GL_LIGHT1);    // turn on light 1

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientCoeff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseCoeff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularCoeff);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShiness);

	// enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	// set the color material parameters
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

}

void pre_setup(void) {
	// set up light and texture mapping 
	enableLightingAndText();
	// set up light and camera transformation matrix(view and projection)

	glPushMatrix();
	
	// set view port 
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	float winRatio = (float)(viewport[2] / viewport[3]);

	// Camera's projection transformation matrix.  
	glLoadIdentity();
	gluPerspective(cameraFovy, winRatio, cameraNearPlane, cameraFarPlane); //viewport[3] may be zero?
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjMat);

	// Camera's viewing transformation matrix.  
	glLoadIdentity();
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraLookAt[0], cameraLookAt[1], cameraLookAt[2],
		cameraUp[0], cameraUp[1], cameraUp[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMat);

	// Light's projection transformation matrix.  
	glLoadIdentity();
	gluPerspective(lightFovy, lightAspect, lightNearPlane, lightFarPlane);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightProjMat);

	// Light's viewing transformation matrix.  
	glLoadIdentity();
	gluLookAt(lightPos[0], lightPos[1], lightPos[2], lightLookAt[0], lightLookAt[1], lightLookAt[2],
		lightUp[0], lightUp[1], lightUp[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMat);

	glPopMatrix();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);// enable double buffer, RGB, depth buffer
	glutInitWindowPosition(WIN_POSX, WIN_POSY);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow("minecraft skin:");

	// record the elapse time to calculate angle
	t_prev = glutGet(GLUT_ELAPSED_TIME);

	printf("welcome to CG! Here you can control the movement with (WASD) and use SPACE to lift his arm! \n");
	printf("choose amimation_rate(1, 10): \n");
	scanf_s("%3lf", &animate_rate);
	printf("animate_rate: %3lf", animate_rate);

	glutIdleFunc(animate);

	// listening keyboard
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	// register moush-click and mouse-move glut for rotation user-interface in draw_secne
	glutMouseFunc(gsrc_mousebutton);
	glutMotionFunc(gsrc_mousemove);
	
	// display
	glutDisplayFunc(draw_scene);

	// light and texture settings and view/projection transformation matrix(from camera and lights)
	pre_setup();

	// keep in loop
	glutMainLoop();
}