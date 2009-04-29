/**
 * @file	ModelViewer.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 01.03.2009
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <map>

#include "GlToolkit.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "OOCTools.h"
#include "ObjModelLoader.h"
#include "RawModelHandler.h"
#include "ColorTable.h"
#include "Camera.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "OctreeHandler.h"
#include "FileIO.h"
#include "LooseProcessingOctree.h"
//#include "glCamera.h"


using namespace std;
using namespace ooctools;
using namespace oocformats;

#define COLOR_TABLE "/home-e/ava/Diplom/Model/colortable.bin"
#define BOUNDING_BOX "/home-e/ava/Diplom/Model/SceneBoundingBox.bin"
#define GET_GLERROR(ret) \
{ \
         GLenum err = glGetError(); \
         if (err != GL_NO_ERROR) { \
                 fprintf(stderr, "[%s line %d] GL Error: %s\n", \
                 __FILE__, __LINE__, gluErrorString(err)); \
                 fflush(stderr); \
         } \
}

Model *model;
Model *model2;
V3f tri1;
V3f tri2;
V3f tri3;

int prevMouseX = 0;
int prevMouseY = 0;
int oldPosX = 0;
int oldPosY = 0;
bool wireFrame = false;
bool vboSwitch= true;
bool showFPS = true;
bool showBBox = false;
bool showLightSource = true;
float zoom = 1.0f;
int mouseLastX, mouseLastY;
float mouseRotX = 0.0f;
float mouseRotY = 0.0f;
int vertexCount = 0;
float transX = 0.0;
float transY = 0.0;
float transZ = 0.0;
float modelScale = 1.0f;
Camera camObj;
//glCamera cam;
//unsigned int    MouseX, MouseY;
int   CenterX, CenterY;

float walkingSpeed = 0.3f;
BoundingBox *bb1;
BoundingBox *bb2;
BoundingBox sbb;
ooctools::Octree* oct2;
OctreeHandler oh;
LooseProcessingOctree* lpo;

VboManager *vboMan;
Fbo *fbo;
ColorTable* ct;


RawModelHandler* moWri;

// frame - the number of frames since we last computed the frame rate
// time - the current number of milliseconds
// timebase - the number of milliseconds since we last computed the frame rate
int frame=0,time_passed,timebase=0;
double fps = 0.0;
int w=512, h=512;

ObjModelLoader moLoader;

/*
 * Begin Phong-Statements
 */
CGprogram g_cgVertexProg;
CGparameter g_cgGlobalAmbient;
CGparameter g_cgLightColor;
CGparameter g_cgEyePosition;
CGparameter g_cgLightPosition;
CGparameter cgColorLut;
CGparameter cgNColors;

CGprogram g_cgFragmentProg;
CGparameter g_cgShininess;
CGparameter g_cgKs;
CGparameter g_cgKd;
CGparameter g_cgKa;
CGparameter g_cgKe;

float myGlobalAmbient[] = { 0.1f, 0.1f, 0.1f };  /* Dim */
float myLightColor[] = { 0.95f, 0.95f, 0.95f };  /* White */
float eyePosition[] = { 0.0f, 0.0f, 8.0f, 1.0f };

float colorLutData[] = {1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f};

unsigned char bbColors[6][3] = {{255,0,0}, {0,255,0}, {0,0,255}, {255,255,0}, {0,255,255}, {255,0,255}};

int g_iFrame=0;
double g_dDist=8.0;
float g_dLightHeight=5.0f;
double g_dLightRadius=3.0;


float lpos[4];

CGparameter g_cgModelViewInv;
/*
 * End Phong-Statements
 */

/*
static void drawTriangle(){
	//(*v1)+=0.5f;
	glBegin(GL_TRIANGLES);
		glColor3f(0.0, 0.0, 0.0);
		glVertex3fv(v1->asArray());
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3fv(v2->asArray());
		glColor3f(1.0, 0.0, 0.0);
		glVertex3fv(v3->asArray());


	glEnd();
}

*/
enum
{
    POSITION_OBJECT = 0,
    COLOR_OBJECT = 1,
    POSITION_POINTER = 2,
    COLOR_POINTER = 3
};

void setBrassMaterial()
{
	float brassEmissive[] = {0.0f,  0.0f,  0.0f};
	float brassAmbient[] = {0.33f, 0.22f, 0.03f};
	float brassDiffuse[] = {0.78f, 0.57f, 0.11f};
	float brassSpecular[] = {0.99f, 0.91f, 0.81f};
	float brassShininess = 27.8f;

	cgSetParameter3fv(g_cgKe, brassEmissive);
	cgSetParameter3fv(g_cgKa, brassAmbient);
	cgSetParameter3fv(g_cgKd, brassDiffuse);
	cgSetParameter3fv(g_cgKs, brassSpecular);
	cgSetParameter1f(g_cgShininess, brassShininess);
}

void drawLpoBbs(LooseProcessingOctree* _lpo){
	if (!_lpo->isLeaf()){
		drawLpoBbs(_lpo->getFnw());
		drawLpoBbs(_lpo->getFne());
		drawLpoBbs(_lpo->getFsw());
		drawLpoBbs(_lpo->getFse());
		drawLpoBbs(_lpo->getBnw());
		drawLpoBbs(_lpo->getBne());
		drawLpoBbs(_lpo->getBsw());
		drawLpoBbs(_lpo->getBse());
	}
	glColor3ubv(bbColors[lpo->getLevel()]);
	_lpo->getBb().draw();
}

void drawXLpoBbs(LooseProcessingOctree* _lpo){
	if (!_lpo->isLeaf()){
		drawXLpoBbs(_lpo->getFnw());
		drawXLpoBbs(_lpo->getFne());
		drawXLpoBbs(_lpo->getFsw());
		drawXLpoBbs(_lpo->getFse());
		drawXLpoBbs(_lpo->getBnw());
		drawXLpoBbs(_lpo->getBne());
		drawXLpoBbs(_lpo->getBsw());
		drawXLpoBbs(_lpo->getBse());
	}
	glColor3ubv(bbColors[_lpo->getLevel()]);
	_lpo->getExtBb().draw();
}

static void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, w, 0, h);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

//void *font = GLUT_BITMAP_9_BY_15;
void *font=(void*)GLUT_BITMAP_HELVETICA_12;

void
drawString(std::string text, unsigned int x, unsigned int y)
{
	glWindowPos2i(x, y);
	const char *string = text.c_str();
  int len, i;

  len = (int) text.size();
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}

void renderSpacedBitmapString(float x, float y, int spacing, void* font,
		string s)
{
	string::iterator sIt;
	int x1=x;
	for (sIt = s.begin(); sIt!=s.end(); ++sIt) {
		glRasterPos2f(x1,y);
		glutBitmapCharacter(font, (*sIt));
		x1 = x1 + glutBitmapWidth(font,(*sIt)) + spacing;
	}
}

static void
resetPerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void
setupShaderPair(string vertexShader, string fragmentShader)
{

}

static void doFPS(){
	frame++;
	time_passed = glutGet(GLUT_ELAPSED_TIME);
	if (time_passed - timebase > 1000) {
		fps = frame*1000.0/(time_passed-timebase);
		timebase = time_passed;
		frame = 0;
	}
	ostringstream s;

	glColor3f(1.0f,1.0f,1.0f);
	glPushMatrix();
	glLoadIdentity();
	void *font=(void*)GLUT_BITMAP_HELVETICA_12;
	setOrthographicProjection();
	s << "FPS: " << fps;
//	renderSpacedBitmapString(30,35,1,font,s.str());
	drawString(s.str(), 30,h-35);
	glPopMatrix();
	resetPerspectiveProjection();

}

static void display() {
	glClearColor(0.0f,0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	lpos[0] = (float)(g_dLightRadius*cos(g_iFrame*3.14f/200.0f));
	lpos[1] = (float)(g_dLightRadius*sin(g_iFrame*3.14f/200.0f));
	lpos[2] = g_dLightHeight;
	lpos[3] = 1.0f;

	cgGLSetParameter3fv(g_cgLightPosition,lpos);
	cgGLSetParameter3fv(g_cgEyePosition,camObj.getEye().getData());


	glPushMatrix();
//		glTranslatef(transX, 0.0, transZ);
		glPushMatrix();
//		cam.SetPrespective();

//			glScalef(zoom, zoom, zoom);
//			glScalef(modelScale,modelScale,modelScale);
//			glRotatef(mouseRotX, 1.0f, 0.0, 0.0);
//			glRotatef(mouseRotY, 0.0f, 1.0, 0.0);

		camObj.applyToGL();
//			gluLookAt(*camObj.getEye().x, *camObj.getEye().y, *camObj.getEye().z,
//					*camObj.getView().x, *camObj.getView().y, *camObj.getView().z,
//					*camObj.getUp().x, *camObj.getUp().y, *camObj.getUp().z);
			glRotatef(camObj.getRoll(), 1.0f, 0.0, 0.0);
//			glTranslatef(*camObj.getEye().x, *camObj.getEye().y, *camObj.getEye().z);

			glPushMatrix();
//				if (showLightSource){
//					glEnable(GL_POINT_SPRITE);
//					glPointSize(10.0f);
//					glColor3f(1.0f, 1.0f, 0.0f);
//					glBegin(GL_POINTS);
//						glVertex3fv(lpos);
//					glEnd();
//					glDisable(GL_POINT_SPRITE);
//				}
			drawLpoBbs(lpo);

				if (showBBox) {
					vboMan->getBb().draw(0,255,0);
					vboMan->drawBbs(255,0,0);
					sbb.draw(255, 255, 0);
				}

//				vboMan->drawNormals();

				// enable profiles
//				glColor3f(0.0f, 1.0f, 0.0f);
//				bb1->draw(1.0f, 0.0f, 0.0f);
//				bb2->draw(0.0f, 1.0f, 0.0f);

				CgToolkit::getSingleton()->startCgShader(CgToolkit::getSingleton()->cgVertexProfile, g_cgVertexProg);
				CgToolkit::getSingleton()->startCgShader(CgToolkit::getSingleton()->cgFragProfile, g_cgFragmentProg);
				GET_GLERROR(0);
				//glColor4ub(222,143,28,1);


				//drawTriangle();
				//drawObj();
				//drawObj2();
				//renderBuffers2();
				//renderBuffers2();
				//drawOldSchool(model);


//				for(map<string, VBO*>::iterator it = vboList->begin(); it!= vboList->end(); ++it){
//					it->second->draw(10);
//				}

				GET_GLERROR(0);
				glBegin(GL_TRIANGLES);
//					glVertex3f(tri1.getX(), tri1.getY(), tri1.getZ());
//					glVertex3f(tri2.getX(), tri2.getY(), tri2.getZ());
//					glVertex3f(tri3.getX(), tri3.getY(), tri3.getZ());
//					glVertex3f(tri3.getX(), tri3.getY(), tri3.getZ());
//					glVertex3f(tri2.getX(), tri2.getY(), tri2.getZ());
//					glVertex3f(tri1.getX(), tri1.getY(), tri1.getZ());
					glVertex4f(tri1.getX(), tri1.getY(), tri1.getZ(), vboMan->getColorTable().calculateTexCoord(0));
					glVertex4f(tri2.getX(), tri2.getY(), tri2.getZ(), vboMan->getColorTable().calculateTexCoord(15));
					glVertex4f(tri3.getX(), tri3.getY(), tri3.getZ(), vboMan->getColorTable().calculateTexCoord(31));
					glVertex4f(tri3.getX(), tri3.getY(), tri3.getZ(), vboMan->getColorTable().calculateTexCoord(8));
					glVertex4f(tri2.getX(), tri2.getY(), tri2.getZ(), vboMan->getColorTable().calculateTexCoord(8));
					glVertex4f(tri1.getX(), tri1.getY(), tri1.getZ(), vboMan->getColorTable().calculateTexCoord(8));
				glEnd();
				GET_GLERROR(0);
				vboMan->drawVbos();
				GET_GLERROR(0);
//				fbo->bind();
//				fbo->clear();
//				vboMan->drawVbo("robot");
//				vboMan->drawVbo("bunny");
//				glGetFloatv (GL_MODELVIEW_MATRIX, mvMatrix);
//				OOCTools::readColorFB(pixels, 0,0, w,h);
//				fbo->unbind();


				CgToolkit::getSingleton()->stopCgShader(CgToolkit::getSingleton()->cgFragProfile);
				CgToolkit::getSingleton()->stopCgShader(CgToolkit::getSingleton()->cgVertexProfile);
				//vboMan->drawVBO("defaultGrp");
				//vbo.draw(60);
				//vbo2.draw(60);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	//glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
//	glWindowPos2i(0,0);
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
//	//glDisable(GL_DEPTH_TEST);
//	glDepthFunc(GL_ALWAYS);
//	glDisable(GL_CULL_FACE);
//	glDrawPixels(w,h,GL_DEPTH_COMPONENT, GL_FLOAT, depth);
//	glDrawPixels(w,h,GL_BGRA, GL_UNSIGNED_BYTE, pixels);
//	glPopAttrib();
	/*
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fbo->getColorTexId());
	glBegin(GL_QUADS);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
*/
	if (showFPS) doFPS();
	++g_iFrame;
	//glutPostRedisplay();
//	exit(0);
	glutSwapBuffers();
	//glFlush();
}

void changeSize(int _w, int _h) {

	w = _w;
	h = _h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, (GLint)w, (GLint)h);
	//glFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 1.0f, 100.0f);					// perspective projection
	//glFrustum(-0.4f, 0.4f, -0.5f, 0.5f, 1.0f, 100.0f);					// perspective projection

	// Set the correct perspective.
	//gluPerspective(45,ratio,1,1000);
	//gluPerspective(20.0,1.0,0.5,50.0);
	gluPerspective(45.0f*zoom, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
	 CenterX = w/2;
	 CenterY = h/2;

	// -------------------------------------
	                     								// load the identity matrix as the modelview (resets all previous transformations)


}

//void CheckMouse(int X,int Y)
//{
//    GLfloat DeltaMouse;
//
//    MouseX = X;
//    MouseY = Y;
//
//    if(MouseX < CenterX)
//    {
//        DeltaMouse = GLfloat(CenterX - MouseX);
//        cam.ChangeHeading(-0.2f * DeltaMouse);
//
//    }
//	else if (MouseX > CenterX) {
//		DeltaMouse = GLfloat(MouseX - CenterX);
//		cam.ChangeHeading(0.2f * DeltaMouse);
//	}
//	if (MouseY < CenterY) {
//		DeltaMouse = GLfloat(CenterY - MouseY);
//		cam.ChangePitch(-0.2f * DeltaMouse);
//	}
//	else if (MouseY > CenterY) {
//		DeltaMouse = GLfloat(MouseY - CenterY);
//		cam.ChangePitch(0.2f * DeltaMouse);
//	}
//	glutWarpPointer(CenterX, CenterY);
//
//	glutPostRedisplay();
//}

//void CheckAlphaKeys(unsigned char key, int x,int y)
//{
//    if(key == 'w')
//    {
//        cam.ChangeVelocity(0.5f);
//    }
//    else if(key == 's')
//    {
//        cam.ChangeVelocity(-0.5f);
//    }
//    else if (key == 27) // esc
//    {
//        exit(0);
//    }
//    glutPostRedisplay();
//}
//
//void CheckKeys(int key, int x,int y)
//{
//    if(key == GLUT_KEY_UP)
//    {
//        cam.ChangePitch(5.0f);
//    }
//    else if(key == GLUT_KEY_DOWN)
//    {
//        cam.ChangePitch(-5.0f);
//    }
//    else if(key == GLUT_KEY_LEFT)
//    {
//        cam.ChangeHeading(-5.0f);
//    }
//    else if(key == GLUT_KEY_RIGHT)
//    {
//        cam.ChangeHeading(5.0f);
//    }
//    glutPostRedisplay();
//}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27 || key == 'q') // a.k.a. quit
		exit(0);
	else if (key == 'f') // toggle show FPS
		showFPS= !showFPS;
	else if (key == 'b') // toggle shoe BoundingBoxes
		showBBox= !showBBox;
	else if (key == 'w') // arrow up
//		transZ+=1.0;
		camObj.moveCamera(1.4f*walkingSpeed);
	else if (key == 'a') // arrow left
//		transX-=1.0;
		camObj.strafeCamera(-1.4f*walkingSpeed);
	else if (key == 's') // arrow down
//		transZ-=1.0;
		camObj.moveCamera(-1.4f*walkingSpeed);
	else if (key == 'd') // arrow right
//		transX+=1.0;
		camObj.strafeCamera(1.4f*walkingSpeed);
	else if (key == 'l') // toggle lightsource visibility
		showLightSource = !showLightSource;
	else if (key == '-')
		camObj.setRoll(camObj.getRoll()+0.05f);
	else if (key == '+')
		camObj.setRoll(camObj.getRoll()-0.05f);
	else if (key == 'r') {
		if (wireFrame)
			glPolygonMode(GL_FRONT, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireFrame = !wireFrame;
	}
	else if (key == 't') {
		if (vboSwitch){
			vboMan->switchOffVBO("defaultGrp");
		}
		else{
			vboMan->switchOnVBO("defaultGrp");
		}
		vboSwitch= !vboSwitch;
	}

}

void
processMouse(int button, int state, int x, int y)
{
	oldPosX = x;
	oldPosY = y;

	if (state == GLUT_DOWN){
		if (button == GLUT_LEFT_BUTTON){
//			mouseLastX = x;
//			mouseLastY = y;
		}
		else if (button == GLUT_RIGHT_BUTTON){
            oldPosX = x;
            oldPosY = y;
		}
	}
}

void processMWheel(int wheel, int direction, int x, int y){
//	zoom += (0.06f * direction);
//	glutPostRedisplay();
//	if (direction < 0)
//		walkingSpeed *= (0.006f);
//	else if (direction > 0)
//		walkingSpeed /= (0.006f);
	walkingSpeed += (0.06f*direction);
}
void processMouseActiveMotion(int x, int y){
	// setting the window coordinate-sys to
	/*          +y
	 *          |
	 *     -x---+---+x
	 *          |
	 *          -y
	 */
	float angleY = (float)(oldPosX - x) / 200.0f;
	float angleZ = (float)(oldPosY - y) / 200.0f;


    camObj.mouseRotate(angleY, angleZ);
    oldPosX = x;
    oldPosY = y;
//    glutWarpPointer(CenterX, CenterY);



//	mouseRotX += 0.5f*(x-mouseLastX);
//	mouseRotY += 0.5f*(y-mouseLastY);
//	mouseLastX = x; mouseLastY = y;
}
void processMousePassiveMotion(int x, int y){

}
void processMouseEntry(int state){
}

void idle(){
	glutPostRedisplay();
}

void animator(int time){

	glutPostRedisplay();
	glutTimerFunc(1, animator, 0);
}

void loadDirectory(fs::path _path)
{
	if (!fs::exists(_path)) {
		cerr << "The path " << _path << " does not exist!" << endl;
		exit(0);
	}
	else if (!fs::is_directory(_path)) {
		cerr << "The path " << _path << " is not a directory!" << endl;
		exit(0);
	}
	else if (!fs::exists(_path / "colortable.bin")) {
		cerr << "No colortable found in " << _path << "!" << endl;
		exit(0);
	}
	VboManager* vboMan = VboManager::getSingleton();
	ColorTable ct(_path / "colortable.bin");
	vboMan->setColorTable(ct);

	fs::directory_iterator dir_iter(_path), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		// iter contains an entry in the starting path _p - hopefully a directory
		if (is_directory(dir_iter->status())) {
			moWri->readModel(dir_iter->path(), ct);
		}
	}
}

void loadDirectory(fs::path _path, const ColorTable& _ct)
{
	if (!fs::exists(_path)) {
		cerr << "The path " << _path << " does not exist!" << endl;
		exit(0);
	}
	else if (!fs::is_directory(_path)) {
		cerr << "The path " << _path << " is not a directory!" << endl;
		exit(0);
	}
	VboManager* vboMan = VboManager::getSingleton();
	vboMan->setColorTable(_ct);

	fs::directory_iterator dir_iter(_path), dir_end;
	for (; dir_iter != dir_end; ++dir_iter) {
		// iter contains an entry in the starting path _p - hopefully a directory
		if (is_directory(dir_iter->status())) {
			moWri->readModel(dir_iter->path(), _ct);
		}
	}
}

static void glInit(int argc, char *argv[]){
	//readObj("meshes/cow.obj");
	GET_GLERROR(0);
	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitWindowSize(w,h);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("ModelConverter");
	glewInit();
	glutDisplayFunc(display);
//    glutKeyboardFunc(CheckAlphaKeys);
//    glutSpecialFunc(CheckKeys);
	glutKeyboardFunc(processNormalKeys);
	glutReshapeFunc(changeSize);
	//glutIdleFunc(idle);
	glutTimerFunc(5, animator, 0);

	//adding here the mouse processing callbacks
	glutMouseFunc(processMouse);
	glutMouseWheelFunc(processMWheel);
//	glutMotionFunc(CheckMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutEntryFunc(processMouseEntry);
    // To enable moving without holding down the mousebutton,
    // uncomment below.
//    glutPassiveMotionFunc(CheckMouse);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
//	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	setupTexture();
	GET_GLERROR(0);

//		vboMan->makeVbos(model);
		vboMan->makeVbos(model2);



//	oh.readOctreeRecursive(fs::path("/media/ClemensHDD/B3_octree_ausschnitt"), oct2);
//	loadDirectory(fs::path("/tmp/model"), *ct);
//	moWri->recursiveReadRawVbos(fs::path("/media/Titanstab/B3_ausschnitt"));
	Vbo* _vbo = moWri->readRawVbo(fs::path("/media/Titanstab/B3_ausschnitt/DPA-E294W5100S02-DD-1.2/294W5110-(_)(6)_S294W502-6634_S294W502-6634(1)_Geometry_0_2"));
	_vbo->setOnline();
	vboMan->addVbo("1", _vbo);
	_vbo = moWri->readRawVbo(fs::path("/media/Titanstab/B3_ausschnitt/DPA-E294W5100S02-DD-1.2/294W5110-(_)(5)_S294W502-6634_S294W502-6634(1)_Geometry_0_2"));
	_vbo->setOnline();
	vboMan->addVbo("2", _vbo);
	_vbo = moWri->readRawVbo(fs::path("/media/Titanstab/B3_ausschnitt/DPA-E294W5100S03--D-1.3/294W5110-(_)(4)_S294W510-(_)_S294W510-6634(1)_Geometry_0_2"));
	_vbo->setOnline();
	vboMan->addVbo("3", _vbo);
	_vbo = moWri->readRawVbo(fs::path("/media/Titanstab/B3_ausschnitt/DPA-E294W5100S03--D-1.3/294W5110-(_)(3)_S294W510-(_)_S294W510-6634(1)_Geometry_0_2"));
	_vbo->setOnline();
	vboMan->addVbo("4", _vbo);
	_vbo = moWri->readRawVbo(fs::path("/media/Titanstab/B3_ausschnitt/DPA-E294W5100S01-HD-1.2/294W5110-(_)(4)_S294W510-(_)_S294W510-6634(1)_Geometry_0_2"));
	_vbo->setOnline();
	vboMan->addVbo("5", _vbo);
	_vbo = moWri->readRawVbo(fs::path("/media/Titanstab/B3_ausschnitt/DPA-E294W5100S01-HD-1.2/294W5110-(_)(3)_S294W510-(_)_S294W510-6634(1)_Geometry_0_2"));
	_vbo->setOnline();
	vboMan->addVbo("6", _vbo);

//	vboMan->addVbo("blablubb", RawModelHandler::readRawVbo(fs::path("/tmp/model/145W5813-1(1)_145W5813-1(1)_Geometry_0_1/")));
//	loadDirectory(fs::path("/media/ClemensHDD/B3_ausschnitt_klein"));
//	moWri->readModel(fs::path("/media/ClemensHDD/B3_ausschnitt/DPA-E232W1201S11--D-11/232W1250-88_Geometry_0"), *ct);
//	moWri->readModel(fs::path("raw_objs/budda")); // 3,2 Mil. / 1,08 Mil.
//	moWri->readModel(fs::path("raw_objs/dragon")); //2,6 Mil. / 871k
//	moWri->readModel(fs::path("raw_objs/armadillo")); // 1,03 Mil. / 345k
//	moWri->readModel(fs::path("raw_objs/MyMini/mini_obj2")); //912k / 304k
//	moWri->readModel(fs::path("raw_objs/mini")); //912k / 304k
//	moWri->readModel(fs::path("raw_objs/bunny")); //48k / 16k
//	moWri->readModel(fs::path("raw_objs/shadow")); //30k / 10k
//	moWri->readModel(fs::path("raw_objs/door")); //25k / 8k
//	moWri->readModel(fs::path("raw_objs/beethoven")); //15k / 5k
//	moWri->readModel(fs::path("raw_objs/robot")); //3,6k / 1,2k
//	vboMan->debugSplit(bb1);
//	vboMan->mergeDown();

	vboMan->printInfo();

	GET_GLERROR(0);

	// Cg ....
	CgToolkit::getSingleton()->initCG(true);
	getGlError(0);
	g_cgVertexProg = CgToolkit::getSingleton()->loadCgShader(
			CgToolkit::getSingleton()->cgVertexProfile,
			"shader/vp_phongFLut.cg", true);

	g_cgFragmentProg = CgToolkit::getSingleton()->loadCgShader(
			CgToolkit::getSingleton()->cgFragProfile,
			"shader/fp_phongFLut.cg", true);
	g_cgGlobalAmbient = cgGetNamedParameter(g_cgFragmentProg, "globalAmbient");
	cgGLSetParameter3fv(g_cgGlobalAmbient, myGlobalAmbient);
	getGlError(0);
	g_cgLightColor = cgGetNamedParameter(g_cgFragmentProg, "lightColor");
	cgGLSetParameter3fv(g_cgLightColor, myLightColor);
	g_cgLightPosition = cgGetNamedParameter(g_cgFragmentProg, "lightPosition");
	g_cgEyePosition = cgGetNamedParameter(g_cgFragmentProg, "eyePosition");
	g_cgKe = cgGetNamedParameter(g_cgFragmentProg, "Ke");
	g_cgKa = cgGetNamedParameter(g_cgFragmentProg, "Ka");
	getGlError(0);
	g_cgKd = cgGetNamedParameter(g_cgFragmentProg, "Kd");
	g_cgKs = cgGetNamedParameter(g_cgFragmentProg, "Ks");
	g_cgShininess = cgGetNamedParameter(g_cgFragmentProg, "shininess");

	vboMan->getColorTable().setupTexture();
	vboMan->getColorTable().setCgParams(g_cgFragmentProg);
	vboMan->getColorTable().bindTex();
	vboMan->getColorTable().debug();
//	ct->setupTexture();
//	ct->setCgParams(g_cgVertexProg);
//
//	ct->bindTex();

	camObj.positionCamera(0.0,0.0,5.0,
			0.0,0.0,-3.0,
			0,1,0);

	setBrassMaterial();
	GET_GLERROR(0);

	// endCg .....
	vboMan->setCgDiffParam(g_cgKd);
	// using just this causes all glColor calls to ignored and be set to orange ..ooOO( -- )
//	fbo = FboFactory::getInstance()->createCompleteFbo(w,h);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glutMainLoop(); // Infinite event loop

}

int main(int argc, char **argv) {
// theory: parse obj into Model* - then write it with Phase1ModelWriter
// then delete model;

	sbb = BoundingBox::fromFile(fs::path(BOUNDING_BOX));
	oct2 = new Octree(sbb, string(""));
	ct = new ColorTable(string(COLOR_TABLE));
	moLoader.setColorTable(ColorTable(string(COLOR_TABLE)));
	moWri = new RawModelHandler();
	cout << "SceneBoundingBox: " << sbb.toString() << endl;
	lpo = new LooseProcessingOctree(0, sbb, "/");
	lpo->generateEmptyTree(4);
//	model = moLoader.parseMultipass("/media/External/B3_triangles/Part1/C141T4001S01-BD-1V4.obj", true);
//	model = moLoader.parseMultipass("meshes/osg.obj", true);
//	model = moLoader.parseMultipass("meshes/robot.obj", true);
	model2 = moLoader.parseMultipass("/home-e/ava/Diplom/Model/bunny.obj", true);
//	model = moLoader.parseMultipass("meshes/happy_buddha.obj", true);
//	model = moLoader.parseMultipass("meshes/Dragon.obj", true);
//	model = moLoader.parseMultipass("meshes/mini_obj2.obj", true);
//	model = moLoader.parseMultipass("meshes/ShadowBattleCrab.obj", true);
//	model = moLoader.parseMultipass("meshes/Armadillo.obj", true);
//	model = moLoader.parseMultipass("meshes/beethoven_tri3.obj", true);
//	ColorTable ct = model->getColorTable();
//	ct.inflateToPowerOfTwo();
//	ct.debug();
//	ct.removePowerOfTwoOverhead();
//	ct.debug();
//	exit(0);
//	bb1 = new BoundingBox(2.0f, -2.0f);
//	bb1 = new BoundingBox(0.0f, -2.0f);
//	bb1 = new BoundingBox(V3f(103.773, -715.835, 0), V3f(1355.28, -354.488, 1200.71));
//	bb2 = new BoundingBox(V3f(989.783, -182.595, -380.283), V3f(991.252, -175.79, -370.003));
//	cout << "intersecting, are they? " << bb1->intersects(*bb2) << endl;
//	unsigned char* testStream = new unsigned char[28];
//	((unsigned int*)testStream)[0] = 8;
//	testStream[4] = 255;
//	testStream[5] = 0;
//	testStream[6] = 0;
//
//	testStream[7] = 0;
//	testStream[8] = 255;
//	testStream[9] = 0;
//
//	testStream[10] = 0;
//	testStream[11] = 0;
//	testStream[12] = 255;
//
//	testStream[13] = 255;
//	testStream[14] = 255;
//	testStream[15] = 0;
//
//	testStream[16] = 143;
//	testStream[17] = 33;
//	testStream[18] = 236;
//
//	testStream[19] = 255;
//	testStream[20] = 0;
//	testStream[21] = 255;
//
//	testStream[22] = 0;
//	testStream[23] = 255;
//	testStream[24] = 255;
//
//	testStream[25] = 143;
//	testStream[26] = 33;
//	testStream[27] = 236;
//
//	ct = new ColorTable((char*)testStream);
//	ct->addColor(new V3ub(255,0,0));
//	ct->addColor(new V3ub(0,255,0));
//	ct->addColor(new V3ub(0,0,255));
//	ct->addColor(new V3ub(255,255,0));

	tri1 = V3f(0.5f, 1.8f, 1.5f);
	tri2 = V3f(0.0f, 1.0f, 1.5f);
	tri3 = V3f(1.5f, 0.7f, 1.5f);
//	cout << "does the bunny intersect with the other box? " << bb1->intersects(*model2->getBB()) << endl;
//	cout << "intersecting? " << bb1->intersects(tri1, tri2, tri3) << endl;
//	return 0;
//	model->calculateNormals();

	vboMan = VboManager::getSingleton();


//	modelScale = 1.0f/((fabs(model->getMaxBB()->getX()) +	fabs(model->getMinBB()->getX()))/2.0);
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getY()) + fabs(model->getMinBB()->getY()))/2.0));
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getZ()) + fabs(model->getMinBB()->getZ()))/2.0));
//	cout << "ModelScale: " << modelScale << endl;
		//fabs(model->getFirstObj()->maxV.x) + fabs(model.getLastObj()->minV.x);

//	exit(0);



	glInit(argc, argv);

	return 0;
}


