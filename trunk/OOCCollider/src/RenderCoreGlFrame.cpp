/**
 * @file	RenderCoreGlFrame.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderCoreGlFrame class declaration.
 */

#include "RenderCoreGlFrame.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>

#include "GeometricOps.h"
#include "V3f.h"
#include "EventManager.h"
#include "MpiControl.h"
#include "KeyPressedEvent.h"
#include "Message.h"
#include "FboFactory.h"
#include "ColorBufferEvent.h"
#include "DepthBufferEvent.h"
#include "NodeRequestEvent.h"
#include "VboEvent.h"
#include "KillApplicationEvent.h"
#include "EndTransmissionEvent.h"
#include "ModelViewMatrixEvent.h"

using namespace std;
using namespace ooctools;
using namespace oocframework;

#define MAX_OFFLINE_VBOS 1000
#define MAX_LOADS_PER_FRAME 80
#define DEPTHBUFFER_INTERVAL 50
#define BASE_MODEL_PATH "/home/ava/Diplom/Model/Octree"

#define GET_GLERROR(ret) \
{ \
         GLenum err = glGetError(); \
         if (err != GL_NO_ERROR) { \
                 fprintf(stderr, "[%s line %d] GL Error: %s\n", \
                 __FILE__, __LINE__, gluErrorString(err)); \
                 fflush(stderr); \
         } \
}

RenderCoreGlFrame::RenderCoreGlFrame() :
	nearPlane(0.1f), farPlane(3200.0f), scale(1.0f), avgFps(0.0f), time(0.0),
			frame(0), mPriVboMan(0), mPriCgt(0), eyePosition(ooctools::V3f()),
			mPriCamHasMoved(false), mPriIVbo(0), mPriFbo(0),
			mPriWindowWidth(0), mPriWindowHeight(0), mPriPixelBuffer(0),
			mPriDepthBuffer(0), mPriTriCount(0), priFrustum(0), mPriIdPathMap(
					std::map<uint64_t, std::string>()), mPriMissingIdsInFrustum(std::set<uint64_t>()),
					mPriObsoleteVbos(std::vector< std::map<uint64_t, ooctools::IndexedVbo*>::iterator >()),
			mPriUseWireFrame(false),
			mPriRequestedVboList(std::set<uint64_t>()), mPriFarClippingPlane(
					100.0f) {

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}
	mPriVboMan = VboManager::getSingleton();
	mPriVboMan->setColorTable(ColorTable(string(
			"/media/ClemensHDD/colortable.bin")));
	mPriCgt = CgToolkit::getSingleton();
	myGlobalAmbient[0] = 0.1f;
	myGlobalAmbient[1] = 0.1f;
	myGlobalAmbient[2] = 0.1f;
	myLightColor[0] = 0.95f;
	myLightColor[1] = 0.95f;
	myLightColor[2] = 0.95f;

	oocframework::EventManager::getSingleton()->addListener(this,
			KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this,
			ModelViewMatrixEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this,
			VboEvent::classid());


	mPriModelViewProjMatrix = new float[16];

	this->priFrustum = new float*[6];
	for (unsigned i = 0; i < 6; ++i) {
		this->priFrustum[i] = new float[4];
	}

}

RenderCoreGlFrame::~RenderCoreGlFrame() {
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, ModelViewMatrixEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, VboEvent::classid());

	delete[] mPriModelViewProjMatrix;
	mPriModelViewProjMatrix = 0;
	delete[] mPriPixelBuffer;
	mPriPixelBuffer = 0;
	delete[] mPriDepthBuffer;
	mPriDepthBuffer = 0;
}

void RenderCoreGlFrame::init() {
	//compute frustum
	frustum = vector<vector<float> > ();
	frustum1 = vector<float> ();
	frustum2 = vector<float> ();
	frustum3 = vector<float> ();
	frustum4 = vector<float> ();
	frustum5 = vector<float> ();
	frustum6 = vector<float> ();

	glewInit();
	glDrawBuffer(GL_FRONT_AND_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
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
	mPriFbo = FboFactory::getSingleton()->createCompleteFbo(mPriWindowWidth,mPriWindowHeight);
	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));
	mPriOh.generateIdPathMap(mPriLo, mPriIdPathMap);
	mPriOh.generateIdLoMap(mPriLo, mPriIdLoMap);
//	setVbo(new IndexedVbo(fs::path("/home/ava/Diplom/Model/Octree/data/5/5/182.idx")));

	mPriSceneBB = mPriLo->getBb();
	mPriSceneBB.computeCenter(mPriSceneCenter);
}

void RenderCoreGlFrame::setupCg()
{
	mPriCgt->initCG(true);
	g_cgVertexProg = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "/media/ClemensHDD/workspace4/OOCCollider/shader/vp_phongDirectional.cg", true);

	g_cgFragmentProg = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "/media/ClemensHDD/workspace4/OOCCollider/shader/fp_phongDirectional.cg", true);
	g_cgGlobalAmbient = cgGetNamedParameter(g_cgFragmentProg, "globalAmbient");
	cgGLSetParameter3fv(g_cgGlobalAmbient, myGlobalAmbient);
	g_cgLightColor = cgGetNamedParameter(g_cgFragmentProg, "lightColor");
	cgGLSetParameter3fv(g_cgLightColor, myLightColor);
	g_cgLightPosition = cgGetNamedParameter(g_cgFragmentProg, "lightPosition");
	g_cgEyePosition = cgGetNamedParameter(g_cgFragmentProg, "eyePosition");
	g_cgKe = cgGetNamedParameter(g_cgFragmentProg, "Ke");
	g_cgKa = cgGetNamedParameter(g_cgFragmentProg, "Ka");
	g_cgKd = cgGetNamedParameter(g_cgFragmentProg, "Kd");
	g_cgKs = cgGetNamedParameter(g_cgFragmentProg, "Ks");
	g_cgShininess = cgGetNamedParameter(g_cgFragmentProg, "shininess");
	g_cgModelViewInv = cgGetNamedParameter(g_cgFragmentProg, "mvi");

}

void RenderCoreGlFrame::display()
{
	double f = glfwGetTime();  // Time (in seconds)
	static GLint T0 = 0;
	static GLint Frames = 0;
//	resizeWindow(height, width);

	// light blue
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMultMatrixf(mPriModelViewMatrix);

	ooctools::GeometricOps::calcEyePosition(mPriModelViewMatrix, eyePosition);

	getFrustum();
	mPriIdsInFrustum.clear();
	mPriLo->isInFrustum(priFrustum, &mPriIdsInFrustum, true, 0);
//	cout << "list of nodes in frustum: " << endl;
//	for(set<uint64_t>::iterator it = mPriIdsInFrustum.begin(); it!=mPriIdsInFrustum.end(); it++){
//		cout << *it << endl;
//	}
	divideIdList();
	requestMissingVbos();
//	loadMissingVbosFromDisk(&mPriIdsInFrustum, &mPriVbosInFrustum);
//	loadMissingVbosFromDisk(&mPriIdsInFrustum, &mPriVbosInFrustum2);
//	compareVbos(&mPriVbosInFrustum, &mPriVbosInFrustum2);


	cgGLSetParameter3fv(g_cgLightPosition,camObj.getEye().getData());
	cgGLSetParameter3fv(g_cgEyePosition,camObj.getEye().getData());

//	normalizeFrustum();
//	glLoadIdentity();
//	gluLookAt( // Set camera position and orientation
//			0.0, 0.0, 10.0, // Camera position (x,y,z)
//			0.0, 0.0, 0.0, // View point (x,y,z)
//			0.0, 1.0, 0.0 // Up-vector (x,y,z)
//	);
	glPushMatrix();
		glPushMatrix();
			cgGLSetStateMatrixParameter(g_cgModelViewInv,CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);
			cgGLGetMatrixParameterfr(g_cgModelViewInv,mPriModelViewProjMatrix);
			mPriEyePosition[0] = mPriModelViewProjMatrix[3];
			mPriEyePosition[1] = mPriModelViewProjMatrix[7];
			mPriEyePosition[2] = mPriModelViewProjMatrix[11];
			glPushMatrix();
				glPushMatrix();
					glLoadIdentity();
					glLightfv(GL_LIGHT0, GL_POSITION, camObj.getEye().getData());
				glPopMatrix();
				glColor3f(1.0f,0.0f,0.0f);
				mPriFbo->bind();
				mPriFbo->clear();
				mPriSceneBB.draw();
				cout << "Number of VBOs: " << mPriVbosInFrustum.size()<< endl;
//				cout << "Number of Tris being rendered: " << mPriTriCount<< endl;
//				for (std::set<uint64_t>::iterator it = mPriIdsInFrustum.begin(); it!= mPriIdsInFrustum.end(); ++it){
////					cout << "drawing box with id: " << *it << endl;
////					cout << "in in node: " << mPriIdLoMap[(*it)]->getIdString() << endl;
//					mPriIdLoMap[(*it)]->getBb().draw();
//				}

				mPriCgt->startCgShader(mPriCgt->cgVertexProfile, g_cgVertexProg);
				mPriCgt->startCgShader(mPriCgt->cgFragProfile, g_cgFragmentProg);

				if (mPriIVbo != 0){
					mPriIVbo->managedDraw();
				}
				for (std::map<uint64_t, ooctools::IndexedVbo*>::iterator it= mPriVbosInFrustum.begin(); it!=mPriVbosInFrustum.end(); ++it){
					it->second->managedDraw();
				}
				// DRAW IT
				glBegin(GL_LINES);
					glVertex4f(mPriEyePosition[1], mPriEyePosition[2], mPriEyePosition[3], mPriVboMan->getColorTable().calculateTexCoord(31));
					glVertex4f(mPriSceneCenter.getX(), mPriSceneCenter.getY(), mPriSceneCenter.getZ(), mPriVboMan->getColorTable().calculateTexCoord(31));
				glEnd();

				double t = glfwGetTime();  // Time (in seconds)


				FboFactory::getSingleton()->readColorFromFb(mPriPixelBuffer, 0, 0, mPriWindowWidth, mPriWindowHeight);
				ColorBufferEvent cbe = ColorBufferEvent(0,0,mPriWindowWidth,mPriWindowHeight,t-f, mPriPixelBuffer);

				// dumping the depth-buffer every 100 frames....
				cout << "frame: " << frame << ", camMoved? " << mPriCamHasMoved<< endl;
				if (frame == (DEPTHBUFFER_INTERVAL - 1) && mPriCamHasMoved){
					mPriCamHasMoved = false;
					FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriWindowWidth, mPriWindowHeight);
//					cout << "getting depthb...." << endl;
//					DepthBufferEvent dbe = DepthBufferEvent(0,0,mPriWindowWidth,mPriWindowHeight, mPriFbo->mapDepthBuffer());
					GET_GLERROR(0);
					DepthBufferEvent dbe = DepthBufferEvent(0,0,mPriWindowWidth,mPriWindowHeight, mPriDepthBuffer);
//					cout << "original: " << mPriWindowHeight << ", " << mPriWindowWidth << endl;
//					cout << "in event: " << dbe.getHeight() << ", " << dbe.getWidth() << endl;
					MpiControl::getSingleton()->clearOutQueue(2);
					MpiControl::getSingleton()->push(new Message(dbe, 2));
					mPriRequestedVboList.clear();

				}

			//
				Message* msg = new Message(cbe, 0);
				MpiControl::getSingleton()->push(msg);
//				MpiControl::getSingleton()->sendAll();
//				delete msg;

				mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
				mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);

				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	mPriFbo->drawAsQuad();

	double diff = t-time;
	fps[frame%10] = 1.0/diff;
	time = t;
	++frame;
	if (frame>=DEPTHBUFFER_INTERVAL){
		frame = 0;
	}
	calcFPS();
}

void RenderCoreGlFrame::reshape(int width, int height) {
	cout << "Window resized to: " << width << ", " << height << endl;
	cout << "SIZE CHANGED" << endl;

	if (mPriPixelBuffer==0|| mPriWindowWidth != width || mPriWindowHeight != height){
		delete[] mPriPixelBuffer;
		mPriPixelBuffer = new GLubyte[width*height*4];
	}
	if (mPriDepthBuffer==0|| mPriWindowWidth != width || mPriWindowHeight != height){
		delete[] mPriDepthBuffer;
		mPriDepthBuffer = new GLfloat[width*height];
	}

	mPriWindowWidth = width;
	mPriWindowHeight = height;
	if (mPriFbo != 0)
		mPriFbo->setDimensions(mPriWindowWidth, mPriWindowHeight);
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(mPriWindowHeight == 0)
		mPriWindowHeight = 1;

	float ratio = 1.0* mPriWindowWidth / mPriWindowHeight;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, (GLint)mPriWindowWidth, (GLint)mPriWindowHeight);
	//glFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 1.0f, 100.0f);					// perspective projection
	//glFrustum(-0.4f, 0.4f, -0.5f, 0.5f, 1.0f, 100.0f);					// perspective projection

	// Set the correct perspective.
	//gluPerspective(45,ratio,1,1000);
	//gluPerspective(20.0,1.0,0.5,50.0);
	gluPerspective(45.0f, ratio, 0.1f, mPriFarClippingPlane);
//	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
}

void RenderCoreGlFrame::setVbo(IndexedVbo* iVbo)
{
	mPriIVbo = iVbo;
}

void RenderCoreGlFrame::resizeWindow() {
	this->resizeWindow(0, height, 0, width);
}

void RenderCoreGlFrame::resizeWindow(unsigned _height, unsigned _width) {
	this->resizeWindow(0, _height, 0, _width);
}

void RenderCoreGlFrame::resizeWindow(unsigned topLine, unsigned tilesheight,
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

void RenderCoreGlFrame::debug() {
	scale *= 2.0f;
	cout << "debug!" << endl;
}

void RenderCoreGlFrame::calcFPS() {
	avgFps = 0.0f;
	for (unsigned i = 0; i < 10; ++i) {
		avgFps += fps[i];
	}
	avgFps /= 10.0f;

}

void RenderCoreGlFrame::normalizeFrustum() {
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

void RenderCoreGlFrame::getFrustum() {
   glGetFloatv(GL_PROJECTION_MATRIX, priProj);
   glGetFloatv(GL_MODELVIEW_MATRIX, priModl);

   for (unsigned i = 0; i < 16; i += 4) {
       priClip[i+0] = priModl[i] * priProj[0] + priModl[i+1] *
priProj[4] + priModl[i+2] * priProj[8]  + priModl[i+3] * priProj[12];
       priClip[i+1] = priModl[i] * priProj[1] + priModl[i+1] *
priProj[5] + priModl[i+2] * priProj[9]  + priModl[i+3] * priProj[13];
       priClip[i+2] = priModl[i] * priProj[2] + priModl[i+1] *
priProj[6] + priModl[i+2] * priProj[10] + priModl[i+3] * priProj[14];
       priClip[i+3] = priModl[i] * priProj[3] + priModl[i+1] *
priProj[7] + priModl[i+2] * priProj[11] + priModl[i+3] * priProj[15];
   }

   /* Extract the RIGHT plane */
   this->priFrustum[0][0] = this->priClip[3] - this->priClip[0];
   this->priFrustum[0][1] = this->priClip[7] - this->priClip[4];
   this->priFrustum[0][2] = this->priClip[11] - this->priClip[8];
   this->priFrustum[0][3] = this->priClip[15] - this->priClip[12];

   /* Extract the numbers for the LEFT plane */
   this->priFrustum[1][0] = this->priClip[3] + this->priClip[0];
   this->priFrustum[1][1] = this->priClip[7] + this->priClip[4];
   this->priFrustum[1][2] = this->priClip[11] + this->priClip[8];
   this->priFrustum[1][3] = this->priClip[15] + this->priClip[12];

   /* Extract the BOTTOM plane */
   this->priFrustum[2][0] = this->priClip[3] + this->priClip[1];
   this->priFrustum[2][1] = this->priClip[7] + this->priClip[5];
   this->priFrustum[2][2] = this->priClip[11] + this->priClip[9];
   this->priFrustum[2][3] = this->priClip[15] + this->priClip[13];

   /* Extract the TOP plane */
   this->priFrustum[3][0] = this->priClip[3] - this->priClip[1];
   this->priFrustum[3][1] = this->priClip[7] - this->priClip[5];
   this->priFrustum[3][2] = this->priClip[11] - this->priClip[9];
   this->priFrustum[3][3] = this->priClip[15] - this->priClip[13];

   /* Extract the BACK plane */
   this->priFrustum[4][0] = this->priClip[3] - this->priClip[2];
   this->priFrustum[4][1] = this->priClip[7] - this->priClip[6];
   this->priFrustum[4][2] = this->priClip[11] - this->priClip[10];
   this->priFrustum[4][3] = this->priClip[15] - this->priClip[14];

   /* Extract the FRONT plane */
   this->priFrustum[5][0] = this->priClip[3] + this->priClip[2];
   this->priFrustum[5][1] = this->priClip[7] + this->priClip[6];
   this->priFrustum[5][2] = this->priClip[11] + this->priClip[10];
   this->priFrustum[5][3] = this->priClip[15] + this->priClip[14];

   /* Normalize the results */
   for (unsigned i = 0; i < 6; ++i) {
       this->priFrustTemp = (float) sqrt(this->priFrustum[i][0] *
this->priFrustum[i][0] + this->priFrustum[i][1] * this->priFrustum[i][1]
+ this->priFrustum[i][2] * this->priFrustum[i][2]);
       if (this->priFrustTemp != 0) {
           this->priFrustum[i][0] /= this->priFrustTemp;
           this->priFrustum[i][1] /= this->priFrustTemp;
           this->priFrustum[i][2] /= this->priFrustTemp;
           this->priFrustum[i][3] /= this->priFrustTemp;
       }
   }
}

//void
//RenderCoreGlFrame::requestMissingVbos(std::set<uint64_t>* idList, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap)
//{
//
//	multimap<float, uint64_t> missingVbosMap = multimap<float, uint64_t>();
//
//	std::set<uint64_t>::iterator setIt = idList->begin();
//	std::set<uint64_t>::iterator requestIt = mPriRequestedVboList.begin();
//	std::map<uint64_t, ooctools::IndexedVbo*>::iterator vboIt = vboMap->begin();
//	std::map<uint64_t, ooctools::IndexedVbo*>::iterator eraseVboIt;
//	std::map<uint64_t, ooctools::IndexedVbo*>::iterator vboOfflineIt = mPriOfflineVbosInFrustum.begin();
//	for (; setIt != idList->end(); ++setIt){ // check each element of the frustum-list against the elements we already have
//		vboIt = vboMap->find(*setIt);
//		if (vboIt == vboMap->end()){ // id is not in list -> it needs to be loaded
//			vboOfflineIt = mPriOfflineVbosInFrustum.find(*setIt);
//			if (vboOfflineIt != mPriOfflineVbosInFrustum.end()){ // missing element is just offline -> move to online
//				vboOfflineIt->second->setOnline();
//				vboMap->insert(*vboOfflineIt);
//				mPriOfflineVbosInFrustum.erase(vboOfflineIt);
//				vboOfflineIt = mPriOfflineVbosInFrustum.end();
//			}
//			else { // missing element needs to be requested
//				// compute eye distance
//				ooctools::V3f center = ooctools::V3f();
//				mPriIdLoMap[*setIt]->getBb().computeCenter(center);
//				missingVbosMap.insert(make_pair(eyePosition.calcSimpleDistance(center), *setIt));
//			}
//		}
//	}
//
//	setIt = idList->begin();
//	for (vboIt = vboMap->begin(); vboIt != vboMap->end(); ++vboIt){ // check each element in current vboList if it is also in the newList
//		setIt = idList->find(vboIt->first);
//		if (setIt == idList->end()){ // element in currentList is not in frustum list -> remove it
//			vboIt->second->setOffline();
//			if (mPriOfflineVbosInFrustum.size()>MAX_OFFLINE_VBOS){ // cachelist is overfull -> reduce by one
//				//TODO remove the element with highest eye-distance
//				delete mPriOfflineVbosInFrustum.begin()->second;
//				mPriOfflineVbosInFrustum.erase(mPriOfflineVbosInFrustum.begin());
//			}
//			mPriOfflineVbosInFrustum.insert(*vboIt);
//			eraseVboIt = vboIt++;
//			vboMap->erase(eraseVboIt);
//			if (vboIt == vboMap->end()){
//				break;
//			}
//		}
//	}
//
//	cerr << "------------------------total size of request-list: " << missingVbosMap.size() << endl;
//	multimap<float, uint64_t> reducedList = multimap<float, uint64_t>();
//	multimap<float, uint64_t>::iterator multiMapIt = missingVbosMap.begin();
//	unsigned requestCount =0;
//	for(; multiMapIt != missingVbosMap.end(); ++multiMapIt){
//		if (requestCount <= MAX_LOADS_PER_FRAME){
//			requestIt = mPriRequestedVboList.find(multiMapIt->second);
//			if (requestIt == mPriRequestedVboList.end()){ // element is not already requested -> make it so
//				// tag this node as requested until next DepthBuffer-update
//				mPriRequestedVboList.insert(multiMapIt->second);
//				reducedList.insert(*multiMapIt);
//				requestCount++;
//			}
//		}
//		else{
//			break;
//		}
//	}
//	cerr << "------------------------requestcount: " << requestCount << endl;
//	cerr << "------------------------sent size of request-list: " << reducedList.size() << endl;
//
//	// requesting and receiving of the missing VBOs
//	if (requestCount>0){
//		NodeRequestEvent nre = NodeRequestEvent(reducedList, MpiControl::getSingleton()->getRank());
//		MpiControl::getSingleton()->push(new Message(nre, 2));
//		MpiControl::getSingleton()->isend();
//	}
//
//}

void
RenderCoreGlFrame::requestMissingVbos()
{
//TODO umbau auf die zusatz methode und maps
	multimap<float, uint64_t> missingVbosMap = multimap<float, uint64_t>();

	std::set<uint64_t>::iterator setIt = mPriIdsInFrustum.begin();
	std::set<uint64_t>::iterator requestIt = mPriRequestedVboList.begin();
	std::map<uint64_t, ooctools::IndexedVbo*>::iterator vboIt = mPriVbosInFrustum.begin();
	std::map<uint64_t, ooctools::IndexedVbo*>::iterator eraseVboIt;
	std::map<uint64_t, ooctools::IndexedVbo*>::iterator vboOfflineIt = mPriOfflineVbosInFrustum.begin();
	for (; setIt != mPriIdsInFrustum.end(); ++setIt){ // check each element of the frustum-list against the elements we already have
		vboIt = mPriVbosInFrustum.find(*setIt);
		if (vboIt == mPriVbosInFrustum.end()){ // id is not in list -> it needs to be loaded
			vboOfflineIt = mPriOfflineVbosInFrustum.find(*setIt);
			if (vboOfflineIt != mPriOfflineVbosInFrustum.end()){ // missing element is just offline -> move to online
				vboOfflineIt->second->setOnline();
				mPriVbosInFrustum.insert(*vboOfflineIt);
				mPriOfflineVbosInFrustum.erase(vboOfflineIt);
				vboOfflineIt = mPriOfflineVbosInFrustum.end();
			}
			else { // missing element needs to be requested
				// compute eye distance
				ooctools::V3f center = ooctools::V3f();
				mPriIdLoMap[*setIt]->getBb().computeCenter(center);
				missingVbosMap.insert(make_pair(eyePosition.calcSimpleDistance(center), *setIt));
			}
		}
	}

	setIt = mPriIdsInFrustum.begin();
	for (vboIt = mPriVbosInFrustum.begin(); vboIt != mPriVbosInFrustum.end(); ++vboIt){ // check each element in current vboList if it is also in the newList
		setIt = mPriIdsInFrustum.find(vboIt->first);
		if (setIt == mPriIdsInFrustum.end()){ // element in currentList is not in frustum list -> remove it
			vboIt->second->setOffline();
			if (mPriOfflineVbosInFrustum.size()>MAX_OFFLINE_VBOS){ // cachelist is overfull -> reduce by one
				//TODO remove the element with highest eye-distance
				delete mPriOfflineVbosInFrustum.begin()->second;
				mPriOfflineVbosInFrustum.erase(mPriOfflineVbosInFrustum.begin());
			}
			mPriOfflineVbosInFrustum.insert(*vboIt);
			eraseVboIt = vboIt++;
			mPriVbosInFrustum.erase(eraseVboIt);
			if (vboIt == mPriVbosInFrustum.end()){
				break;
			}
		}
	}

	cerr << "------------------------total size of request-list: " << missingVbosMap.size() << endl;
	multimap<float, uint64_t> reducedList = multimap<float, uint64_t>();
	multimap<float, uint64_t>::iterator multiMapIt = missingVbosMap.begin();
	unsigned requestCount =0;
	for(; multiMapIt != missingVbosMap.end(); ++multiMapIt){
		if (requestCount <= MAX_LOADS_PER_FRAME){
			requestIt = mPriRequestedVboList.find(multiMapIt->second);
			if (requestIt == mPriRequestedVboList.end()){ // element is not already requested -> make it so
				// tag this node as requested until next DepthBuffer-update
				mPriRequestedVboList.insert(multiMapIt->second);
				reducedList.insert(*multiMapIt);
				requestCount++;
			}
		}
		else{
			break;
		}
	}
	cerr << "------------------------requestcount: " << requestCount << endl;
	cerr << "------------------------sent size of request-list: " << reducedList.size() << endl;

	// requesting and receiving of the missing VBOs
	if (requestCount>0){
		NodeRequestEvent nre = NodeRequestEvent(reducedList, MpiControl::getSingleton()->getRank());
		MpiControl::getSingleton()->push(new Message(nre, 2));
		MpiControl::getSingleton()->isend();
	}

}

void
RenderCoreGlFrame::loadMissingVbosFromDisk(std::set<uint64_t>* idList, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap)
{
	// change list to set - auto-sorted
	// we look for two cases:
		// nodes we haven't loaded yet
		// nodes we don't need anymore
	bool term = false;
	if (vboMap->size() >0) term = true;
//	cout << "ids in frustum: " << idList->size() << endl;
//	cout << "vbos already in map: " << vboMap->size() << endl;
	std::set<uint64_t>::iterator setIt = idList->begin();
	VboMapIter mapIt = vboMap->begin();
	VboMapIter eraseIt;
	unsigned loadCount = 0;
	cout << "list of nodes LOADED by RenderCore: " << endl;
	for(; setIt!=idList->end(); ++setIt){
		while(mapIt != vboMap->end() && *setIt > mapIt->first){
			eraseIt = mapIt; // needs to be erased
			mapIt++;
//			cout << "deleting " << "/media/ClemensHDD/Sampletree/data/"+mPriIdPathMap[eraseIt->first]+".idx" << endl;
//			cout << "because " << *setIt << " > " << eraseIt->first << endl;
			eraseIt->second->setOffline();
			VboMapIter eraseIt2 = eraseIt++;
			mPriOfflineVbosInFrustum.insert(eraseIt2, eraseIt);
//			delete eraseIt->second;
			vboMap->erase(eraseIt2);
			if (mPriOfflineVbosInFrustum.size()>MAX_OFFLINE_VBOS){
				delete mPriOfflineVbosInFrustum.begin()->second;
				mPriOfflineVbosInFrustum.erase(mPriOfflineVbosInFrustum.begin());
			}
		}
		if (mapIt == vboMap->end() || *setIt < mapIt->first){
			// needs to be inserted
//			cout << "adding " << "/media/ClemensHDD/Sampletree/data/"+mPriIdPathMap[*setIt]+".idx" << endl;
//			vboMap->insert(make_pair(*setIt, new IndexedVbo(fs::path("/media/ClemensHDD/Sampletree/data/1/3/404.idx"))));
			VboMapIter offIt = mPriOfflineVbosInFrustum.find(*setIt);
			if (offIt != mPriOfflineVbosInFrustum.end()){
				offIt->second->setOnline();
				VboMapIter offIt2 = offIt++;
				vboMap->insert(offIt2, offIt);
				mPriOfflineVbosInFrustum.erase(offIt2);
			}
			else {
				if (loadCount < MAX_LOADS_PER_FRAME){
					vboMap->insert(make_pair(*setIt, new IndexedVbo(fs::path(string(BASE_MODEL_PATH)+"/data/"+mPriIdPathMap[*setIt]+".idx"))));
					cout << *setIt << endl;
					loadCount++;
				}
				else
					break;
			}
		}
		else if(*setIt == mapIt->first){
			mapIt++;
		}
	}
//	if (term )exit(0);
}

void
RenderCoreGlFrame::divideIdList()
{
	// input: set<uint64_t> idsInFrustum, map<uint64_t, IndexedVbo*> loadedVbos
	// output set<uint64_t> missingIdsInFrustum, vector<map<uint64_t, IndexedVbo*>::iterator> obsoleteVbos
	set<uint64_t>::iterator setIt;
	set<uint64_t>::iterator reqIt;
	map<uint64_t, IndexedVbo*>::iterator mapIt;

	// iterator over idList and save every id that is not loaded in a new list
	for (setIt=mPriIdsInFrustum.begin(); setIt!=mPriIdsInFrustum.end(); ++setIt){
		mapIt = mPriVbosInFrustum.find(*setIt);
		if (mapIt == mPriVbosInFrustum.end()){
			reqIt = mPriRequestedVboList.find(*setIt);
			if (reqIt == mPriRequestedVboList.end()){
				mPriMissingIdsInFrustum.insert(*setIt);
			}
		}
	}

	//iterate over loadedVboList and save an iterator for each one that is obsolete
	for (mapIt = mPriVbosInFrustum.begin(); mapIt!= mPriVbosInFrustum.end(); ++mapIt){
		setIt = mPriIdsInFrustum.find(mapIt->first);
		if (setIt == mPriIdsInFrustum.end()){
			mPriObsoleteVbos.push_back(mapIt);
		}
	}
}

void
RenderCoreGlFrame::compareVbos(std::map<uint64_t, ooctools::IndexedVbo*>* vboMap, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap2)
{
	if (vboMap->size() != vboMap2->size()){
		cout << "Vbo-Count Miss Match!!!" << endl;
		cout << vboMap->size() << " vs " << vboMap2->size() << endl;

		unsigned exitCount = (vboMap->size() > vboMap2->size()) ? vboMap->size() : vboMap2->size();
		VboMapIter it1 = vboMap->begin();
		VboMapIter it2 = vboMap2->begin();
		for (unsigned i=0; i < exitCount; ++i){
			if (i+1 < vboMap->size()){
				cout << "ID: " << it1->first;
				it1++;
			}
			else {
				cout << "NONE    ";
			}
			cout << " \t - \t ";
			if (i+1 < vboMap2->size()){
				cout << "" << it2->first;
				it2++;
			}
			else {
				cout << "NONE    ";
			}
			cout << endl;
		}

		exit(0);
	}
	VboMapIter mapIt = vboMap->begin();
	for(; mapIt != vboMap->end(); ++mapIt){
		if (mapIt->second->getIndexCount() != (*vboMap2)[mapIt->first]->getIndexCount()){
			cout << "Index-Count Miss Match!!!" << endl;
			exit(0);
		}
		if (mapIt->second->getVertexCount() != (*vboMap2)[mapIt->first]->getVertexCount()){
			cout << "Vertex-Count Miss Match!!!" << endl;
			exit(0);
		}
		for (unsigned i=0; i < mapIt->second->getIndexCount(); ++i){
			if (mapIt->second->getIndexData()[i] != (*vboMap2)[mapIt->first]->getIndexData()[i]){
				cout << "Index-Value Miss Match!!!" << endl;
				exit(0);
			}
		}
		for (unsigned i=0; i < mapIt->second->getVertexCount(); ++i){
			for (unsigned j=0; j<4; ++j){
				if (mapIt->second->getVertexData()[i].v[j] != (*vboMap2)[mapIt->first]->getVertexData()[i].v[j] ||
						mapIt->second->getVertexData()[i].n[j] != (*vboMap2)[mapIt->first]->getVertexData()[i].n[j]){
					cout << "Vertex-Value Miss Match!!!" << endl;
					exit(0);
				}
			}
		}
	}
}

void RenderCoreGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(ModelViewMatrixEvent::classid())){
		ModelViewMatrixEvent& mve = (ModelViewMatrixEvent&)event;
//		glLoadMatrixf(mve.getMatrix());
		for (unsigned i=0; i<16; ++i){
			if (mve.getMatrix()[i] != mPriModelViewMatrix[i]){
//				exit(0);
				mPriCamHasMoved = true;
				memcpy(mPriModelViewMatrix, mve.getMatrix(), 16*sizeof(float));
				cout << "loaded matrix: " << MpiControl::getSingleton()->getRank() << endl;
				break;
			}
		}
	}
	else if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& mde = (KeyPressedEvent&)event;
	    switch (mde.getKey()) {
			case GLFW_KEY_PAGEUP: // tilt up
			break;
			case 'B': {// manually resend depth-buffer
				FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriWindowWidth, mPriWindowHeight);
				DepthBufferEvent dbe = DepthBufferEvent(0,0,mPriWindowWidth,mPriWindowHeight, mPriDepthBuffer);
				MpiControl::getSingleton()->push(new Message(dbe, 2));
				mPriRequestedVboList.clear();
			break;}
			case 'R': // switch wireframe
				if (mPriUseWireFrame)
					glPolygonMode(GL_FRONT, GL_FILL);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				mPriUseWireFrame = !mPriUseWireFrame;
			break;
			case 'F': // increase far-clipping plane
				mPriFarClippingPlane*=2.0f;
				reshape(mPriWindowWidth, mPriWindowHeight);
				mPriCamHasMoved = true;
			break;
			case 'N': // decrease far-clipping plane
				mPriFarClippingPlane = max(mPriFarClippingPlane/2.0f, 50.0f);
				reshape(mPriWindowWidth, mPriWindowHeight);
				mPriCamHasMoved = true;
			break;
			default:
			break;
		}
	}
	else if (event.instanceOf(VboEvent::classid())){
		VboEvent& ve = (VboEvent&)event;
		mPriVbosInFrustum.insert(make_pair(ve.getNodeId(), new IndexedVbo(ve.getIndexArray(), ve.getIndexCount(), ve.getVertexArray(), ve.getVertexCount())));
	}


}
