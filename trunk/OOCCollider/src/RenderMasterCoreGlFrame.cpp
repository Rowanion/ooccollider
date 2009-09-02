/**
 * @file	RenderMasterCoreGlFrame.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderMasterCoreGlFrame class declaration.
 */

#include "RenderMasterCoreGlFrame.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <cmath>
#include <iostream>
#include <cstring>

#include "GeometricOps.h"
#include "V3f.h"
#include "EventManager.h"
#include "MouseDraggedEvent.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "ModelViewMatrixEvent.h"
#include "MpiControl.h"
#include "KeyPressedEvent.h"
#include "Message.h"
#include "MouseWheelEvent.h"
#include "FboFactory.h"
#include "ColorBufferEvent.h"

#include <X11/Xlib.h>
#include <spnav.h>

using namespace std;
using namespace ooctools;
using namespace oocframework;

RenderMasterCoreGlFrame::RenderMasterCoreGlFrame() :
	nearPlane(0.1f), farPlane(3200.0f), scale(1.0f), avgFps(0.0f), time(0.0), frame(0), mPriVboMan(0), mPriCgt(0), mPriIVbo(0), walkingSpeed(0.3f),
	mPriFbo(0), mPriColorBuffer(0), mPriCBufWidth(0), mPriCBufHeight(0), mPriCBufX(0), mPriCBufY(0), mPriUseSpaceNav(false){
	// TODO Auto-generated constructor stub

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}
	mPriVboMan = VboManager::getSingleton();
	mPriVboMan->setColorTable(ColorTable(string("/media/ClemensHDD/colortable.bin")));
	mPriCgt = CgToolkit::getSingleton();
	myGlobalAmbient[0] = 0.1f;
	myGlobalAmbient[1] = 0.1f;
	myGlobalAmbient[2] = 0.1f;
	myLightColor[0] = 0.95f;
	myLightColor[1] = 0.95f;
	myLightColor[2] = 0.95f;

	// cam setup
	xmove = 0.0;
	ymove = 0.0;
	zmove = 0.0;

	myXRot = 0.0f;
	myYRot = 0.0f;
	myZRot = 0.0f;

	myTranslateMatrix = new float[16];
	myGLRotMatrix = new float[16];

	first = true;
	myQuatRotMat  = new float[16];
	myTempMat = new float[16];
	localQuat = Quaternion();
	totalQuat = Quaternion();

	oocframework::EventManager::getSingleton()->addListener(this, MouseDraggedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseButtonEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseWheelEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, ColorBufferEvent::classid());


}

RenderMasterCoreGlFrame::~RenderMasterCoreGlFrame() {
	oocframework::EventManager::getSingleton()->removeListener(this, MouseDraggedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseButtonEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseWheelEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, ColorBufferEvent::classid());

	delete[] myTranslateMatrix;
	delete[] myGLRotMatrix;
	delete mPriFbo;

}

void RenderMasterCoreGlFrame::init() {
	//compute frustum
	frustum = vector<vector<float> > ();
	frustum1 = vector<float> ();
	frustum2 = vector<float> ();
	frustum3 = vector<float> ();
	frustum4 = vector<float> ();
	frustum5 = vector<float> ();
	frustum6 = vector<float> ();

	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//	glEnable(GL_CULL_FACE);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	GET_GLERROR(0);
	// Cg ....
	setupCg();
	mPriVboMan->getColorTable().setupTexture();
	mPriVboMan->getColorTable().setCgParams(g_cgFragmentProg);
	mPriVboMan->getColorTable().bindTex();
	mPriVboMan->getColorTable().debug();
	camObj.positionCamera(0.0,0.0,5.0,
			0.0,0.0,-3.0,
			0,1,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);
	mPriVboMan->setCgDiffParam(g_cgKd);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	initMatrices();
	mPriFbo = FboFactory::getSingleton()->createCompleteFbo(640, 480);

	cout << "TexID: " << mPriFbo->getColorTexId()<< endl;
	cout << "TexID: " << mPriFbo->getDepthTexId()<< endl;
	cout << "TexID: " << mPriFbo->getId()<< endl;

	if(spnav_open()==-1) {
	  	fprintf(stderr, "failed to connect to the space navigator daemon\n");
	  	mPriUseSpaceNav = true;
	}

}

void RenderMasterCoreGlFrame::setupCg()
{
	mPriCgt->initCG(true);

}

void RenderMasterCoreGlFrame::display()
{
	static GLint T0 = 0;
	static GLint Frames = 0;
//	resizeWindow(height, width);

	if (mPriButtonActions[0] == GLFW_PRESS && mPriButtonActions[1] == GLFW_PRESS){
    	zmove -= (0.1*walkingSpeed);
    	mPriCamHasMoved = true;
	}
	else if (mPriButtonActions[1] == GLFW_PRESS){
    	zmove += (0.1*walkingSpeed);
    	mPriCamHasMoved = true;
	}

	// light blue
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	calcMatrix();
	char* data = new char[16*sizeof(float)];
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)data);
	Message* msg = new Message(ModelViewMatrixEvent::classid()->getShortId(),16*sizeof(float),1,data);
	MpiControl::getSingleton()->push(msg);
//
//	double f = glfwGetTime();  // Time (in seconds)
	glPushMatrix();
		glPushMatrix();
			camObj.applyToGL();
			glPushMatrix();
				if (mPriColorBuffer!=0){
//					FboFactory::getSingleton()->drawColorToFb(mPriColorBuffer, mPriCBufX, mPriCBufY, mPriCBufWidth, mPriCBufHeight);
					cout << "rendering the colorbuffer" << endl;
					cout << "display cbuf: " << (int64_t)mPriColorBuffer << endl;

//					static bool isFirst = true;
//
//					if(isFirst){
//					   glMatrixMode(GL_MODELVIEW);
//					   glLoadIdentity();
//					   glMatrixMode(GL_PROJECTION);
//					   glLoadIdentity();
//					   glOrtho(0, 640, 0, 480, -1, 1);
//					   isFirst = false;
//					}
//				//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//					   glRasterPos2i(0, 0);
//					   GLubyte *test = new GLubyte[640*480*4];
//					   for (unsigned i=0; i< 640*480*4; i+=8){
//						   test[i] = 0xff;
//						   test[i+1] = 0xff;
//						   test[i+2] = 0xff;
//						   test[i+3] = 0xff;
//
//					   }
//					   glDrawPixels(640,480,GL_BGRA, GL_UNSIGNED_BYTE, (GLubyte*)test);
//					   delete[] test;
//					   glMatrixMode(GL_MODELVIEW);
//					GET_GLERROR(0);
//					glfwSwapBuffers();

					glEnable(GL_TEXTURE_2D);
					GLuint tex;
					glGenTextures(1, &tex);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex);
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 640, 480, 0, GL_BGRA, GL_UNSIGNED_BYTE, mPriColorBuffer);


					glMatrixMode (GL_MODELVIEW);
					glPushMatrix ();
						glLoadIdentity ();
						glMatrixMode (GL_PROJECTION);
						glPushMatrix ();
							glLoadIdentity ();
							glBegin (GL_QUADS);
								glTexCoord2f(0.0f, 0.0f); glVertex3i (-1, -1, -1);
								glTexCoord2f(1.0f, 0.0f); glVertex3i (1, -1, -1);
								glTexCoord2f(1.0f, 1.0f); glVertex3i (1, 1, -1);
								glTexCoord2f(0.0f, 1.0f); glVertex3i (-1, 1, -1);
							glEnd ();
						glPopMatrix ();
						glMatrixMode (GL_MODELVIEW);
					glPopMatrix ();

					glDisable(GL_TEXTURE_2D);
					glDeleteTextures(1, &tex);

				}
//				mPriCgt->startCgShader(mPriCgt->cgVertexProfile, g_cgVertexProg);
//				mPriCgt->startCgShader(mPriCgt->cgFragProfile, g_cgFragmentProg);
//				mPriFbo->bind();
//				mPriFbo->clear();
//
//				if (mPriIVbo != 0){
//					mPriIVbo->managedDraw();
//				}
//				// DRAW IT
//				glBegin(GL_LINES);
//				glVertex4f(1556.15, -260.363, -965.504, mPriVboMan->getColorTable().calculateTexCoord(31));
//				glVertex4f(0.0, 0.0, 0.0, mPriVboMan->getColorTable().calculateTexCoord(31));
//				glEnd();
//

			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

//	mPriFbo->drawAsQuad();

	double t = glfwGetTime();  // Time (in seconds)
	double diff = t-time;
	fps[frame%100] = 1.0/diff;
	time = t;
	++frame;
	if (frame>=100)
		frame = 0;
	calcFPS();
	pollSpaceNav();
}

void RenderMasterCoreGlFrame::reshape(int width, int height) {
	cout << "Window resized to: " << width << ", " << height << endl;
	cout << "SIZE CHANGED" << endl;
	if (mPriFbo != 0){
		mPriFbo->setDimensions(width, height);
	}
	int w = width;
	int h = height;
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
	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
}

void RenderMasterCoreGlFrame::setVbo(IndexedVbo* iVbo)
{
	mPriIVbo = iVbo;
}

void RenderMasterCoreGlFrame::resizeWindow() {
	this->resizeWindow(0, height, 0, width);
}

void RenderMasterCoreGlFrame::resizeWindow(unsigned _height, unsigned _width) {
	this->resizeWindow(0, _height, 0, _width);
}

void RenderMasterCoreGlFrame::resizeWindow(unsigned topLine, unsigned tilesheight,
		unsigned leftLine, unsigned tileswidth) {
	if (tilesheight == 0)
		tilesheight = 1;
	if (tileswidth == 0)
		tileswidth = 1;
	worldTopLine = (GLdouble) topLine / (GLdouble) height;
	worldBottomLine = (GLdouble) (topLine + tilesheight) / (GLdouble) height;
	worldLeftLine = (GLdouble) leftLine / (GLdouble) width;
	worldRightLine = (GLdouble) (leftLine + tileswidth) / (GLdouble) width;

	glViewport(0, 0, (GLint) tileswidth, (GLint) tilesheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	worldTopLine = screenYMin + ((screenYMax - screenYMin) * worldTopLine);
	worldBottomLine = screenYMin
			+ ((screenYMax - screenYMin) * worldBottomLine);

	worldLeftLine = screenYMinH + ((screenYMaxH - screenYMinH) * worldLeftLine);
	worldRightLine = screenYMinH + ((screenYMaxH - screenYMinH)
			* worldRightLine);

	glFrustum(worldLeftLine, worldRightLine, worldTopLine, worldBottomLine,
			nearPlane, farPlane);

	glMatrixMode(GL_MODELVIEW);
}

void RenderMasterCoreGlFrame::debug() {
	scale *= 2.0f;
	cout << "debug!" << endl;
}

void RenderMasterCoreGlFrame::calcFPS() {
	avgFps = 0.0f;
	for (unsigned i = 0; i < 10; ++i) {
		avgFps += fps[i];
	}
	avgFps /= 10.0f;

}

void RenderMasterCoreGlFrame::setColorBuffer(int x, int y, int width, int height, GLubyte* buffer){
	if (mPriColorBuffer == 0 || mPriCBufHeight!=height || mPriCBufWidth!=width){
		delete[] mPriColorBuffer;
		mPriColorBuffer = new GLubyte[height * width];
		mPriCBufHeight = height;
		mPriCBufWidth = width;
		mPriCBufX = x;
		mPriCBufY = y;
		memcpy(mPriColorBuffer, buffer, sizeof(GLubyte)*width*height);
	}
}

/* MYPART BEGIN */

void RenderMasterCoreGlFrame::initMatrices(){
	myTranslateMatrix[0]  = 1.0f;
	myTranslateMatrix[1]  = 0;
	myTranslateMatrix[2]  = 0;
	myTranslateMatrix[3]  = 0;

	myTranslateMatrix[4]  = 0;
	myTranslateMatrix[5]  = 1;
	myTranslateMatrix[6]  = 0;
	myTranslateMatrix[7]  = 0;

	myTranslateMatrix[8]  = 0;
	myTranslateMatrix[9]  = 0;
	myTranslateMatrix[10] = 1;
	myTranslateMatrix[11] = 0;

	myTranslateMatrix[12] = 0;
	myTranslateMatrix[13] = 0;
	myTranslateMatrix[14] = 0;
	myTranslateMatrix[15] = 1;

	myGLRotMatrix[0]  = 1;
	myGLRotMatrix[1]  = 0;
	myGLRotMatrix[2]  = 0;
	myGLRotMatrix[3]  = 0;

	myGLRotMatrix[4]  = 0;
	myGLRotMatrix[5]  = 1;
	myGLRotMatrix[6]  = 0;
	myGLRotMatrix[7]  = 0;

	myGLRotMatrix[8]  = 0;
	myGLRotMatrix[9]  = 0;
	myGLRotMatrix[10] = 1;
	myGLRotMatrix[11] = 0;

	myGLRotMatrix[12] = 0;
	myGLRotMatrix[13] = 0;
	myGLRotMatrix[14] = 0;
	myGLRotMatrix[15] = 1;
}

void RenderMasterCoreGlFrame::calcMatrix(){
	localQuat.fromEulerAngles(myYRot, myZRot, myXRot);
	totalQuat.reset();
	totalQuat = localQuat * totalQuat;
	totalQuat.getRotationMatrix(myQuatRotMat);

	multMatrix(myQuatRotMat, myGLRotMatrix, myTempMat);

        myTranslateMatrix[12] = xmove;
        myTranslateMatrix[13] = ymove;
        myTranslateMatrix[14] = zmove;

	multMatrix(myTranslateMatrix, myTempMat, myGLRotMatrix);

	glMultMatrixf(myGLRotMatrix);

	myXRot = myYRot = myZRot = 0.0f;
	xmove = ymove = zmove = 0.0f;
}

void RenderMasterCoreGlFrame::multMatrix(float *m1, float *m2, float *res){
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j){
			res[i*4 + j] = m1[j]*m2[i*4] + m1[j+4]*m2[i*4+1] + m1[j+8]*m2[i*4+2] + m1[j+12]*m2[i*4+3];
		}
	}
}

/* MYPART END */

void RenderMasterCoreGlFrame::normalizeFrustum() {
   frustum.clear();
   frustum1.clear();
   frustum2.clear();
   frustum3.clear();
   frustum4.clear();
   frustum5.clear();
   frustum6.clear();
   frustTemp = 0.0;

   glGetFloatv(GL_PROJECTION_MATRIX, proj);
   glGetFloatv(GL_MODELVIEW_MATRIX, modl);

   clip[0]  = modl[0]  * proj[0] + modl[1]  * proj[4] + modl[2]  * proj[8]  + modl[3]  * proj[12];
   clip[1]  = modl[0]  * proj[1] + modl[1]  * proj[5] + modl[2]  * proj[9]  + modl[3]  * proj[13];
   clip[2]  = modl[0]  * proj[2] + modl[1]  * proj[6] + modl[2]  * proj[10] + modl[3]  * proj[14];
   clip[3]  = modl[0]  * proj[3] + modl[1]  * proj[7] + modl[2]  * proj[11] + modl[3]  * proj[15];

   clip[4]  = modl[4]  * proj[0] + modl[5]  * proj[4] + modl[6]  * proj[8]  + modl[7]  * proj[12];
   clip[5]  = modl[4]  * proj[1] + modl[5]  * proj[5] + modl[6]  * proj[9]  + modl[7]  * proj[13];
   clip[6]  = modl[4]  * proj[2] + modl[5]  * proj[6] + modl[6]  * proj[10] + modl[7]  * proj[14];
   clip[7]  = modl[4]  * proj[3] + modl[5]  * proj[7] + modl[6]  * proj[11] + modl[7]  * proj[15];

   clip[8]  = modl[8]  * proj[0] + modl[9]  * proj[4] + modl[10] * proj[8]  + modl[11] * proj[12];
   clip[9]  = modl[8]  * proj[1] + modl[9]  * proj[5] + modl[10] * proj[9]  + modl[11] * proj[13];
   clip[10] = modl[8]  * proj[2] + modl[9]  * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[8]  * proj[3] + modl[9]  * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8]  + modl[15] * proj[12];
   clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9]  + modl[15] * proj[13];
   clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the RIGHT plane */
   frustum1.push_back(clip[3]  - clip[0]);
   frustum1.push_back(clip[7]  - clip[4]);
   frustum1.push_back(clip[11] - clip[8]);
   frustum1.push_back(clip[15] - clip[12]);
   frustum.push_back(frustum1);

   /* Normalize the result */
   frustTemp = (float) sqrt((frustum.at(0))[0] * (frustum.at(0))[0] + (frustum.at(0))[1] * (frustum.at(0))[1] + (frustum.at(0))[2] * (frustum.at(0))[2]);
   if(frustTemp != 0){
      (frustum.at(0))[0] /= frustTemp;
      (frustum.at(0))[1] /= frustTemp;
      (frustum.at(0))[2] /= frustTemp;
      (frustum.at(0))[3] /= frustTemp;
   }
   /* Extract the numbers for the LEFT plane */
   frustum2.push_back(clip[3]  + clip[0]);
   frustum2.push_back(clip[7]  + clip[4]);
   frustum2.push_back(clip[11] + clip[8]);
   frustum2.push_back(clip[15] + clip[12]);
   frustum.push_back(frustum2);

   /* Normalize the result */
   frustTemp = (float) sqrt((frustum.at(1))[0] * (frustum.at(1))[0] + (frustum.at(1))[1] * (frustum.at(1))[1] + (frustum.at(1))[2] * (frustum.at(1))[2]);
   if(frustTemp != 0){
      (frustum.at(1))[0] /= frustTemp;
      (frustum.at(1))[1] /= frustTemp;
      (frustum.at(1))[2] /= frustTemp;
      (frustum.at(1))[3] /= frustTemp;
   }
   /* Extract the BOTTOM plane */
   frustum3.push_back((clip[3]  + clip[1]) );
   frustum3.push_back((clip[7]  + clip[5]) );
   frustum3.push_back((clip[11] + clip[9]) );
   frustum3.push_back((clip[15] + clip[13]));
   frustum.push_back(frustum3);

   /* Normalize the result */
   frustTemp = (float) sqrt((frustum.at(2))[0] * (frustum.at(2))[0] + (frustum.at(2))[1] * (frustum.at(2))[1] + (frustum.at(2))[2] * (frustum.at(2))[2]);
   if(frustTemp != 0){
      (frustum.at(2))[0] /= frustTemp;
      (frustum.at(2))[1] /= frustTemp;
      (frustum.at(2))[2] /= frustTemp;
      (frustum.at(2))[3] /= frustTemp;
   }
   /* Extract the TOP plane */
   frustum4.push_back(clip[3]  - clip[1]);
   frustum4.push_back(clip[7]  - clip[5]);
   frustum4.push_back(clip[11] - clip[9]);
   frustum4.push_back(clip[15] - clip[13]);
   frustum.push_back(frustum4);

   /* Normalize the result */
   frustTemp = (float) sqrt((frustum.at(3))[0] * (frustum.at(3))[0] + (frustum.at(3))[1] * (frustum.at(3))[1] + (frustum.at(3))[2] * (frustum.at(3))[2]);
   if(frustTemp != 0){
      (frustum.at(3))[0] /= frustTemp;
      (frustum.at(3))[1] /= frustTemp;
      (frustum.at(3))[2] /= frustTemp;
      (frustum.at(3))[3] /= frustTemp;
   }

   /* Extract the BACK plane */
   frustum5.push_back(clip[3]  - clip[2]);
   frustum5.push_back(clip[7]  - clip[6]);
   frustum5.push_back(clip[11] - clip[10]);
   frustum5.push_back(clip[15] - clip[14]);
   frustum.push_back(frustum5);

   /* Normalize the result */
   frustTemp = (float) sqrt((frustum.at(4))[0] * (frustum.at(4))[0] + (frustum.at(4))[1] * (frustum.at(4))[1] + (frustum.at(4))[2] * (frustum.at(4))[2]);
   if(frustTemp != 0){
      (frustum.at(4))[0] /= frustTemp;
      (frustum.at(4))[1] /= frustTemp;
      (frustum.at(4))[2] /= frustTemp;
      (frustum.at(4))[3] /= frustTemp;
   }
   /* Extract the FRONT plane */
   frustum6.push_back(clip[3]  + clip[2]);
   frustum6.push_back(clip[7]  + clip[6]);
   frustum6.push_back(clip[11] + clip[10]);
   frustum6.push_back(clip[15] + clip[14]);
   frustum.push_back(frustum6);

   frustTemp = (float) sqrt((frustum.at(5))[0] * (frustum.at(5))[0] + (frustum.at(5))[1] * (frustum.at(5))[1] + (frustum.at(5))[2] * (frustum.at(5))[2]);
   if(frustTemp != 0){
      (frustum.at(5))[0] /= frustTemp;
      (frustum.at(5))[1] /= frustTemp;
      (frustum.at(5))[2] /= frustTemp;
      (frustum.at(5))[3] /= frustTemp;
   }
}

//void RenderMasterCoreGlFrame::setColorBuffer(const GLubyte* pixel, int x, int y, int width, int height){
//	memcpy(mPriColorBuffer, pixel, sizeof(GLubyte)*width*height*4);
//}

void RenderMasterCoreGlFrame::pollSpaceNav()
{
	if (mPriUseSpaceNav){
		spnav_event sev;
		if(spnav_poll_event(&sev)) {
			if(sev.type == SPNAV_EVENT_MOTION) {
				printf("got motion event: t(%d, %d, %d) ", sev.motion.x, sev.motion.y, sev.motion.z);
				printf("r(%d, %d, %d)\n", sev.motion.rx, sev.motion.ry, sev.motion.rz);
			} else {	/* SPNAV_EVENT_BUTTON */
				printf("got button %s event b(%d)\n", sev.button.press ? "press" : "release", sev.button.bnum);
			}
		}
	}
}

void RenderMasterCoreGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(MouseDraggedEvent::classid())){
		MouseDraggedEvent& mde = (MouseDraggedEvent&)event;
		cout << "MouseDragged: " << mde.getX() << ", " << mde.getY() << endl;
		myXRot = (oldPosY - mde.getY());
		myYRot = (oldPosX - mde.getX());

	    oldPosX = mde.getX();
	    oldPosY = mde.getY();

//		float matrix[16];
//		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
//
//		ModelViewMatrixEvent mve = ModelViewMatrixEvent(mModelViewMatrix);
//		oocframework::EventManager::getSingleton()->fire(mve);
	    mPriCamHasMoved = true;
	}
	else if (event.instanceOf(MouseButtonEvent::classid())){
		MouseButtonEvent& mbe = (MouseButtonEvent&)event;
		if (mPriButtonActions[mbe.getButton()] != mbe.getState()){
			mPriButtonActions[mbe.getButton()] = mbe.getState();

//			if (mbe.getButton() == 0 && mPriButtonActions[1] == GLFW_RELEASE){
				oldPosX = mbe.getX();
				oldPosY = mbe.getY();
//			}

		}

	}
	if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& mde = (KeyPressedEvent&)event;
	    switch (mde.getKey()) {
			case GLFW_KEY_PAGEUP: // tilt up
				ymove -= 0.1;
				break;

			case GLFW_KEY_PAGEDOWN: // tilt down
				ymove += 0.1;
			break;

			case GLFW_KEY_UP: // walk forward (bob head)
				myXRot = 1.0;
			break;

			case GLFW_KEY_DOWN: // walk back (bob head)
				myXRot = -1.0;
			break;

			case GLFW_KEY_LEFT: // look left(int)
				myYRot = 1.0;
			break;

			case GLFW_KEY_RIGHT: // look right
				myYRot = -1.0;
			break;
		    case (int)'W':
		    	zmove += (0.1*walkingSpeed);
		    	break;
		    case (int)'S':
		    	zmove -= (0.1*walkingSpeed);
		        break;
		    case (int)'A':
		    	xmove += (0.1*walkingSpeed);
		        break;
		    case (int)'D':
		    	xmove -= (0.1*walkingSpeed);
		        break;
		    case (int)'Q':
		    	myZRot = 1.0;
		        break;
		    case (int)'E':
		    	myZRot = -1.0;
		        break;
			default:
			break;
		}
//		ModelViewMatrixEvent mve = ModelViewMatrixEvent(mModelViewMatrix);
//		oocframework::EventManager::getSingleton()->fire(mve);
	    mPriCamHasMoved = true;
	}
	else if (event.instanceOf(MouseWheelEvent::classid())){
		MouseWheelEvent& mwe = (MouseWheelEvent&)event;
		if (mwe.getDirection() == MouseWheelEvent::UP){
			walkingSpeed += (0.6f);
		}
		else if  (mwe.getDirection() == MouseWheelEvent::DOWN){
			walkingSpeed -= (0.6f);
		}
	}
	else if (event.instanceOf(ColorBufferEvent::classid())){
		ColorBufferEvent& cbe = (ColorBufferEvent&)event;
		if (mPriColorBuffer==0){
			mPriColorBuffer = new GLubyte[cbe.getHeight()*cbe.getWidth()*4];
			cout << "der Master hat nen colorbuffer initialisiert...." << cbe.getHeight() << " " << cbe.getWidth() << endl;
			cout << "pre init cbuf: " << (int64_t)mPriColorBuffer << endl;
//			for (unsigned i=0; i<cbe.getHeight()*cbe.getWidth()*4; i+=4){
//				mPriColorBuffer[i] = 255;
//				mPriColorBuffer[i+1] = 0;
//				mPriColorBuffer[i+2] = 0;
//				mPriColorBuffer[i+3] = 255;

	//		}
			cout << "post init cbuf: " << (int64_t)mPriColorBuffer << endl;
		}
//		exit(0);
		memcpy(mPriColorBuffer, cbe.getPixel(), cbe.getHeight()*cbe.getWidth()*sizeof(GLubyte)*4);
	}

}
