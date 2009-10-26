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
#include <cmath>
#include <sstream>

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
#include "InfoRequestEvent.h"

using namespace std;
using namespace ooctools;
using namespace oocframework;

RenderCoreGlFrame::RenderCoreGlFrame(int winWidth, int winHeight, int targetWinWidth, int targetWinHeight) :
AbstractGlFrame(winWidth, winHeight, targetWinWidth, targetWinHeight), scale(1.0f), avgFps(0.0f), time(0.0), frame(0), mPriVboMan(0), mPriCgt(0),
			mPriEyePosition(ooctools::V3f()), mPriViewVector(ooctools::V3f()), mPriCamHasMoved(false),
			mPriBBMode(0), mPriExtendedFovy(EXTENDED_FOVY), mPriAspectRatio(0.0f), mPriMaxDistPerLevel(0), mPriFbo(0),
			mPriTileYPos(0), mPriTileXPos(0), mPriTileWidth(0), mPriTileHeight(0),
			mPriPixelBuffer(0), mPriDepthBuffer(0), mPriTriCount(0),mPriColorBufferEvent(0,0,0,0,0.0,0),
			priFrustum(0), mPriIdPathMap(std::map<uint64_t, std::string>()),
			mPriMissingIdsInFrustum(std::set<uint64_t>()), mPriObsoleteVbos(
					std::vector<IdVboMapIter>()), mPriUseWireFrame(false),
			mPriRequestedVboList(std::set<uint64_t>()),
			mPriCamera(OOCCamera()), mPriRenderTimeSum(0.0), mPriShowOffset(false){

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}
	mPriVboMan = VboManager::getSingleton();
	mPriColorTable = ColorTable(string(BASE_MODEL_PATH) + string("/colortable.bin"));
	mPriVboMan->setColorTable(mPriColorTable);
	mPriCgt = CgToolkit::getSingleton();
	myGlobalAmbient[0] = 0.1f;
	myGlobalAmbient[1] = 0.1f;
	myGlobalAmbient[2] = 0.1f;
	myLightColor[0] = 0.95f;
	myLightColor[1] = 0.95f;
	myLightColor[2] = 0.95f;
	lightPos[0] = 0.0;
	lightPos[1] = 0.0;
	lightPos[2] = 5.0;

	oocframework::EventManager::getSingleton()->addListener(this,
			KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this,
			ModelViewMatrixEvent::classid() );
	oocframework::EventManager::getSingleton()->addListener(this,
			VboEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, InfoRequestEvent::classid());



	this->priFrustum = new float*[6];
	for (unsigned i = 0; i < 6; ++i) {
		this->priFrustum[i] = new float[4];
	}

	mPriDepthBufferD = 0;
	mPriDepthTexture = 0;

	mPriPixelBuffer = new GLubyte[mProWindowWidth*mProWindowHeight*4];
	mPriDepthBuffer = new GLfloat[mProWindowWidth*mProWindowHeight];

}

RenderCoreGlFrame::~RenderCoreGlFrame() {
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, ModelViewMatrixEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, VboEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, InfoRequestEvent::classid());

	delete[] mPriPixelBuffer;
	mPriPixelBuffer = 0;
	delete[] mPriDepthBuffer;
	mPriDepthBuffer = 0;
	IdVboMapIter mapIt = mPriVbosInFrustum.begin();
	for (; mapIt != mPriVbosInFrustum.end(); ++mapIt){
		delete mapIt->second;
		mapIt->second = 0;
	}
	mapIt = mPriOfflineVbosInFrustum.begin();
	for (; mapIt != mPriVbosInFrustum.end(); ++mapIt){
		delete mapIt->second;
		mapIt->second = 0;
	}
	mPriVbosInFrustum.clear();

	delete mPriLo;
	mPriLo = 0;
	delete mPriFbo;
	mPriFbo = 0;
	delete[] mPriMaxDistPerLevel;
	mPriMaxDistPerLevel = 0;
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
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glGenTextures(1, &mPriDepthTexId);
//	mPriDepthTexId = 25;
	GET_GLERROR(0);
	// Cg ....
	mPriColorTable.setupTexture();
	mPriColorTable.debug();
	setupCg();
//	camObj.positionCamera(0.0,0.0,5.0,
//			0.0,0.0,-3.0,
//			0,1,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);
	mPriVboMan->setCgDiffParam(g_cgKd);
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);

	mPriCamera.initMatrices();
	mPriFbo = FboFactory::getSingleton()->createCompleteFbo(mProWindowWidth,mProWindowHeight);
	mPriFbo->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mPriFbo->unbind();
	ooctools::FboFactory::getSingleton()->readColorFromFb(mPriPixelBuffer, 0, 0, mPriTileWidth, mPriTileHeight);
	ooctools::FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mProWindowWidth, mProWindowHeight);

	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));
	mPriOh.generateIdPathMap(mPriLo, mPriIdPathMap);
	mPriOh.generateIdLoMap(mPriLo, mPriIdLoMap);

	generateMaxDistPerLevel(14, mPriLo->getBb().computeDiameter());

	mPriSceneBB = mPriLo->getBb();
	mPriSceneBB.computeCenter(mPriSceneCenter);
	cout << "(" << MpiControl::getSingleton()->getRank() << ") render window resolution: " << mProWindowWidth << ", " << mProWindowHeight << endl;
	cout << "(" << MpiControl::getSingleton()->getRank() << ") tile: " << mPriTileXPos << ", " << mPriTileYPos << ", " << mPriTileWidth << ", " << mPriTileHeight << endl;

}

void RenderCoreGlFrame::setupCg()
{
	mPriCgt->initCG(true);
	cgVertDepthTex = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/vp_depth2color.cg", true, "");
	cgFragDepthTex = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_depth2color.cg", true, "");
//	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriDepthTexId);
	glEnable(GL_TEXTURE_2D);
	cgDepthTex = cgGetNamedParameter(cgFragDepthTex, "depthTex");
	cgGLSetTextureParameter(cgDepthTex, mPriDepthTexId);
	glDisable(GL_TEXTURE_2D);


	cgVertNoLight = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/vp_NoLightLut.cg", true, "");
	cgFragNoLight = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_NoLightLut.cg", true, "");
	cgNoLightLUT = cgGetNamedParameter(cgFragNoLight,  "colorLut");
	cgGLSetTextureParameter(cgNoLightLUT, mPriColorTable.getTextureId());

	g_cgVertexProg = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/vp_phongDirectional.cg", true, "");
	g_cgFragmentProg = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_phongDirectional.cg", true, "");
	cgFragLUT = cgGetNamedParameter(g_cgFragmentProg,  "colorLut");
	cgGLSetTextureParameter(cgFragLUT, mPriColorTable.getTextureId());

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

	cgGLSetParameter3fv(g_cgLightPosition,lightPos);
	cgGLSetParameter3fv(g_cgEyePosition,lightPos);

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
			glPushMatrix();
				glPushMatrix();
					glLoadIdentity();
					glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
				glPopMatrix();
				glColor3f(1.0f,0.0f,0.0f);
				GET_GLERROR(0);
				mPriFbo->bind();
				mPriFbo->clear();
				GET_GLERROR(0);
//				cout << "Number of VBOs: " << mPriVbosInFrustum.size()<< endl;
//				cout << "Number of Tris being rendered: " << mPriTriCount<< endl;
//				for (std::set<uint64_t>::iterator it = mPriIdsInFrustum.begin(); it!= mPriIdsInFrustum.end(); ++it){
////					cout << "drawing box with id: " << *it << endl;
////					cout << "in in node: " << mPriIdLoMap[(*it)]->getIdString() << endl;
//					mPriIdLoMap[(*it)]->getBb().draw();
//				}

				GET_GLERROR(0);

				mPriColorTable.bindTex();
				cgGLEnableTextureParameter(cgNoLightLUT);
				mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertNoLight);
				mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragNoLight);
				glDisable(GL_DEPTH_TEST);
				mPriSceneBB.draw(mPriColorTable.calculateTexCoord(29));
				glEnable(GL_DEPTH_TEST);

				glBegin(GL_LINES);
					glVertex4f(mPriEyePosition.getX()-5.0f, mPriEyePosition.getY(), mPriEyePosition.getZ(), mPriColorTable.calculateTexCoord(21));
					glVertex4f(mPriSceneCenter.getX(), mPriSceneCenter.getY(), mPriSceneCenter.getZ(), mPriColorTable.calculateTexCoord(22));

				glEnd();
				mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
				mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
				cgGLDisableTextureParameter(cgNoLightLUT);
				mPriColorTable.unbindTex();

				for (IdVboMapIter it= mPriVbosInFrustum.begin(); it!=mPriVbosInFrustum.end(); ++it){
					if (mPriBBMode == 0){
						mPriColorTable.bindTex();
						cgGLEnableTextureParameter(cgFragLUT);
						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, g_cgVertexProg);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, g_cgFragmentProg);
						it->second->managedDraw();
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
						cgGLDisableTextureParameter(cgFragLUT);
						mPriColorTable.unbindTex();
					}
					else if (mPriBBMode == 1){
						mPriColorTable.bindTex();
						cgGLEnableTextureParameter(cgFragLUT);
						cgGLEnableTextureParameter(cgNoLightLUT);
						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, g_cgVertexProg);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, g_cgFragmentProg);
						it->second->managedDraw();
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);

						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertNoLight);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragNoLight);
						mPriIdLoMap[it->first]->getBb().draw(mPriColorTable.calculateTexCoord(mPriIdLoMap[it->first]->getLevel()));
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
						cgGLDisableTextureParameter(cgNoLightLUT);
						cgGLDisableTextureParameter(cgFragLUT);
						mPriColorTable.unbindTex();
					}
					else if (mPriBBMode == 2){
						mPriColorTable.bindTex();
						cgGLEnableTextureParameter(cgNoLightLUT);
						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertNoLight);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragNoLight);
						mPriIdLoMap[it->first]->getBb().draw(mPriColorTable.calculateTexCoord(mPriIdLoMap[it->first]->getLevel()));
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
						cgGLDisableTextureParameter(cgNoLightLUT);
						mPriColorTable.unbindTex();
					}
					else if (mPriBBMode == 3){
						mPriColorTable.bindTex();
						cgGLEnableTextureParameter(cgNoLightLUT);
						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertNoLight);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragNoLight);
						mPriIdLoMap[it->first]->getBb().drawSolidTriFan(mPriColorTable.calculateTexCoord(mPriIdLoMap[it->first]->getLevel()));
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
						cgGLDisableTextureParameter(cgNoLightLUT);
						mPriColorTable.unbindTex();
					}
				}
				if (mPriBBMode>0){
					mPriColorTable.drawLegend();
				}
				GET_GLERROR(0);
				// DRAW an orientation line

				double t = glfwGetTime();  // Time (in seconds)


				FboFactory::getSingleton()->readColorFromFb(mPriPixelBuffer, 0, 0, mPriTileWidth, mPriTileHeight);
//				ColorBufferEvent cbe = ColorBufferEvent(0,0,mPriWindowWidth,mPriWindowHeight,t-f, mPriPixelBuffer);
				mPriColorBufferEvent.set(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight,t-f, mPriPixelBuffer);
				mPriRenderTimeSum += t-f;

				// dumping the depth-buffer every 100 frames....
//				cout << "frame: " << frame << ", camMoved? " << mPriCamHasMoved<< endl;
//				if (frame == (DEPTHBUFFER_INTERVAL - 1)){ // send time
//				}
//				if (frame == (DEPTHBUFFER_INTERVAL - 1) && mPriCamHasMoved){
//					GET_GLERROR(0);
//					mPriCamHasMoved = false;mPriExtendedFovy
//					FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriTileWidth, mPriTileHeight);
//					DepthBufferEvent dbe = DepthBufferEvent(mPriTileXPos,mPriTileYPos,mPriTileWidth,mPriTileHeight, mPriDepthBuffer);
//					MpiControl::getSingleton()->clearOutQueue(MpiControl::DATA);
//					Message* msg = new Message(dbe, 0, MpiControl::DATA);
//					MpiControl::getSingleton()->push(msg);
//					mPriRequestedVboList.clear();
////					setupTexture();
//				}

			//
//				Message* msg = new Message(mPriColorBufferEvent, 0);
//				MpiControl::getSingleton()->push(msg);
//				MpiControl::getSingleton()->sendAll();
//				delete msg;

				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	// restore normal frustum before drawing
	// NOTE: will be removed in final version because there is no need to visibly draw for a slave. (...in computer-scientist way of meaning.)
	reshape(mProWindowWidth,mProWindowHeight);
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	glPolygonMode(GL_FRONT, GL_FILL);
	mPriFbo->drawColorFSQuad();
//	drawDepthTex();
	glPolygonMode(GL_FRONT, polyMode);
	GET_GLERROR(0);

	double diff = t-time;
	fps[frame%10] = 1.0/diff;
	time = t;
	++frame;
	if (frame>=DEPTHBUFFER_INTERVAL){
		frame = 0;
	}
	calcFPS();
}

void RenderCoreGlFrame::reshape(int width, int height)
{
	reshape(width, height, -1.0f);
}

void RenderCoreGlFrame::reshape(int width, int height, float farPlane) {
//	cout << "Window resized to: " << width << ", " << height << endl;
//	cout << "SIZE CHANGED" << endl;


	mProWindowWidth = width;
	mProWindowHeight = height;
	if (mPriFbo != 0)
		mPriFbo->setDimensions(mProWindowWidth, mProWindowHeight);
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(mProWindowHeight == 0)
		mProWindowHeight = 1;

	mPriAspectRatio = 1.0* mProWindowWidth / mProWindowHeight;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, (GLint)mProWindowWidth, (GLint)mProWindowHeight);
	//glFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 1.0f, 100.0f);					// perspective projection
	//glFrustum(-0.4f, 0.4f, -0.5f, 0.5f, 1.0f, 100.0f);					// perspective projection

	// Set the correct perspective.
	//gluPerspective(45,ratio,1,1000);
	//gluPerspective(20.0,1.0,0.5,50.0);
	if (farPlane == -1.0f){
		gluPerspective(45.0f, mPriAspectRatio, mProNearClippingPlane, mProFarClippingPlane);
	}
	else{
		gluPerspective(45.0f, mPriAspectRatio, 0.1f, farPlane);
	}
//	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);

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

void
RenderCoreGlFrame::requestMissingVbos()
{
	unsigned frustReq =0;
	unsigned extFrustReq =0;
	oocformats::LooseOctree* currentNode = 0;
	// deleting obsolete vbos
	for (unsigned i=0; i<mPriObsoleteVbos.size(); ++i){
		mPriObsoleteVbos[i]->second->setOffline();
		mPriOfflineVbosInFrustum.insert(make_pair(mPriObsoleteVbos[i]->first, mPriObsoleteVbos[i]->second));
		//TODO research why getTriCount() seems to malfunction
		mPriTriCount -= mPriObsoleteVbos[i]->second->getTriCount();

		mPriVbosInFrustum.erase(mPriObsoleteVbos[i]);
	}
	mPriObsoleteVbos.clear();
	trimCacheMap();

	// ----------------------------------------------
	// check if any of the new VBOs is already in the cache
	set<ooctools::Quadruple> missingQuadruple = set<ooctools::Quadruple>();
	IdSetIter setIt = mPriMissingIdsInFrustum.begin();
	IdVboMapIter offIt;
	ooctools::V3f center = ooctools::V3f();
	unsigned reqCount = 0;
	for (; setIt!=mPriMissingIdsInFrustum.end() && reqCount < MAX_LOADS_PER_FRAME; ++setIt){
		offIt = mPriOfflineVbosInFrustum.find(*setIt);
		if (offIt == mPriOfflineVbosInFrustum.end()){ // not in cache -> needs to be requested
			//calculate eye distances of missing vbos
			ooctools::V3f center = ooctools::V3f();
			currentNode = mPriIdLoMap[*setIt];
			currentNode->getBb().computeCenter(center);
			missingQuadruple.insert(Quadruple(currentNode->getLevel(), mPriEyePosition.calcDistance(center), MpiControl::getSingleton()->getRank(), *setIt));
			mPriRequestedVboList.insert(*setIt);
			reqCount++;
		}
		else { // VBO is in cache -> flip
			offIt->second->setOnline();
			mPriTriCount += offIt->second->getTriCount();
			mPriVbosInFrustum.insert(make_pair(offIt->first, offIt->second));
			mPriOfflineVbosInFrustum.erase(offIt);
		}
	}

	if (missingQuadruple.size() > 0){
		NodeRequestEvent nre = NodeRequestEvent(missingQuadruple, MpiControl::getSingleton()->getRank(), false);
		MpiControl::getSingleton()->isend(new Message(nre, 0));
		mPriMissingIdsInFrustum.clear();
		frustReq = nre.getIdxCount();
	}

	// now dealing with extended frustum
	mPriMissingIdsInFrustum.clear();
	missingQuadruple.clear();
	uniqueElements(mPriIdsInFrustum, mPriIdsInExtFrustum, mPriMissingIdsInFrustum);
	stripDoublesFromRight(mPriOfflineVbosInFrustum, mPriMissingIdsInFrustum);
	stripDoublesFromRight(mPriRequestedVboList, mPriMissingIdsInFrustum);

	if (mPriMissingIdsInFrustum.size()>0){
		// calc eyeDistances
		reqCount = 0;
		for (setIt = mPriMissingIdsInFrustum.begin(); setIt != mPriMissingIdsInFrustum.end() && reqCount < MAX_LOADS_PER_FRAME; ++setIt){
			ooctools::V3f center = ooctools::V3f();
			currentNode = mPriIdLoMap[*setIt];
			currentNode->getBb().computeCenter(center);
			missingQuadruple.insert(ooctools::Quadruple(currentNode->getLevel(), mPriEyePosition.calcDistance(center), MpiControl::getSingleton()->getRank(), *setIt));
			mPriRequestedVboList.insert(*setIt);
			reqCount++;
		}

		//TODO do something about the loadPerFrame limit
		NodeRequestEvent nre = NodeRequestEvent(missingQuadruple, MpiControl::getSingleton()->getRank(), true);
		MpiControl::getSingleton()->isend(new Message(nre, 0));
//		cout << "missingVbos vs requested - " << mPriMissingIdsInFrustum.size() << " vs " << nre.getIdxCount() << " vs " << missingTriple.size() << endl;
		extFrustReq = nre.getIdxCount();
		missingQuadruple.clear();
		mPriMissingIdsInFrustum.clear();

	}

	EndTransmissionEvent ete = EndTransmissionEvent();
	MpiControl::getSingleton()->send(new Message(ete, 0));
//	cout << "renderer finalized nodeRequests" << endl;

}

void
RenderCoreGlFrame::divideIdList()
{
	IdSetIter setIt, reqIt;
	IdVboMapIter mapIt;

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
RenderCoreGlFrame::uniqueElements(const std::set<uint64_t>& leftSet, const std::set<uint64_t>& rightSet, std::set<uint64_t>& uniqueSet)
{
	CIdSetIter leftIt = leftSet.begin();
	CIdSetIter rightIt = rightSet.begin();
	IdSetIter uniqueIt = uniqueSet.begin();

	// 1 2 3
	// 1 2 3 4
	bool debug = false;
//	if (MpiControl::getSingleton()->getRank() == 1) debug = true;

	while (leftIt != leftSet.end() && rightIt != rightSet.end()){
		if (debug) cout << "comparing elements " << (*leftIt) << " and " << (*rightIt) << endl;
		if (*leftIt < *rightIt){
			if (debug) cout << *leftIt << " l < r " << *rightIt << endl;
			uniqueSet.insert(uniqueIt, *leftIt);
			uniqueIt++;
			leftIt++;
		}
		else if (*leftIt > *rightIt){
			if (debug) cout << *rightIt << " r < l " << *leftIt << endl;
			uniqueSet.insert(uniqueIt, *rightIt);
			uniqueIt++;
			rightIt++;
		}
		else { // elements are equal
			if (debug) cout << *leftIt << " == " << *rightIt << endl;
			++rightIt;
			++leftIt;
		}
	}
	if (leftIt == leftSet.end() && rightIt != rightSet.end()){
		if (debug) cout << "L-equality insert" << endl;
		uniqueSet.insert(rightIt, rightSet.end());
	}
	else if (rightIt == rightSet.end() && leftIt != leftSet.end()){
		if (debug) cout << "R-equality insert" << endl;
		uniqueSet.insert(leftIt, leftSet.end());
	}

}

void
RenderCoreGlFrame::uniqueElements(const std::map<uint64_t, ooctools::IndexedVbo*>& leftMap, const std::set<uint64_t>& rightSet, std::set<uint64_t>& uniqueSet)
{
	CIdVboMapIter leftIt = leftMap.begin();
	CIdSetIter rightIt = rightSet.begin();
	IdSetIter uniqueIt = uniqueSet.begin();

	// 1 2 3
	// 1 2 3 4
	bool debug = false;
//	if (MpiControl::getSingleton()->getRank() == 1) debug = true;

	while (leftIt != leftMap.end() && rightIt != rightSet.end()){
		if (debug) cout << "comparing elements " << leftIt->first << " and " << (*rightIt) << endl;
		if (leftIt->first < *rightIt){
			if (debug) cout << leftIt->first << " l < r " << *rightIt << endl;
			uniqueSet.insert(uniqueIt, leftIt->first);
			uniqueIt++;
			leftIt++;
		}
		else if (leftIt->first > *rightIt){
			if (debug) cout << *rightIt << " r < l " << leftIt->first << endl;
			uniqueSet.insert(uniqueIt, *rightIt);
			uniqueIt++;
			rightIt++;
		}
		else { // elements are equal
			if (debug) cout << leftIt->first << " == " << *rightIt << endl;
			++rightIt;
			++leftIt;
		}
	}
	if (leftIt == leftMap.end() && rightIt != rightSet.end()){
		if (debug) cout << "L-equality insert" << endl;
		uniqueSet.insert(rightIt, rightSet.end());
	}
	else if (rightIt == rightSet.end() && leftIt != leftMap.end()){
		if (debug) cout << "R-equality insert" << endl;
		while(leftIt != leftMap.end()){
			uniqueSet.insert(uniqueIt, leftIt->first);
			uniqueIt++;
			leftIt++;
		}
	}

}

void
RenderCoreGlFrame::stripDoublesFromRight(const std::set<uint64_t>& leftSet, std::set<uint64_t>& rightSet)
{
	//TODO test
	// removes from right side all entries which are also in the left side
	CIdSetIter leftIt = leftSet.begin();
	CIdSetIter rightIt = rightSet.begin();

	for (; rightIt != rightSet.end();) {
		leftIt = leftSet.find(*rightIt);
	  if (leftIt != leftSet.end())
		  rightSet.erase(rightIt++);
	  else ++rightIt;
	}
}

void
RenderCoreGlFrame::stripDoublesFromRight(const std::map<uint64_t, ooctools::IndexedVbo*>& leftMap, std::set<uint64_t>& rightSet)
{
	//TODO
	// removes from right side all entries which are also in the left side
	CIdVboMapIter leftIt = leftMap.begin();
	CIdSetIter rightIt = rightSet.begin();

	for (; rightIt != rightSet.end();) {
		leftIt = leftMap.find(*rightIt);
	  if (leftIt != leftMap.end())
		  rightSet.erase(rightIt++);
	  else ++rightIt;
	}
}

void RenderCoreGlFrame::trimCacheMap()
{
	unsigned cacheSize = mPriOfflineVbosInFrustum.size();
	if (cacheSize > MAX_OFFLINE_VBOS){
		unsigned diff = cacheSize - MAX_OFFLINE_VBOS;
		multimap<float, IdVboMapIter> distMap = multimap<float, IdVboMapIter>();
		V3f center = V3f();
		IdVboMapIter mapIt = mPriOfflineVbosInFrustum.begin();
		for (; mapIt != mPriOfflineVbosInFrustum.end(); ++mapIt){
			mPriIdLoMap[mapIt->first]->getBb().computeCenter(center);
			distMap.insert(make_pair(mPriEyePosition.calcDistance(center), mapIt));
		}
		unsigned counter = 0;
		multimap<float, IdVboMapIter>::reverse_iterator rIt = distMap.rbegin();
		for (; rIt != distMap.rend() && counter < diff; rIt++, counter++){
//			mPriTriCount -= rIt->second->second->getTriCount();
			delete rIt->second->second;
			rIt->second->second = 0;
			mPriOfflineVbosInFrustum.erase(rIt->second);
		}
		distMap.clear();
	}
}

void
RenderCoreGlFrame::compareVbos(std::map<uint64_t, ooctools::IndexedVbo*>* vboMap, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap2)
{
	if (vboMap->size() != vboMap2->size()){
		cout << "Vbo-Count Miss Match!!!" << endl;
		cout << vboMap->size() << " vs " << vboMap2->size() << endl;

		unsigned exitCount = (vboMap->size() > vboMap2->size()) ? vboMap->size() : vboMap2->size();
		IdVboMapIter it1 = vboMap->begin();
		IdVboMapIter it2 = vboMap2->begin();
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
	IdVboMapIter mapIt = vboMap->begin();
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

void RenderCoreGlFrame::setupTexture()
{
	if (mPriDepthTexture == 0){
		mPriDepthTexture = new GLfloat[mProWindowWidth*mProWindowHeight*4];
	}
	if (mPriDepthBufferD == 0){
		mPriDepthBufferD = new GLfloat[mProWindowWidth*mProWindowHeight];
	}
	FboFactory::getSingleton()->readDepthFromFb(mPriDepthBufferD, 0, 0, mProWindowWidth, mProWindowHeight);
//	cout << "(render) depth at " << 62981 << ": " << mPriDepthBufferD[62981] << endl;

//	for(int i=0; i< mPriWindowWidth*mPriWindowHeight*4; i+=4){
//		mPriDepthTexture[i] = 1000.0-(mPriDepthBufferD[i/4]*1000.0);
//		mPriDepthTexture[i+1] = mPriDepthTexture[i];
//		mPriDepthTexture[i+2] = mPriDepthTexture[i];
//		mPriDepthTexture[i+3] = 255;
//	}
	GET_GLERROR(0);
//	glActiveTexture(GL_TEXTURE2);
	GET_GLERROR(0);
	glBindTexture(GL_TEXTURE_2D, mPriDepthTexId);
	GET_GLERROR(0);
	glEnable(GL_TEXTURE_2D);
	GET_GLERROR(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	GET_GLERROR(0);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, mPriWindowWidth, mPriWindowHeight, 0, GL_BGRA, GL_FLOAT, mPriDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mProWindowWidth, mProWindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, mPriDepthBufferD);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA16, mPriWindowWidth, mPriWindowHeight, 0, GL_ALPHA, GL_FLOAT, mPriDepthBufferD);
	GET_GLERROR(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void RenderCoreGlFrame::drawDepthTex()
{
	glBindTexture(GL_TEXTURE_2D, mPriDepthTexId);
	cgGLEnableTextureParameter(cgDepthTex);

	mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertDepthTex);
	mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragDepthTex);

//	glActiveTexture(GL_TEXTURE2);
	GET_GLERROR(0);
	glEnable(GL_TEXTURE_2D);

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
	mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
	mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
	cgGLDisableTextureParameter(cgDepthTex);
	glDisable(GL_TEXTURE_2D);

}

void
RenderCoreGlFrame::setTileDimensions(int xPos, int yPos, int width, int height)
{
//	cout << MpiControl::getSingleton()->getRank() << " ---> changing tiledimensions: " << xPos << ", " << yPos << ", " << width << ", " << height << endl;
	mPriTileXPos = xPos;
	mPriTileYPos = yPos;
	mPriTileWidth = width;
	mPriTileHeight = height;
//	cout << "x, y: " << mPriTileXPos << ", " << mPriTileYPos << endl;
//	cout << "width, height: " << mPriTileWidth << ", " << mPriTileHeight << endl;

}

void RenderCoreGlFrame::depthPass()
{
	resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight);
	glLoadIdentity();
	mPriCamera.setRotationMatrix(mPriModelViewMatrix);
	mPriCamera.calcMatrix();

	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	glPolygonMode(GL_FRONT, GL_FILL);
	GET_GLERROR(0);
	mPriFbo->bind();
	mPriFbo->clear();
	GET_GLERROR(0);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	for (IdVboMapIter it= mPriVbosInFrustum.begin(); it!=mPriVbosInFrustum.end(); ++it){
		it->second->managedDraw(true);
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	GET_GLERROR(0);
//	FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriTileWidth, mPriTileHeight);
//	DepthBufferEvent dbe = DepthBufferEvent(mPriTileXPos,mPriTileYPos,mPriTileWidth,mPriTileHeight, MpiControl::getSingleton()->getRank(), mPriDepthBuffer);
	FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriTileWidth+(2*mProFrustumExtensionX_px), mPriTileHeight+(2*mProFrustumExtensionY_px));
	DepthBufferEvent dbe = DepthBufferEvent(Tile(mPriTileXPos,mPriTileYPos,mPriTileWidth,mPriTileHeight, 0.0),
			mPriTileXPos,mPriTileYPos,mPriTileWidth+(2*mProFrustumExtensionX_px),mPriTileHeight+(2*mProFrustumExtensionY_px),
			MpiControl::getSingleton()->getRank(), mPriDepthBuffer);
	MpiControl::getSingleton()->clearOutQueue(MpiControl::DATA);
	Message* msg = new Message(dbe, 0, MpiControl::DATA);
	MpiControl::getSingleton()->send(msg);
//	cout << MpiControl::getSingleton()->getRank() << " has sent depthbuffer" << endl;
	mPriRequestedVboList.clear();
//	setupTexture();
	mPriFbo->unbind();
	glPolygonMode(GL_FRONT, polyMode);
	GET_GLERROR(0);

}

void RenderCoreGlFrame::cullFrustum()
{
	//TODO create lookuptable
	/*
	 * for each vector and face of the 1st boundingbox
	 *   calc the normalized Normal vectors
	 *   dot-product with view-vector
	 *   estimate the smallest (closest to -1) dot-product of them
	 *   then estimate the drawing order of the children
	 *   +-------------+
	 *   |  nw  |  ne  |
	 *   +------+------+
	 *   |  sw  |  se  |         \
	 *   +-------------+          o
	 *                           /
	 * Order would be in this case se,ne,sw,nw
	 * Need to estimate nw(etc.)-names to child-ids
	 * Need to find relation between BB min/max and child-ids
	 * normalize: v/sqrt(x^2+y^2+z^2)
	 * v1=B-A
	 * v2=C-A
	 * n = norm(v1,v2)
	 * vertexNormal = sum(adjacentFaceNormals)
	 * normalize
	 */

	//extend frustum
	resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight);

	// light blue
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);
	GET_GLERROR(0);
	glClear(GL_DEPTH_BUFFER_BIT);
	GET_GLERROR(0);
	glLoadIdentity();
	GET_GLERROR(0);
	// setting current matrix, apply to camera and recalc
	mPriCamera.setRotationMatrix(mPriModelViewMatrix);
	mPriCamera.calcMatrix();

//	GET_GLERROR(0);

	if (!ooctools::GeometricOps::calcEyePosFast(mPriModelViewMatrix, mPriEyePosition, mPriViewVector)){
		cout << "----------------------------------------- NO INVERSE!" << endl;
	}
//	GET_GLERROR(0);

	getFrustum();
	mPriIdsInExtFrustum.clear();
	mPriLo->isInFrustum_orig(priFrustum, &mPriIdsInExtFrustum, BoundingBox::getMinDotIdx(mPriViewVector), mPriEyePosition, mPriMaxDistPerLevel);

	// original frustum
	resizeFrustum(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight);


	getFrustum();
	mPriIdsInFrustum.clear();
	mPriLo->isInFrustum_orig(priFrustum, &mPriIdsInFrustum, BoundingBox::getMinDotIdx(mPriViewVector), mPriEyePosition, mPriMaxDistPerLevel);

	if (mPriShowOffset){
		resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight);
	}


	divideIdList();
	requestMissingVbos();
}

void RenderCoreGlFrame::generateMaxDistPerLevel(unsigned _maxLevel, float _originalSize)
{
	delete[] mPriMaxDistPerLevel;
	mPriMaxDistPerLevel = new float[_maxLevel+1];
	float tempSize = _originalSize;

	float halfPixels = 0.5*mProWindowWidth; // half pixel size because half frustum
	float pixelLength = tan(45.0 * (ooctools::GeometricOps::PI / 180.0))*mProNearClippingPlane;
	float pixelSize = pixelLength / halfPixels;
	float tanTheta = pixelSize / mProNearClippingPlane; // the smaller angle corresponding to length of 1 pixel

	for (unsigned i =0; i<=_maxLevel; i++){
		mPriMaxDistPerLevel[i] = tempSize/tanTheta; // the distance at which the diameter is exactly 1
		mPriMaxDistPerLevel[i] *= mPriMaxDistPerLevel[i];
		cout << "Lvl: " << i << ", size: " << tempSize << ", dist: " << mPriMaxDistPerLevel[i] << endl;
		tempSize*=0.5;
	}
}

void RenderCoreGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(ModelViewMatrixEvent::classid())){
		ModelViewMatrixEvent& mve = (ModelViewMatrixEvent&)event;
		memcpy(mPriModelViewMatrix, mve.getMatrix(), sizeof(float)*16);
	}
	else if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& mde = (KeyPressedEvent&)event;
	    switch (mde.getKey()) {
			case GLFW_KEY_PAGEUP: // tilt up
			break;
			case GLFW_KEY_KP_SUBTRACT:
				mPriFrustumExtension -= 0.1;
				break;
			case GLFW_KEY_KP_ADD:
				mPriFrustumExtension += 0.1;
				break;
			case 'R': // switch wireframe
				if (mPriUseWireFrame)
					glPolygonMode(GL_FRONT, GL_FILL);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				mPriUseWireFrame = !mPriUseWireFrame;
			break;
			case 'L':{
				mPriShowOffset = !mPriShowOffset;

			break;}
			case 'O': // switch BoundingBoxMode
				mPriBBMode++;
				if (mPriBBMode>3)
					mPriBBMode = 0;
			break;
			case 'F': // increase far-clipping plane
				mProFarClippingPlane*=2.0f;
				reshape(mProWindowWidth, mProWindowHeight);
				mPriCamHasMoved = true;
			break;
			case 'N': // decrease far-clipping plane
				mProFarClippingPlane = max(mProFarClippingPlane/2.0f, 50.0f);
				reshape(mProWindowWidth, mProWindowHeight);
				mPriCamHasMoved = true;
			break;
			default:
			break;
		}
	}
	else if (event.instanceOf(VboEvent::classid())){
		VboEvent& ve = (VboEvent&)event;

		if (ve.isExtendedFrustum()){
			for (unsigned i=0; i< ve.getVboCount(); ++i){
				mPriOfflineVbosInFrustum.insert(make_pair(ve.getNodeId(i), new IndexedVbo(ve.getIndexArray(i), ve.getIndexCount(i), ve.getVertexArray(i), ve.getVertexCount(i), false)));
				trimCacheMap();
			}
		}
		else {
			for (unsigned i=0; i< ve.getVboCount(); ++i){
				mPriTriCount += ve.getIndexCount(i)/3;
				mPriVbosInFrustum.insert(make_pair(ve.getNodeId(i), new IndexedVbo(ve.getIndexArray(i), ve.getIndexCount(i), ve.getVertexArray(i), ve.getVertexCount(i))));
			}
		}
	}
	else if (event.instanceOf(InfoRequestEvent::classid())){
		stringstream headerS;
		for (int i=0; i < MpiControl::getSingleton()->getRank()*2 + 1; ++i){
			headerS << "-";
		}
		headerS << "> (" << MpiControl::getSingleton()->getRank() << ") - ";
		cout << headerS.str() << "currently loaded tris: " << mPriTriCount << endl;
		cout << headerS.str() << "currently loaded vbos: " << mPriVbosInFrustum.size() << endl;
		cout << headerS.str() << "total requested vbos: " << mPriRequestedVboList.size() << endl;
		cout << headerS.str() << "loaded + requested: " << mPriVbosInFrustum.size()+ mPriRequestedVboList.size()<< endl;
		cout << headerS.str() << "total vbos in frustum: " << mPriIdsInFrustum.size() << endl;
		cout << headerS.str() << "VBOs in cache: " << mPriOfflineVbosInFrustum.size() << "/" << MAX_OFFLINE_VBOS<< endl;
		unsigned tCount = 0;
		unsigned iCount = 0;
		IdVboMapIter mIt = mPriVbosInFrustum.begin();
		for (; mIt!=mPriVbosInFrustum.end(); ++mIt){
			tCount += mIt->second->getTriCount();
			iCount += mIt->second->getIndexCount();
		}
		cout << headerS.str() << "loaded online tris (counted): " << tCount << endl;
		cout << headerS.str() << "loaded online indices (counted): " << iCount << endl;
		tCount = 0;
		iCount = 0;
		for (mIt = mPriOfflineVbosInFrustum.begin(); mIt!=mPriOfflineVbosInFrustum.end(); ++mIt){
			tCount += mIt->second->getTriCount();
			iCount += mIt->second->getIndexCount();
		}
		cout << headerS.str() << "loaded offline tris (counted): " << tCount << endl;
		cout << headerS.str() << "loaded offline indices (counted): " << iCount << endl;

		cout << headerS.str() << "nearPlane: " << mProNearClippingPlane << endl;
		cout << headerS.str() << "farPlane: " << mProFarClippingPlane << endl;

		cout << headerS.str() << "Tile-Dimensions: " << mPriTileXPos << ", " << mPriTileYPos << ", " << mPriTileWidth << ", " << mPriTileHeight << endl;
		cout << headerS.str() << "Frustum-Modifier: " << mPriFrustumExtension << endl;
		cout << "---------------------------------------" << endl;
	}


}
