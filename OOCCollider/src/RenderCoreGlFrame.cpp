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
#include "OcclusionRequestEvent.h"
#include "OcclusionResultsEvent.h"
#include "MemTools.h"


using namespace std;
using namespace ooctools;
using namespace oocformats;
using namespace oocframework;

bool compListFarToNear (const WrappedOcNode* _lhs, const WrappedOcNode* _rhs)
{
	return (_lhs->dist < _rhs->dist);
}


RenderCoreGlFrame::RenderCoreGlFrame(int winWidth, int winHeight, int targetWinWidth, int targetWinHeight) :
AbstractGlFrame(winWidth, winHeight, targetWinWidth, targetWinHeight), avgFps(0.0f), time(0.0), frame(0), mPriCgt(0),
			mPriEyePosition(ooctools::V3f()), mPriViewVector(ooctools::V3f()),
			mPriBBMode(0), mPriAspectRatio(0.0f), mPriMaxDistPerLevel(0), mPriFbo(0),
			mPriTileYPos(0), mPriTileXPos(0), mPriTileWidth(0), mPriTileHeight(0),
			mPriPixelBuffer(0), mPriDepthBuffer(0), mPriTriCount(0),mPriColorBufferEvent(0,0,0,0,0.0,0),
			priFrustum(0), mPriL1Cache(0), mPriL2Cache(0), mPriUseWireFrame(false),
			mPriCamera(OOCCamera()), mPriRenderTimeSum(0.0), mPriShowOffset(false),
			mPriFrameTick(0), mPriDisplayTime(0.0), mPriFrustumCullingTime(0.0), mPriRequestDataTime(0.0)
{

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}
	mPriColorTable = ColorTable(string(BASE_MODEL_PATH) + string("/colortable.bin"));
	VboManager::getSingleton()->setColorTable(mPriColorTable);
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
	oocframework::EventManager::getSingleton()->addListener(this, OcclusionResultsEvent::classid());



	this->priFrustum = new float*[6];
	for (unsigned i = 0; i < 6; ++i) {
		this->priFrustum[i] = new float[4];
	}


	mPriPixelBuffer = new GLubyte[mProWindowWidth*mProWindowHeight*4];
	mPriDepthBuffer = new GLfloat[mProWindowWidth*mProWindowHeight];

}

RenderCoreGlFrame::~RenderCoreGlFrame() {
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, ModelViewMatrixEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, VboEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, InfoRequestEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, OcclusionResultsEvent::classid());

	delete[] mPriPixelBuffer;
	mPriPixelBuffer = 0;
	delete[] mPriDepthBuffer;
	mPriDepthBuffer = 0;
	delete mPriLo;
	mPriLo = 0;
	delete mPriFbo;
	mPriFbo = 0;
	delete[] mPriMaxDistPerLevel;
	mPriMaxDistPerLevel = 0;
	glDeleteQueries(L1_CACHE_VBO_LIMIT, mPriOccQueries);
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
	glPolygonMode(GL_FRONT, GL_FILL);
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
	VboManager::getSingleton()->setCgDiffParam(g_cgKd);
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);

	mPriCamera.initMatrices();
	mPriFbo = FboFactory::getSingleton()->createCompleteFbo(mProWindowWidth,mProWindowHeight);
	mPriFbo->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mPriFbo->unbind();
	ooctools::FboFactory::getSingleton()->readColorFromFb(mPriPixelBuffer, 0, 0, mPriTileWidth, mPriTileHeight);
	ooctools::FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mProWindowWidth, mProWindowHeight);

	mPriLo = mPriOh.loadLooseRenderOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));

	mPriLo->pubTickLimit = DISTANCE_RENEWAL;
	mPriOh.generateIdLoMap(mPriLo, mPriIdLoMap);

	generateMaxDistPerLevel(14, mPriLo->getBb().computeDiameter());

	mPriSceneBB = mPriLo->getBb();
	mPriSceneBB.computeCenter(mPriSceneCenter);
	cout << "(" << MpiControl::getSingleton()->getRank() << ") render window resolution: " << mProWindowWidth << ", " << mProWindowHeight << endl;
	cout << "(" << MpiControl::getSingleton()->getRank() << ") tile: " << mPriTileXPos << ", " << mPriTileYPos << ", " << mPriTileWidth << ", " << mPriTileHeight << endl;

	glGenQueries(L1_CACHE_VBO_LIMIT, mPriOccQueries);

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
#ifdef DEBUG_RENDERLOOP
	double newTime = glfwGetTime();
#endif

	double f = glfwGetTime();  // Time (in seconds)
	if (mPriShowOffset){
		resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, 2000.0f);
	}
	else {
		resizeFrustum(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, 2000.0f);
	}

	cgGLSetParameter3fv(g_cgLightPosition,lightPos);
	cgGLSetParameter3fv(g_cgEyePosition,lightPos);

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


				// ---------------------------------
//				if (mPriFrameTick%50 == 0){
//					cerr << " -----> current ListSize: " << mPriWrapperInFrustum.size() << endl;
//				}
				unsigned onlineCount = 0;
				for (RWrapperListIter wIt = mPriWrapperInFrustum.rbegin(); wIt != mPriWrapperInFrustum.rend(); ++wIt){
					if (mPriBBMode == 0){
						mPriColorTable.bindTex();
						cgGLEnableTextureParameter(cgFragLUT);
						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, g_cgVertexProg);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, g_cgFragmentProg);
						// -------------------------------------
						switch ((*wIt)->state) {
//						case WrappedOcNode::MISSING:
//							mPriRequests.insert(Quintuple((*wIt)->octreeNode->getLevel(), (*wIt)->dist, MpiControl::getSingleton()->getRank(), (*wIt)->octreeNode->getId(), true));
//							(*wIt)->state = WrappedOcNode::REQUESTED;
//							break;
						case WrappedOcNode::RETEST_OFFLINE:
							mPriReRequestList.push_back((*wIt));
							(*wIt)->state = WrappedOcNode::REQUESTED_OFFLINE;
							break;
						case WrappedOcNode::ONLINE:
							++onlineCount;
							(*wIt)->iVbo->managedDraw();
							break;
						case WrappedOcNode::RETEST_ONLINE:
							++onlineCount;
							(*wIt)->iVbo->managedDraw();
							mPriReRequestList.push_back((*wIt));
							(*wIt)->state = WrappedOcNode::REQUESTED_ONLINE;
							break;
						case WrappedOcNode::REQUESTED_ONLINE:
							++onlineCount;
							(*wIt)->iVbo->managedDraw();
							break;
						default:
							break;

						}
						//TODO do 2-lvl caching
						// -------------------------------------
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
						switch ((*wIt)->state) {
						case WrappedOcNode::MISSING:
							mPriRequests.insert(Quintuple((*wIt)->octreeNode->getLevel(), (*wIt)->dist, MpiControl::getSingleton()->getRank(), (*wIt)->octreeNode->getId(), true));
							(*wIt)->state = WrappedOcNode::REQUESTED;
							break;
						case WrappedOcNode::RETEST_OFFLINE:
							mPriReRequestList.push_back((*wIt));
							(*wIt)->state = WrappedOcNode::REQUESTED_OFFLINE;
							break;
						case WrappedOcNode::ONLINE:
							++onlineCount;
							(*wIt)->iVbo->managedDraw();
							break;
						case WrappedOcNode::RETEST_ONLINE:
							++onlineCount;
							mPriReRequestList.push_back((*wIt));
							(*wIt)->state = WrappedOcNode::REQUESTED_ONLINE;
							(*wIt)->iVbo->managedDraw();
							break;
						case WrappedOcNode::REQUESTED_ONLINE:
							++onlineCount;
							(*wIt)->iVbo->managedDraw();
							break;
						default:
							break;

						}
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);

						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertNoLight);
						mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragNoLight);
						switch ((*wIt)->state) {
						case WrappedOcNode::ONLINE:
							(*wIt)->octreeNode->getBb().draw(mPriColorTable.calculateTexCoord((*wIt)->octreeNode->getLevel()));
							break;
						case WrappedOcNode::RETEST_ONLINE:
							(*wIt)->octreeNode->getBb().draw(mPriColorTable.calculateTexCoord((*wIt)->octreeNode->getLevel()));
							break;
						case WrappedOcNode::REQUESTED_ONLINE:
							(*wIt)->octreeNode->getBb().draw(mPriColorTable.calculateTexCoord((*wIt)->octreeNode->getLevel()));
							break;
						default:
							break;

						}
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
						switch ((*wIt)->state) {
						case WrappedOcNode::MISSING:
							mPriRequests.insert(Quintuple((*wIt)->octreeNode->getLevel(), (*wIt)->dist, MpiControl::getSingleton()->getRank(), (*wIt)->octreeNode->getId(), true));
							(*wIt)->state = WrappedOcNode::REQUESTED;
							break;
						case WrappedOcNode::RETEST_OFFLINE:
							mPriReRequestList.push_back((*wIt));
							(*wIt)->state = WrappedOcNode::REQUESTED_OFFLINE;
							break;
						case WrappedOcNode::ONLINE:
							++onlineCount;
							(*wIt)->octreeNode->getBb().draw(mPriColorTable.calculateTexCoord((*wIt)->octreeNode->getLevel()));
							break;
						case WrappedOcNode::RETEST_ONLINE:
							++onlineCount;
							mPriReRequestList.push_back((*wIt));
							(*wIt)->state = WrappedOcNode::REQUESTED_ONLINE;
							(*wIt)->octreeNode->getBb().draw(mPriColorTable.calculateTexCoord((*wIt)->octreeNode->getLevel()));
							break;
						case WrappedOcNode::REQUESTED_ONLINE:
							++onlineCount;
							(*wIt)->octreeNode->getBb().draw(mPriColorTable.calculateTexCoord((*wIt)->octreeNode->getLevel()));
							break;
						default:
							break;

						}
						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
						cgGLDisableTextureParameter(cgNoLightLUT);
						mPriColorTable.unbindTex();
					}
//					else if (mPriBBMode == 3){
//						mPriColorTable.bindTex();
//						cgGLEnableTextureParameter(cgNoLightLUT);
//						mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertNoLight);
//						mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragNoLight);
//						mPriIdLoMap[it->first]->getBb().drawSolidTriFan(mPriColorTable.calculateTexCoord(mPriIdLoMap[it->first]->getLevel()));
//						mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
//						mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
//						cgGLDisableTextureParameter(cgNoLightLUT);
//						mPriColorTable.unbindTex();
//					}

				}

				// -----------------------------------

				if (mPriBBMode>0){
					mPriColorTable.drawLegend();
				}
//				if (mPriFrameTick%50 == 0){
//					cerr << "VBOs on GPU: " << onlineCount << endl;
//				}
				GET_GLERROR(0);
				// DRAW an orientation line

				double t = glfwGetTime();  // Time (in seconds)


				FboFactory::getSingleton()->readColorFromFb(mPriPixelBuffer, 0, 0, mPriTileWidth, mPriTileHeight);
				mPriColorBufferEvent.set(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight,t-f, mPriPixelBuffer);
				mPriRenderTimeSum += t-f;

				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

#ifdef DEBUG_RENDERLOOP
	double newerTime = glfwGetTime();
	mPriDisplayTime += (newerTime-newTime);
	if (mPriFrameTick % CCOLLISION_AVG == 0){
		cout << "(" << MpiControl::getSingleton()->getRank() << ") RenderLoop took " << mPriDisplayTime/DISPLAY_AVG << " secs avg. over " << DISPLAY_AVG << " frames." << endl;
		mPriDisplayTime = 0.0;
	}
#endif

#ifdef SHOW_RENDER_IMAGE
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
#endif

	double diff = t-time;
	fps[frame%10] = 1.0/diff;
	time = t;
	++frame;
	if (frame>=DEPTHBUFFER_INTERVAL){
		frame = 0;
	}
	calcFPS();

	// ---------------------------------------
//	cerr << MpiControl::getSingleton()->getRank() << " - mPriIdLoMap: " << mPriIdLoMap.size() << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - mPriWrapperInFrustum: " << mPriWrapperInFrustum.size() << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - mPriRequests: " << mPriRequests.size() << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - mPriReRequests: " << mPriReRequests.size() << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - mPriReRequestList: " << mPriReRequestList.size() << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - mPriOccResults: " << mPriOccResults.size() << endl;
	// ---------------------------------------
}


void RenderCoreGlFrame::occlusionTest()
{
	depthPass(false);
	mPriReRequestList.sort(compListFarToNear);
	unsigned delta = 0;
	glDisable(GL_CULL_FACE);
	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	resizeFrustum(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, mProFarClippingPlane);
//	glLoadIdentity();
//	mPriCamera.setRotationMatrix(mPriModelViewMatrix);
//	mPriCamera.calcMatrix();

//	cout << "data resizing frustum to " << mPriTileMap[nre.getRecepient()].xPos << ", " << mPriTileMap[nre.getRecepient()].yPos << ", " << mPriTileMap[nre.getRecepient()].width << ", " << mPriTileMap[nre.getRecepient()].height << endl;
//	resizeWindow(height, width);
//	cout << "starting display of DataCore" << endl;
	// light blue
	glPushMatrix();
		glPushMatrix();
			glPushMatrix();
				glColor3f(1.0f,0.0f,0.0f);
				mPriFbo->bind();
//				mPriFbo->clearColor();
//				cout << "Number of VBOs: " << mPriVbosInFrustum.size()<< endl;
//				for (std::set<uint64_t>::iterator it = mPriIdsInFrustum.begin(); it!= mPriIdsInFrustum.end(); ++it){
////					cout << "drawing box with id: " << *it << endl;
////					cout << "in in node: " << mPriIdLoMap[(*it)]->getIdString() << endl;
//					mPriIdLoMap[(*it)]->getBb().draw();
//				}

				// performing the depthtest
//				std::set<ooctools::Quadruple>::iterator quadIterator = mPriQuadSet.begin();
//				std::list<const ooctools::Quadruple*>::iterator quadIterator = mPriQuadSet.begin();

				unsigned queryCount = 0;
				glDepthMask(GL_FALSE);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				WrapperListIter wIt = mPriReRequestList.begin();
				for (; wIt != mPriReRequestList.end(); ++wIt, queryCount++){
					glBeginQuery(GL_SAMPLES_PASSED, mPriOccQueries[queryCount]);
						mPriIdLoMap[(*wIt)->octreeNode->getId()]->getBb().drawSolidTriFan();
					glEndQuery(GL_SAMPLES_PASSED);
				}
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDepthMask(GL_TRUE);

				// handle query-results
				queryCount = 0;
				GLint queryState;
				for(wIt = mPriReRequestList.begin(); wIt != mPriReRequestList.end(); ++wIt, queryCount++){
					queryState = GL_FALSE;
					while(queryState != GL_TRUE){
					  glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT_AVAILABLE, &queryState);
					}

					glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT, &mPriOccResults[(*wIt)->octreeNode->getId()]);

					// -----------------------------------------------
					if (mPriOccResults[(*wIt)->octreeNode->getId()] <= 0){
						if ((*wIt)->state == WrappedOcNode::REQUESTED_ONLINE ){
							(*wIt)->state = WrappedOcNode::OFFLINE;
							(*wIt)->iVbo->setOffline();
							(*wIt)->usageCount = 0;
							delta = ((*wIt)->iVbo->getVertexCount()*sizeof(V4N4)) + ((*wIt)->iVbo->getIndexCount()*4);
							mPriL2Cache += delta;
							mPriL1Cache -= delta;
						}
						else if ((*wIt)->state == WrappedOcNode::REQUESTED_OFFLINE){
							(*wIt)->state = WrappedOcNode::OFFLINE;
							(*wIt)->usageCount = 0;
						}
					}
					else {
						if ((*wIt)->state == WrappedOcNode::REQUESTED_ONLINE ){
							(*wIt)->state = WrappedOcNode::ONLINE;
							(*wIt)->usageCount = 0;
						}
						else if ((*wIt)->state == WrappedOcNode::REQUESTED_OFFLINE){
							(*wIt)->state = WrappedOcNode::ONLINE;
							(*wIt)->iVbo->setOnline();
							(*wIt)->usageCount = 0;
							delta = ((*wIt)->iVbo->getVertexCount()*sizeof(V4N4)) + ((*wIt)->iVbo->getIndexCount()*4);
							mPriL2Cache -= delta;
							mPriL1Cache += delta;
						}
					}
				}
				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();


	mPriOccResults.clear();
	mPriReRequestList.clear();
	glPolygonMode(GL_FRONT, polyMode);
	glEnable(GL_CULL_FACE);

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
//	cerr << "size of wrappers in frustum: " << mPriWrapperInFrustum.size() << endl;

	unsigned int loadCount = 0;
	QuintSetIter qIt = mPriRequests.begin();
	while (loadCount <= MAX_LOADS_PER_FRAME && qIt != mPriRequests.end()){
		qIt++;
		loadCount++;
	}
	if (loadCount>0){
		// actually request them
		NodeRequestEvent nre = NodeRequestEvent(mPriRequests.begin(), qIt, loadCount);
		MpiControl::getSingleton()->isend(new Message(nre, 0));
		//		cout << "missingVbos vs requested - " << mPriMissingIdsInFrustum.size() << " vs " << nre.getIdxCount() << " vs " << missingTriple.size() << endl;
		mPriRequests.erase(mPriRequests.begin(), qIt);
//		cerr << "requesting " << loadCount << "vbos.." << endl;
	}
	EndTransmissionEvent ete = EndTransmissionEvent();
	MpiControl::getSingleton()->send(new Message(ete, 0));

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

void RenderCoreGlFrame::depthPass(bool _send)
{
	resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, mProFarClippingPlane);
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
	RWrapperListIter wIt = mPriWrapperInFrustum.rbegin();
	for (; wIt != mPriWrapperInFrustum.rend(); ++wIt){
		if((*wIt)->state == WrappedOcNode::ONLINE || (*wIt)->state == WrappedOcNode::RETEST_ONLINE || (*wIt)->state == WrappedOcNode::REQUESTED_ONLINE){
			(*wIt)->iVbo->managedDraw(true);
		}
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	GET_GLERROR(0);
	if (_send){
		FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriTileWidth+(2*mProFrustumExtensionX_px), mPriTileHeight+(2*mProFrustumExtensionY_px));
		DepthBufferEvent dbe = DepthBufferEvent(Tile(mPriTileXPos,mPriTileYPos,mPriTileWidth,mPriTileHeight, 0.0),
				mPriTileXPos,mPriTileYPos,mPriTileWidth+(2*mProFrustumExtensionX_px),mPriTileHeight+(2*mProFrustumExtensionY_px),
				MpiControl::getSingleton()->getRank(), mPriDepthBuffer);
		MpiControl::getSingleton()->clearOutQueue(MpiControl::DATA);
		Message* msg = new Message(dbe, 0, MpiControl::DATA);
		MpiControl::getSingleton()->send(msg);
		clearRequests();
	}
	mPriFbo->unbind();
	glPolygonMode(GL_FRONT, polyMode);
	GET_GLERROR(0);

}

void RenderCoreGlFrame::readAndSendDepth()
{
	mPriFbo->bind();
	FboFactory::getSingleton()->readDepthFromFb(mPriDepthBuffer, 0, 0, mPriTileWidth+(2*mProFrustumExtensionX_px), mPriTileHeight+(2*mProFrustumExtensionY_px));
	DepthBufferEvent dbe = DepthBufferEvent(Tile(mPriTileXPos,mPriTileYPos,mPriTileWidth,mPriTileHeight, 0.0),
			mPriTileXPos,mPriTileYPos,mPriTileWidth+(2*mProFrustumExtensionX_px),mPriTileHeight+(2*mProFrustumExtensionY_px),
			MpiControl::getSingleton()->getRank(), mPriDepthBuffer);
	MpiControl::getSingleton()->clearOutQueue(MpiControl::DATA);
	Message* msg = new Message(dbe, 0, MpiControl::DATA);
	MpiControl::getSingleton()->send(msg);
	clearRequests();
	mPriFbo->unbind();
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
#ifdef DEBUG_FRUSTUMCULLING
	double newTime = glfwGetTime();
#endif

	resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, mProFarClippingPlane);

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
	//TODO umstellen auf ne list
	mPriLo->isInFrustum_orig(priFrustum, &mPriWrapperInFrustum, BoundingBox::getMinDotIdx(mPriViewVector), mPriEyePosition, mPriMaxDistPerLevel, true);
	// original frustum
	resizeFrustum(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, mProFarClippingPlane);


//	getFrustum();
//	mPriLo->isInFrustum_orig(priFrustum, &mPriWrapperInFrustum, BoundingBox::getMinDotIdx(mPriViewVector), mPriEyePosition, mPriMaxDistPerLevel, false);

	if (mPriShowOffset){
		resizeFrustumExt(mPriTileXPos, mPriTileYPos, mPriTileWidth, mPriTileHeight, mProFarClippingPlane);
	}

#ifdef DEBUG_FRUSTUMCULLING
	double newerTime = glfwGetTime();
	mPriFrustumCullingTime += (newerTime-newTime);
	if (mPriFrameTick % FRUSTUMCULLING_AVG == 0){
		cout << "(" << MpiControl::getSingleton()->getRank() << ") FrustumCulling took " << mPriFrustumCullingTime/FRUSTUMCULLING_AVG << " secs avg. over " << DISPLAY_AVG << " frames." << endl;
		mPriFrustumCullingTime = 0.0;
	}
#endif

#ifdef DEBUG_REQUESTDATA
	double newTime2 = glfwGetTime();
#endif

	manageCaching();
//	reRequestVbos();
//	occlusionTest();
	requestMissingVbos();


#ifdef DEBUG_REQUESTDATA
	double newerTime2 = glfwGetTime();
	mPriRequestDataTime += (newerTime2-newTime2);
	if (mPriFrameTick % REQUESTDATA_AVG == 0){
		cout << "(" << MpiControl::getSingleton()->getRank() << ") assemble DataRequests took " << mPriRequestDataTime/REQUESTDATA_AVG << " secs avg. over " << DISPLAY_AVG << " frames." << endl;
		mPriRequestDataTime = 0.0;
	}
#endif
}

void RenderCoreGlFrame::manageCaching()
{
	mPriWrapperInFrustum.sort();
	unsigned int delta = 0;
	WrapperListIter wIt = mPriWrapperInFrustum.begin();
	unsigned cache1 = 0;
	unsigned cache2 = 0;

	unsigned onlineCount = 0;
	for (; wIt != mPriWrapperInFrustum.end(); ++wIt){
		if ((*wIt)->state == WrappedOcNode::ONLINE){
			delta = ((*wIt)->iVbo->getVertexCount()*20) + ((*wIt)->iVbo->getIndexCount()*sizeof(unsigned));
			if (mPriFrameTick != (*wIt)->timeStamp){
//				cerr << (*wIt)->timeStamp << " vs " << mPriFrameTick << endl;
				(*wIt)->iVbo->setOffline();
				(*wIt)->state = WrappedOcNode::OFFLINE;
				mPriL2Cache += delta;
				mPriL1Cache -= delta;
				cache2 += delta;
			}
			else {
				onlineCount++;
				cache1 += delta;
			}
		}
		else if ((*wIt)->state == WrappedOcNode::MISSING){
			mPriRequests.insert(Quintuple((*wIt)->octreeNode->getLevel(), (*wIt)->dist, MpiControl::getSingleton()->getRank(), (*wIt)->octreeNode->getId(), true));
			(*wIt)->state = WrappedOcNode::REQUESTED;
		}
		else if ((*wIt)->state == WrappedOcNode::RETEST_ONLINE || (*wIt)->state == WrappedOcNode::REQUESTED_ONLINE){
			delta = ((*wIt)->iVbo->getVertexCount()*20) + ((*wIt)->iVbo->getIndexCount()*sizeof(unsigned));
			onlineCount++;
			cache1 += delta;
		}
		else if ((*wIt)->state == WrappedOcNode::RETEST_OFFLINE || (*wIt)->state == WrappedOcNode::REQUESTED_OFFLINE || (*wIt)->state == WrappedOcNode::OFFLINE){
			delta = ((*wIt)->iVbo->getVertexCount()*20) + ((*wIt)->iVbo->getIndexCount()*sizeof(unsigned));
			cache2 += delta;
		}
	}

//	cerr << MpiControl::getSingleton()->getRank() << " - l1 cache (counted): " << cache1/1048576 << "Mb" << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - l2 cache (counted): " << cache2/1048576 << "Mb" << endl;
//	cerr << MpiControl::getSingleton()->getRank() << " - VBOS online: " << onlineCount << endl;
//	cerr << "l1 cache: " << mPriL1Cache/1048576 << "Mb" << endl;
//	cerr << "l2 cache: " << mPriL2Cache/1048576 << "Mb" << endl;
//	cerr << "----------------------" << endl;
	// L1-Cache cleaning
	wIt = mPriWrapperInFrustum.begin();
	while ((cache1>L1_CACHE_THRESHOLD || onlineCount>L1_CACHE_VBO_LIMIT) && wIt != mPriWrapperInFrustum.end()){
		// letzen eincachen
		if ((*wIt)->state == WrappedOcNode::ONLINE){
			(*wIt)->iVbo->setOffline();
			(*wIt)->state = WrappedOcNode::OFFLINE;
			delta = ((*wIt)->iVbo->getVertexCount()*20) + ((*wIt)->iVbo->getIndexCount()*sizeof(unsigned));
			mPriL2Cache += delta;
			mPriL1Cache -= delta;
			cache1 -= delta;
			cache2 += delta;
			onlineCount--;
		}
		++wIt;
	}

	// L2 cache cleaning
	wIt = mPriWrapperInFrustum.begin();
	while (cache2>L2_CACHE_THRESHOLD && wIt != mPriWrapperInFrustum.end()){
		if ((*wIt)->state == WrappedOcNode::OFFLINE){
			delta = ((*wIt)->iVbo->getVertexCount()*20) + ((*wIt)->iVbo->getIndexCount()*sizeof(unsigned));
			mPriL2Cache -= delta;
			cache2 -= delta;
			delete (*wIt)->iVbo;
			(*wIt)->iVbo = 0;
			(*wIt)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wIt++);
		}
		else if ((*wIt)->state == WrappedOcNode::MISSING){
			mPriWrapperInFrustum.erase(wIt++);
		}
		else {
			++wIt;
		}
	}
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

void RenderCoreGlFrame::clearRequests()
{
	//delete un-sent requests
	//clear requestlist and remove those from list
	WrapperListIter wIt = mPriWrapperInFrustum.begin();
	while (wIt != mPriWrapperInFrustum.end()){
		if ((*wIt)->state == WrappedOcNode::REQUESTED){
			(*wIt)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wIt++);
		}
//		else if ((*wIt)->state == WrappedOcNode::REQUESTED_ONLINE || (*wIt)->state == WrappedOcNode::RETEST_ONLINE){
//			(*wIt)->state = WrappedOcNode::ONLINE;
//			(*wIt)->usageCount = 0;
//			++wIt;
//		}
//		else if ((*wIt)->state == WrappedOcNode::REQUESTED_OFFLINE || (*wIt)->state == WrappedOcNode::RETEST_OFFLINE){
//			(*wIt)->state = WrappedOcNode::OFFLINE;
//			(*wIt)->usageCount = 0;
//			++wIt;
//		}
		else{
			++wIt;
		}
	}
	mPriRequests.clear();
//	mPriReRequests.clear();
}

void
RenderCoreGlFrame::advTick(){
	++mPriFrameTick %= MODULO_FRAMECOUNT;
	mPriLo->pubTick = mPriFrameTick;

};

void
RenderCoreGlFrame::clearCache()
{
	WrapperListIter wli = mPriWrapperInFrustum.begin();
	for (; wli != mPriWrapperInFrustum.end(); ){
		switch ((*wli)->state){
		case WrappedOcNode::MISSING:
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::ONLINE:
			(*wli)->iVbo->setOffline();
			wli++;
			break;
		case WrappedOcNode::RETEST_ONLINE:
			(*wli)->iVbo->setOffline();
			wli++;
			break;
		case WrappedOcNode::REQUESTED_ONLINE:
			(*wli)->iVbo->setOffline();
			wli++;
			break;
		case WrappedOcNode::OFFLINE:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::REQUESTED_OFFLINE:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::RETEST_OFFLINE:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		default:
			wli++;
			break;
		}
	}

	wli = mPriWrapperInFrustum.begin();
	for (; wli != mPriWrapperInFrustum.end(); ){
		switch ((*wli)->state){
		case WrappedOcNode::ONLINE:
			(*wli)->iVbo->setOnline();
			wli++;
			break;
		case WrappedOcNode::RETEST_ONLINE:
			(*wli)->iVbo->setOnline();
			wli++;
			break;
		case WrappedOcNode::REQUESTED_ONLINE:
			(*wli)->iVbo->setOnline();
			wli++;
			break;
		default:
			wli++;
			break;
		}
	}
}

void
RenderCoreGlFrame::clearEverything()
{
	cerr << "Mem BEFORE " << MpiControl::getSingleton()->getRank() <<  ": " << MemTools::getSingleton()->usedMem() << endl;

	WrapperListIter wli = mPriWrapperInFrustum.begin();
	for (; wli != mPriWrapperInFrustum.end(); ){
		switch ((*wli)->state){
		case WrappedOcNode::MISSING:
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::ONLINE:
			(*wli)->iVbo->setOffline();
			(*wli)->state = WrappedOcNode::MISSING;
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::RETEST_ONLINE:
			(*wli)->iVbo->setOffline();
			(*wli)->state = WrappedOcNode::MISSING;
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::REQUESTED_ONLINE:
			(*wli)->iVbo->setOffline();
			(*wli)->state = WrappedOcNode::MISSING;
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::OFFLINE:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::REQUESTED_OFFLINE:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::RETEST_OFFLINE:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		case WrappedOcNode::REQUESTED:
			delete ((*wli)->iVbo);
			(*wli)->iVbo = 0;
			(*wli)->state = WrappedOcNode::MISSING;
			mPriWrapperInFrustum.erase(wli++);
			break;
		default:
			wli++;
			break;
		}
	}
	mPriReRequestList.clear();
	mPriWrapperInFrustum.clear();
	mPriWrapperInFrustum.resize(0);
	cerr << "Mem AFTER " << MpiControl::getSingleton()->getRank() <<  ": " << MemTools::getSingleton()->usedMem() << endl;
//	int* test = 0;
//	for (unsigned i=0; i<500; ++i){
//		test = new int[125000000];
//		delete[] test;
//	}
}

void
RenderCoreGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(ModelViewMatrixEvent::classid())){
		ModelViewMatrixEvent& mve = (ModelViewMatrixEvent&)event;
//		for (unsigned i=0; i<16; ++i){
//			if (mPriModelViewMatrix[i] != mve.getMatrix()[i]){
//				for (RWrapperListIter wIt = mPriWrapperInFrustum.rbegin(); wIt != mPriWrapperInFrustum.rend(); ++wIt){
//					if ((*wIt)->state== WrappedOcNode::RETEST_ONLINE || (*wIt)->state== WrappedOcNode::REQUESTED_ONLINE){
//						(*wIt)->state = WrappedOcNode::ONLINE;
//						(*wIt)->usageCount = 0;
//					}
//				}
//				break;
//			}
//		}
		memcpy(mPriModelViewMatrix, mve.getMatrix(), sizeof(float)*16);
	}
	else if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& kpe = (KeyPressedEvent&)event;
	    switch (kpe.getKey()) {
			case GLFW_KEY_PAGEUP: // tilt up
			break;
			case GLFW_KEY_KP_SUBTRACT:
				break;
			case GLFW_KEY_KP_ADD:
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
			break;
			case 'N': // decrease far-clipping plane
				mProFarClippingPlane = max(mProFarClippingPlane/2.0f, 50.0f);
				reshape(mProWindowWidth, mProWindowHeight);
			break;
			case 'X':{ // clear cache
				if (kpe.isCtrl()){
					clearEverything();
				}
				else {
					clearCache();
				}
			case 'M':
				cerr << "Mem " << MpiControl::getSingleton()->getRank() <<  ": " << MemTools::getSingleton()->usedMem() << endl;
				break;
			break;}
			default:
			break;
		}
	}
	else if (event.instanceOf(VboEvent::classid())){
		VboEvent& ve = (VboEvent&)event;
		std::map<uint64_t, oocformats::LooseRenderOctree*>::iterator idLoIt;
		for (unsigned i=0; i< ve.getVboCount(); ++i){
			idLoIt = mPriIdLoMap.find(ve.getNodeId(i));
			if (idLoIt != mPriIdLoMap.end()){
				if (idLoIt->second->getWrapper()->state == WrappedOcNode::REQUESTED){
					idLoIt->second->getWrapper()->state = WrappedOcNode::OFFLINE;
					idLoIt->second->getWrapper()->usageCount = 0;
					if (idLoIt->second->getWrapper()->iVbo != 0) {
						cerr << "requested VBO was not 0 as expected!" << endl;
						exit(0);
					}
					idLoIt->second->getWrapper()->iVbo = new IndexedVbo(ve.getIndexArray(i), ve.getIndexCount(i), ve.getVertexArray(i), ve.getVertexCount(i), false);
					mPriL2Cache += (ve.getIndexCount(i)*4) + (ve.getVertexCount(i)*20);
				}
//				else if (idLoIt->second->getWrapper()->state == WrappedOcNode::MISSING){
//					idLoIt->second->getWrapper()->state = WrappedOcNode::OFFLINE;
//					idLoIt->second->getWrapper()->iVbo = new IndexedVbo(ve.getIndexArray(i), ve.getIndexCount(i), ve.getVertexArray(i), ve.getVertexCount(i), false);
//					mPriL2Cache += (ve.getIndexCount(i)*4) + (ve.getVertexCount(i)*20);
//					mPriWrapperInFrustum.push_back(idLoIt->second->getWrapper());
//				}
			}
		}
	}
	else if (event.instanceOf(InfoRequestEvent::classid())){
		stringstream headerS;
		for (int i=0; i < MpiControl::getSingleton()->getRank()*2 + 1; ++i){
			headerS << "-";
		}
		unsigned triCount = 0;
		unsigned triCount_online = 0;
		unsigned triCount_offline = 0;
		unsigned l1Cache = 0;
		unsigned l2Cache = 0;
		unsigned vboCount = 0;
		unsigned vboCount_online = 0;
		unsigned vboCount_offline = 0;
		unsigned vboCount_missing = 0;

		WrapperListIter wli = mPriWrapperInFrustum.begin();
		for(; wli != mPriWrapperInFrustum.end(); ++wli){
			if ((*wli)->state == WrappedOcNode::ONLINE || (*wli)->state == WrappedOcNode::RETEST_ONLINE || (*wli)->state == WrappedOcNode::REQUESTED_ONLINE){
				++vboCount;
				vboCount_online++;
				triCount += (*wli)->octreeNode->getTriangleCount();
				triCount_online += (*wli)->octreeNode->getTriangleCount();
				l1Cache += ((*wli)->iVbo->getIndexCount()*sizeof(unsigned)) + ((*wli)->iVbo->getVertexCount()*sizeof(V4N4));
			}
			else if((*wli)->state == WrappedOcNode::OFFLINE || (*wli)->state == WrappedOcNode::RETEST_OFFLINE || (*wli)->state == WrappedOcNode::REQUESTED_OFFLINE){
				++vboCount;
				vboCount_offline++;
				triCount += (*wli)->octreeNode->getTriangleCount();
				triCount_offline += (*wli)->octreeNode->getTriangleCount();
				l2Cache += ((*wli)->iVbo->getIndexCount()*sizeof(unsigned)) + ((*wli)->iVbo->getVertexCount()*sizeof(V4N4));
			}
			else if((*wli)->state == WrappedOcNode::MISSING || (*wli)->state == WrappedOcNode::REQUESTED){
				vboCount_missing++;
			}
		}

		headerS << "> (" << MpiControl::getSingleton()->getRank() << ") - ";
		cout << headerS.str() << "Tris in RAM: " << triCount << endl;
		cout << headerS.str() << "Tris on GPU: " << triCount_online << endl;
		cout << headerS.str() << "Tris only in RAM: " << triCount_offline << endl;
		cout << headerS.str() << "VBOs in RAM: " << vboCount << endl;
		cout << headerS.str() << "VBOs offline: " << vboCount_offline << endl;
		cout << headerS.str() << "VBOs online: " << vboCount_online << endl;
		cout << headerS.str() << "VBOs missing: " << vboCount_missing << endl;
		cout << headerS.str() << "L1_Cache: " << l1Cache/1048576 << "Mib / " << L1_CACHE_THRESHOLD/1048576 << "Mib" << endl;
		cout << headerS.str() << "L2_Cache: " << l2Cache/1048576 << "Mib / " << L2_CACHE_THRESHOLD/1048576 << "Mib" << endl;
		cout << headerS.str() << "Total Cache: " << (l1Cache+l2Cache)/1048576 << "Mib / " << endl;
		cout << headerS.str() << "L1_Cache (offi): " << mPriL1Cache/1048576 << "Mib / " << L1_CACHE_THRESHOLD/1048576 << "Mib" << endl;
		cout << headerS.str() << "L2_Cache (offi): " << mPriL2Cache/1048576 << "Mib / " << L2_CACHE_THRESHOLD/1048576 << "Mib" << endl;
//		cout << headerS.str() << "currently loaded vbos: " << mPriVbosInFrustum.size() << endl;
//		cout << headerS.str() << "total requested vbos: " << mPriRequestedVboList.size() << endl;
//		cout << headerS.str() << "loaded + requested: " << mPriVbosInFrustum.size()+ mPriRequestedVboList.size()<< endl;
//		cout << headerS.str() << "total vbos in frustum: " << mPriIdsInFrustum.size() << endl;
//		cout << headerS.str() << "VBOs in cache: " << mPriOfflineVbosInFrustum.size() << "/" << MAX_OFFLINE_VBOS<< endl;
		unsigned tCount = 0;
		unsigned iCount = 0;
//		IdVboMapIter mIt = mPriVbosInFrustum.begin();
//		for (; mIt!=mPriVbosInFrustum.end(); ++mIt){
//			tCount += mIt->second->getTriCount();
//			iCount += mIt->second->getIndexCount();
//		}
//		cout << headerS.str() << "loaded online tris (counted): " << tCount << endl;
//		cout << headerS.str() << "loaded online indices (counted): " << iCount << endl;
		tCount = 0;
		iCount = 0;
//		for (mIt = mPriOfflineVbosInFrustum.begin(); mIt!=mPriOfflineVbosInFrustum.end(); ++mIt){
//			tCount += mIt->second->getTriCount();
//			iCount += mIt->second->getIndexCount();
//		}
//		cout << headerS.str() << "loaded offline tris (counted): " << tCount << endl;
//		cout << headerS.str() << "loaded offline indices (counted): " << iCount << endl;

		cout << headerS.str() << "nearPlane: " << mProNearClippingPlane << endl;
		cout << headerS.str() << "farPlane: " << mProFarClippingPlane << endl;

		cout << headerS.str() << "Tile-Dimensions: " << mPriTileXPos << ", " << mPriTileYPos << ", " << mPriTileWidth << ", " << mPriTileHeight << endl;
		cout << "---------------------------------------" << endl;
	}


}
