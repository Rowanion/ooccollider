//============================================================================
// Name        : ModelViewer.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

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
#include "ModelWriter.h"
#include "RawModelWriter.h"
#include "ColorTable.h"
#include "AbstractVertex.h"


using namespace std;
using namespace ooctools;
using namespace oocformats;


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

BoundingBox *bb1;
BoundingBox *bb2;
VboManager *vboMan;
Fbo *fbo;
ColorTable* ct;
GLuint tex;

RawModelWriter *moWri = new RawModelWriter();

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
CGparameter g_cgModelViewProj;
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

void setupTexture()
{
	unsigned char pixelData[] = {255, 0,0,0,255,0,0,0,255};
	glGenTextures(1, &tex);
	GET_GLERROR(0);

	glBindTexture(GL_TEXTURE_1D, tex);
	GET_GLERROR(0);

// stretch the texture in all dimensions to the edge of the face
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D,
		GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GET_GLERROR(0);

// this actually binds the scalar values as texture to the texture-name
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16, 3,
		0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
	GET_GLERROR(0);
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

static void renderSpacedBitmapString(
		float x,
		float y,
		int spacing,
		void *font,
		string s) {
	string::iterator sIt;
	int x1=x;
	for (sIt = s.begin(); sIt!=s.end(); ++sIt) {
		glRasterPos2f(x1,y);
		glutBitmapCharacter(font, (*sIt));
		x1 = x1 + glutBitmapWidth(font,(*sIt)) + spacing;
	}
}

static void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
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
	renderSpacedBitmapString(30,35,1,font,s.str());
	glPopMatrix();
	resetPerspectiveProjection();

}

static void display() {
	glClearColor(0.0f,0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	lpos[0] = (float)(g_dLightRadius*cos(g_iFrame*3.14f/200.0f));
	lpos[1] = (float)(g_dLightRadius*sin(g_iFrame*3.14f/200.0f));
	lpos[2] = g_dLightHeight;
	lpos[3] = 1.0f;
	cgGLSetParameter3fv(g_cgLightPosition,lpos);
	cgGLSetParameter3fv(g_cgEyePosition,eyePosition);


	glPushMatrix();
		glTranslatef(transX, 0.0, transZ);
		glPushMatrix();

			glScalef(zoom, zoom, zoom);
			glScalef(modelScale,modelScale,modelScale);
			glRotatef(mouseRotX, 1.0f, 0.0, 0.0);
			glRotatef(mouseRotY, 0.0f, 1.0, 0.0);
			glPushMatrix();
				if (showLightSource){
					glEnable(GL_POINT_SPRITE);
					glPointSize(10.0f);
					glColor3f(1.0f, 1.0f, 0.0f);
					glBegin(GL_POINTS);
						glVertex3fv(lpos);
					glEnd();
					glDisable(GL_POINT_SPRITE);
				}
				if (showBBox) {
					vboMan->getBb()->draw(0,255,0);
					vboMan->drawBbs(255,0,0);
				}
//				vboMan->drawNormals();
				cgGLSetStateMatrixParameter(g_cgModelViewProj,CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
				cgGLSetStateMatrixParameter(g_cgModelViewInv,CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_INVERSE);

				// enable profiles
				glColor3f(0.0f, 1.0f, 0.0f);
				bb1->draw(1.0f, 0.0f, 0.0f);

				CgToolkit::getInstancePtr()->startCgShader(CgToolkit::getInstancePtr()->cgVertexProfile, g_cgVertexProg);
				CgToolkit::getInstancePtr()->startCgShader(CgToolkit::getInstancePtr()->cgFragProfile, g_cgFragmentProg);
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
//				bb2->draw(0.0f, 1.0f, 0.0f);

//				glActiveTexture(GL_TEXTURE0);
//				glBindTexture(GL_TEXTURE_1D, tex);
//				GET_GLERROR(0);
//				glEnable(GL_TEXTURE_1D);
//				GET_GLERROR(0);
//				cgGLEnableTextureParameter(cgColorLut);
//				GET_GLERROR(0);
				glBegin(GL_TRIANGLES);
//					glVertex3f(tri1.getX(), tri1.getY(), tri1.getZ());
//					glVertex3f(tri2.getX(), tri2.getY(), tri2.getZ());
//					glVertex3f(tri3.getX(), tri3.getY(), tri3.getZ());
//					glVertex3f(tri3.getX(), tri3.getY(), tri3.getZ());
//					glVertex3f(tri2.getX(), tri2.getY(), tri2.getZ());
//					glVertex3f(tri1.getX(), tri1.getY(), tri1.getZ());
					glVertex4f(tri1.getX(), tri1.getY(), tri1.getZ(), vboMan->getColorTable().calculateTexCoord(9));
					glVertex4f(tri2.getX(), tri2.getY(), tri2.getZ(), vboMan->getColorTable().calculateTexCoord(11));
					glVertex4f(tri3.getX(), tri3.getY(), tri3.getZ(), vboMan->getColorTable().calculateTexCoord(10));
					glVertex4f(tri3.getX(), tri3.getY(), tri3.getZ(), vboMan->getColorTable().calculateTexCoord(8));
					glVertex4f(tri2.getX(), tri2.getY(), tri2.getZ(), vboMan->getColorTable().calculateTexCoord(8));
					glVertex4f(tri1.getX(), tri1.getY(), tri1.getZ(), vboMan->getColorTable().calculateTexCoord(8));
				glEnd();
				vboMan->drawVbos();
//				ct->unbindTex();
//				cgGLDisableTextureParameter(cgColorLut);
//				glDisable(GL_TEXTURE_2D);
				GET_GLERROR(0);
//				fbo->bind();
//				fbo->clear();
//				vboMan->drawVbo("robot");
//				vboMan->drawVbo("bunny");
//				glGetFloatv (GL_MODELVIEW_MATRIX, mvMatrix);
//				OOCTools::readColorFB(pixels, 0,0, w,h);
//				fbo->unbind();


				CgToolkit::getInstancePtr()->stopCgShader(CgToolkit::getInstancePtr()->cgFragProfile);
				CgToolkit::getInstancePtr()->stopCgShader(CgToolkit::getInstancePtr()->cgVertexProfile);
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

void changeSize(int w, int h) {

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
	gluPerspective(45.0f, ratio, 0.01f, 200.0f);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);


	// -------------------------------------
	                     								// load the identity matrix as the modelview (resets all previous transformations)


}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27 || key == 'q') // a.k.a. quit
		exit(0);
	else if (key == 'f') // toggle show FPS
		showFPS= !showFPS;
	else if (key == 'b') // toggle shoe BoundingBoxes
		showBBox= !showBBox;
	else if (key == 'w') // arrow up
		transZ+=1.0;
	else if (key == 'a') // arrow left
		transX-=1.0;
	else if (key == 's') // arrow down
		transZ-=1.0;
	else if (key == 'd') // arrow right
		transX+=1.0;
	else if (key == 'l') // toggle lightsource visibility
		showLightSource = !showLightSource;
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

void processMouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		mouseLastX = x;
		mouseLastY = y;
	}
}

void processMWheel(int wheel, int direction, int x, int y){
	zoom += (0.06f * direction);
	glutPostRedisplay();
}
void processMouseActiveMotion(int x, int y){
	// seeting the window coordinate-sys to
	/*          +y
	 *          |
	 *     -x---+---+x
	 *          |
	 *          -y
	 */
	mouseRotY += 0.5f*(x-mouseLastX);
	mouseRotX += 0.5f*(y-mouseLastY);
	mouseLastX = x; mouseLastY = y;
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
	glutKeyboardFunc(processNormalKeys);
	glutReshapeFunc(changeSize);
	//glutIdleFunc(idle);
	glutTimerFunc(5, animator, 0);

	//adding here the mouse processing callbacks
	glutMouseFunc(processMouse);
	glutMouseWheelFunc(processMWheel);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutEntryFunc(processMouseEntry);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	setupTexture();
	GET_GLERROR(0);

	vboMan->makeVbos(model);

//	moWri->readModel(fs::path("/home/ava/leeresDir/C141T4001S01-BD-1V4")); //912k / 304k
//	moWri->readModel(fs::path("raw_objs/budda")); // 3,2 Mil. / 1,08 Mil.
//	moWri->readModel(fs::path("raw_objs/dragon")); //2,6 Mil. / 871k
//	moWri->readModel(fs::path("raw_objs/armadillo")); // 1,03 Mil. / 345k
//	moWri->readModel(fs::path("raw_objs/mini")); //912k / 304k
//	moWri->readModel(fs::path("raw_objs/bunny")); //48k / 16k
//	moWri->readModel(fs::path("raw_objs/shadow")); //30k / 10k
//	moWri->readModel(fs::path("raw_objs/door")); //25k / 8k
//	moWri->readModel(fs::path("raw_objs/beethoven")); //15k / 5k
//	moWri->readModel(fs::path("raw_objs/robot")); //3,6k / 1,2k
	vboMan->printInfo();

	GET_GLERROR(0);

	// Cg ....

	CgToolkit::getInstancePtr()->initCG(true);
	getGlError(0);
	g_cgVertexProg = CgToolkit::getInstancePtr()->loadCgShader(CgToolkit::getInstancePtr()->cgVertexProfile, "shader/vp_phongFLut.cg", true);
	g_cgModelViewProj = cgGetNamedParameter(g_cgVertexProg, "modelViewProj");

	g_cgFragmentProg = CgToolkit::getInstancePtr()->loadCgShader(CgToolkit::getInstancePtr()->cgFragProfile, "shader/fp_phongFLut.cg", true);
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
	g_cgModelViewInv = cgGetNamedParameter(g_cgFragmentProg, "modelViewInv");

	vboMan->getColorTable().setupTexture();
	vboMan->getColorTable().setCgParams(g_cgFragmentProg);
	vboMan->getColorTable().bindTex();
	vboMan->getColorTable().debug();
//	ct->setupTexture();
//	ct->setCgParams(g_cgVertexProg);
//
//	ct->bindTex();

	setBrassMaterial();
	GET_GLERROR(0);

	// endCg .....
	vboMan->setCgDiffParam(g_cgKd);
	fbo = FboFactory::getInstance()->createCompleteFbo(w,h);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glutMainLoop(); // Infinite event loop

}

int main(int argc, char *argv[]) {
// theory: parse obj into Model* - then write it with Phase1ModelWriter
// then delete model;
	moLoader.setColorTable(new ColorTable(string("/media/External/B3x7/Farben/colortable.bin")));
//	model = moLoader.parseMultipass("/media/External/B3_triangles/Part1/C141T4001S01-BD-1V4.obj", true);
//	model = moLoader.parseMultipass("meshes/osg.obj", true);
//	model = moLoader.parseMultipass("meshes/robot.obj", true);
	model = moLoader.parseMultipass("meshes/bunny.obj", true);
//	model = moLoader.parseMultipass("meshes/happy_buddha.obj", true);
//	model = moLoader.parseMultipass("meshes/Dragon.obj", true);
//	model = moLoader.parseMultipass("meshes/mini_obj2.obj", true);
//	model = moLoader.parseMultipass("meshes/ShadowBattleCrab.obj", true);
//	model = moLoader.parseMultipass("meshes/Armadillo.obj", true);
//	model = moLoader.parseMultipass("meshes/beethoven_tri3.obj", true);
	ColorTable ct = model->getColorTable();
//	ct.inflateToPowerOfTwo();
	ct.debug();
//	ct.removePowerOfTwoOverhead();
//	ct.debug();
//	exit(0);
	bb1 = new BoundingBox(2.0f, 1.0f);
//	bb2 = new BoundingBox(0.0f, -2.5333333f);
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
	cout << "intersecting? " << bb1->intersects(tri1, tri2, tri3) << endl;
	V3f center;
	bb1->computeCenter(center);
	cout << (center.toString()) << endl;
//	model->calculateNormals();

	vboMan = VboManager::getInstancePtr();


//	modelScale = 1.0f/((fabs(model->getMaxBB()->getX()) +	fabs(model->getMinBB()->getX()))/2.0);
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getY()) + fabs(model->getMinBB()->getY()))/2.0));
//	modelScale = fmax(modelScale, 1.0f/((fabs(model->getMaxBB()->getZ()) + fabs(model->getMinBB()->getZ()))/2.0));
//	cout << "ModelScale: " << modelScale << endl;
		//fabs(model->getFirstObj()->maxV.x) + fabs(model.getLastObj()->minV.x);

//	exit(0);



	glInit(argc, argv);

	return 0;
}


