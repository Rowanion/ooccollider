/**
 * @file	DataCoreGlFrame.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	DataCoreGlFrame class declaration.
 */

#include "DataCoreGlFrame.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <sstream>

#include "GeometricOps.h"
#include "V3f.h"
#include "EventManager.h"
#include "ModelViewMatrixEvent.h"
#include "MpiControl.h"
#include "KeyPressedEvent.h"
#include "Message.h"
#include "FboFactory.h"
#include "ColorBufferEvent.h"
#include "DepthBufferEvent.h"
#include "NodeRequestEvent.h"
#include "VboEvent.h"
#include "EndTransmissionEvent.h"
#include "InfoRequestEvent.h"

using namespace std;
using namespace ooctools;
using namespace oocframework;

DataCoreGlFrame::DataCoreGlFrame() :
	scale(1.0f), avgFps(0.0f), time(0.0),
			frame(0), mPriVboMan(0), mPriCgt(0),
			mPriWindowWidth(0), mPriWindowHeight(0), mPriDepthBuffer(0), mPriNewDepthBuf(false), mPriExtendedFovy(EXTENDED_FOVY),
			mPriOccResults(std::map<uint64_t, GLint>()), mPriIdPathMap(std::map<uint64_t, std::string>()),
			mPriDistanceMap(std::multimap<float, uint64_t>()), mPriFarClippingPlane(FAR_CLIPPING_PLANE),
			mPriNearClippingPlane(0.1f), mPriFbo(0), mPriCamera(OOCCamera())
			{

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}
	mPriVboMan = VboManager::getSingleton();
	mPriVboMan->setColorTable(ColorTable(string(BASE_MODEL_PATH) + string("/colortable.bin")));
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
			DepthBufferEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this,
			InfoRequestEvent::classid());

}

DataCoreGlFrame::~DataCoreGlFrame() {
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, ModelViewMatrixEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, DepthBufferEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, InfoRequestEvent::classid());

	for (map<int, GLuint>::iterator iter = mPriDepthTextures.begin(); iter != mPriDepthTextures.end(); ++iter){
		glDeleteTextures(1, &iter->second);
	}
	mPriDepthTextures.clear();

	delete mPriFbo;

	mPriTileMap.clear();

}

void DataCoreGlFrame::init() {

	glewInit();
	glDrawBuffer(GL_FRONT_AND_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	GET_GLERROR(0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);

	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);

	glGenQueries(100, mPriOccQueries);
	GET_GLERROR(0);

	mPriFbo = new Fbo(mPriWindowWidth,mPriWindowHeight);
	Tile t = Tile();
	for (unsigned i=0; i< MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER); ++i){
		GET_GLERROR(0);
		mPriDepthTextures.insert(make_pair(MpiControl::getSingleton()->getRenderGroup()[i], mPriFbo->createDepthTex()));
		mPriFbo->setDepthTex(mPriDepthTextures[MpiControl::getSingleton()->getRenderGroup()[i]]);
		mPriFbo->bind();
		mPriFbo->clearDepth();
		mPriFbo->clearColor();
		Fbo::checkFbo(mPriFbo);
		mPriFbo->unbind();

		GET_GLERROR(0);
		mPriTileMap.insert(make_pair(MpiControl::getSingleton()->getRenderGroup()[i], t));
	}
	setupCg();

	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));
	mPriOh.generateIdPathMap(mPriLo, mPriIdPathMap);
	mPriOh.generateIdLoMap(mPriLo, mPriIdLoMap);
	reshape(mPriWindowWidth, mPriWindowHeight);
}

void DataCoreGlFrame::setupCg()
{
	mPriCgt->initCG(true);
	cgVertexProg = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/vp_depth2color.cg", true, "");
	cgFragmentProg = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_depth2color.cg", true, "");

	cgTexture = cgGetNamedParameter(cgFragmentProg, "depthTex");
//	cgGLSetTextureParameter(cgTexture, mPriFbo->getDepthTexId());
}

void DataCoreGlFrame::display()
{
	NodeRequestEvent nre = NodeRequestEvent();
	display(nre);
}

void DataCoreGlFrame::display(NodeRequestEvent& nre)
{
	GET_GLERROR(0);
	resizeFrustum(mPriTileMap[nre.getRecepient()].xPos, mPriTileMap[nre.getRecepient()].yPos, mPriTileMap[nre.getRecepient()].width, mPriTileMap[nre.getRecepient()].height, true);
//	cout << "data resizing frustum to " << mPriTileMap[nre.getRecepient()].xPos << ", " << mPriTileMap[nre.getRecepient()].yPos << ", " << mPriTileMap[nre.getRecepient()].width << ", " << mPriTileMap[nre.getRecepient()].height << endl;
//	resizeWindow(height, width);
//	cout << "starting display of DataCore" << endl;
	// light blue
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);
	GET_GLERROR(0);
	glClear(GL_DEPTH_BUFFER_BIT);
	GET_GLERROR(0);
	glLoadIdentity();
	mPriCamera.setRotationMatrix(mPriModelViewMatrix);
	mPriCamera.calcMatrix();
	GET_GLERROR(0);

	// load all requested vbos
//	cout << "DataCore Loading VBOs:" << endl;
	for (unsigned i=0; i< nre.getIdxCount(); ++i){
		map<uint64_t, std::string>::iterator IT = mPriIdPathMap.find(nre.getId(i));
		if(IT == mPriIdPathMap.end()) {
			cerr << "ID " << nre.getId(i) << " not in pathmap!" << endl;
			exit(0);
		}
//		cout << "  - VBO " << nre.getId(i) << "," << nre.getByteSize() << ", " << nre.getIdxCount() << endl;
//		cout << "  - PATH " << "/home/ava/Diplom/Model/Octree/data/"+mPriIdPathMap[nre.getId(i)]+".idx" << endl;
		mPriVboMap.insert(make_pair(nre.getId(i), new IndexedVbo(fs::path(string(BASE_MODEL_PATH)+"/data/"+mPriIdPathMap[nre.getId(i)]+".idx"), nre.getId(i))));
		mPriDistanceMap.insert(make_pair(nre.getDistance(i), nre.getId(i)));
//		cout << nre.getDistance(i) << endl;
//		exit(0);
	}



//	normalizeFrustum();
//	glLoadIdentity();
//	gluLookAt( // Set camera position and orientation
//			0.0, 0.0, 10.0, // Camera position (x,y,z)
//			0.0, 0.0, 0.0, // View point (x,y,z)
//			0.0, 1.0, 0.0 // Up-vector (x,y,z)
//	);
	glPushMatrix();
		glPushMatrix();
			glPushMatrix();
				glColor3f(1.0f,0.0f,0.0f);
				mPriFbo->setDepthTex(mPriDepthTextures[nre.getRecepient()]);
				mPriFbo->bind();
//				mPriFbo->clearColor();
//				cout << "Number of VBOs: " << mPriVbosInFrustum.size()<< endl;
//				for (std::set<uint64_t>::iterator it = mPriIdsInFrustum.begin(); it!= mPriIdsInFrustum.end(); ++it){
////					cout << "drawing box with id: " << *it << endl;
////					cout << "in in node: " << mPriIdLoMap[(*it)]->getIdString() << endl;
//					mPriIdLoMap[(*it)]->getBb().draw();
//				}

				// performing the depthtest
				std::map<uint64_t, ooctools::IndexedVbo*>::iterator vboIterator = mPriVboMap.begin();
				std::map<float, uint64_t>::iterator distIterator = mPriDistanceMap.begin();
				unsigned queryCount = 0;
				glDepthMask(GL_FALSE);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				for(distIterator = mPriDistanceMap.begin(); distIterator != mPriDistanceMap.end(); ++distIterator){
//				for(vboIterator = mPriVboMap.begin(); vboIterator != mPriVboMap.end(); ++vboIterator){
					glBeginQuery(GL_SAMPLES_PASSED, mPriOccQueries[queryCount]);
						mPriVboMap[distIterator->second]->managedDraw(true);
					glEndQuery(GL_SAMPLES_PASSED);
					queryCount++;
				}
				glDepthMask(GL_TRUE);

				// handle query-results
				queryCount = 0;
				for(distIterator = mPriDistanceMap.begin(); distIterator != mPriDistanceMap.end(); ++distIterator){
					GLint queryState = GL_FALSE;
					while(queryState != GL_TRUE){
					  glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT_AVAILABLE, &queryState);
					}
					glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT, &mPriOccResults[distIterator->second]);
//					if (true){
					if (mPriOccResults[distIterator->second]>0){
						// add visible VBO to the current DepthBuffer
						mPriVboMap[distIterator->second]->managedDraw(true);
						mPriVisibleVbosVec.push_back(mPriVboMap[distIterator->second]);
						mPriVisibleDistVec.push_back(distIterator->first);
					}
					queryCount++;
				}
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//				setupTexture();
				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();


	if (mPriVisibleDistVec.size() > 0){
		// send the visible object to the requester
		VboEvent ve = VboEvent(mPriVisibleVbosVec, mPriVisibleDistVec, nre.isExtendedFrustum());
		if (nre.isExtendedFrustum()){
//			cout << "sending CacheVBOS: (" << ve.getNodeId(0) << ") - " << ve.getVboCount() << endl;
		}

		Message* msg = new Message(ve, nre.getRecepient());
		MpiControl::getSingleton()->push(msg);
		//  MpiControl::getSingleton()->isend();
	}
	// cleanup
	unsigned vboMapSize = mPriVboMap.size();
	for (unsigned i=0; i< vboMapSize; ++i){
		mPriVboMap.begin()->second->setOffline();
		delete mPriVboMap.begin()->second;
		mPriVboMap.erase(mPriVboMap.begin());
	}
	mPriVboMap.clear();
	mPriDistanceMap.clear();
	mPriVisibleVbosVec.clear();
	mPriVisibleDistVec.clear();

	mPriOccResults.clear();

	if (nre.getRecepient() == 1){
		// draw the result for debugging
		reshape(mPriWindowWidth,mPriWindowHeight);
		cgGLSetTextureParameter(cgTexture, mPriFbo->getDepthTexId());
		cgGLEnableTextureParameter(cgTexture);
		mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertexProg);
		mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragmentProg);

		mPriFbo->drawDepthFSQuad();

		mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
		mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
		cgGLDisableTextureParameter(cgTexture);
		GET_GLERROR(0);
	}

}

void DataCoreGlFrame::reshape(int width, int height) {
//	cout << "Window resized to: " << width << ", " << height << endl;
//	cout << "SIZE CHANGED" << endl;

	mPriWindowWidth = width;
	mPriWindowHeight = height;

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
	gluPerspective(45.0f, ratio, mPriNearClippingPlane, mPriFarClippingPlane);
//	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
	//resize
	initTiles(false);
}

void DataCoreGlFrame::initTiles(bool extendFovy)
{
	//resize
	float fovy = 45;
//	if (extendFovy){
//		fovy = mPriExtendedFovy;
//	}
//	else {
//		fovy = 45.0;
//	}

	GLfloat oppFac = (GLfloat)800 / (GLfloat)640;
	oppFac *= oppFac;
	screenYMax = tan(fovy / 360.0 * ooctools::GeometricOps::PI) * mPriNearClippingPlane;
	screenYMaxH = tan((fovy * ratio) / 360.0 * ooctools::GeometricOps::PI) * mPriNearClippingPlane;
	screenYMax *= oppFac;
	screenYMaxH *= oppFac;

//	ratio = (GLfloat)mPriWindowWidth / (GLfloat)mPriWindowHeight;
//	ratio = (GLfloat)800 / (GLfloat)600;
	ratio = (GLfloat)640 / (GLfloat)480;

	screenXMax = screenYMax * ratio;
	screenYMin = -screenYMax;

	screenXMaxH = screenYMaxH * ratio;
	screenYMinH = -screenYMaxH;


}

void DataCoreGlFrame::resizeFrustum() {
	this->resizeFrustum(0, 0, mPriWindowWidth, mPriWindowHeight);
}

void DataCoreGlFrame::resizeFrustum(unsigned _width, unsigned _height) {
	this->resizeFrustum(0, 0, _width, _height);
}

void DataCoreGlFrame::resizeFrustum(unsigned tileXPos, unsigned tileYPos, unsigned tileswidth, unsigned tilesheight, bool extendFrustum)
{
	GET_GLERROR(0);


	if (tilesheight == 0)
		tilesheight = 1;
	if (tileswidth == 0)
		tileswidth = 1;
	worldTopLine = (GLdouble) tileYPos / (GLdouble) mPriWindowHeight;
	worldBottomLine = (GLdouble) (tileYPos + tilesheight) / (GLdouble) mPriWindowHeight;
	worldLeftLine = (GLdouble) tileXPos / (GLdouble) mPriWindowWidth;
	worldRightLine = (GLdouble) (tileXPos + tileswidth) / (GLdouble) mPriWindowWidth;

	GET_GLERROR(0);
	glViewport(0, 0, (GLint) 800, (GLint) 600);
//	glViewport(0, 0, (GLint) 640, (GLint) 480);
	GET_GLERROR(0);
//	glViewport(0, 0, (GLint) tileswidth, (GLint) tilesheight);
	GET_GLERROR(0);
	glMatrixMode(GL_PROJECTION);
	GET_GLERROR(0);
	glLoadIdentity();
	GET_GLERROR(0);

	worldTopLine = screenYMin + ((screenYMax - screenYMin) * worldTopLine);
	worldBottomLine = screenYMin
			+ ((screenYMax - screenYMin) * worldBottomLine);

	worldLeftLine = screenYMinH + ((screenYMaxH - screenYMinH) * worldLeftLine);
	worldRightLine = screenYMinH + ((screenYMaxH - screenYMinH)
			* worldRightLine);

	GET_GLERROR(0);
	glFrustum(worldLeftLine, worldRightLine, worldTopLine, worldBottomLine,
			mPriNearClippingPlane, mPriFarClippingPlane);

	GET_GLERROR(0);
	glMatrixMode(GL_MODELVIEW);
	GET_GLERROR(0);

}

void DataCoreGlFrame::debug() {
	scale *= 2.0f;
	cout << "debug!" << endl;
}

void DataCoreGlFrame::calcFPS() {
	avgFps = 0.0f;
	for (unsigned i = 0; i < 10; ++i) {
		avgFps += fps[i];
	}
	avgFps /= 10.0f;

}

void DataCoreGlFrame::setMvMatrix(const float* matrix)
{
	memcpy(mPriModelViewMatrix, matrix, 16*sizeof(float));
}

void DataCoreGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(ModelViewMatrixEvent::classid())){
		ModelViewMatrixEvent& mve = (ModelViewMatrixEvent&)event;
//		glLoadMatrixf(mve.getMatrix());
		memcpy(mPriModelViewMatrix, mve.getMatrix(), 16*sizeof(float));
//		cout << "loaded matrix: " << MpiControl::getSingleton()->getRank() << endl;
	}
	else if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& mde = (KeyPressedEvent&)event;
	    switch (mde.getKey()) {
			case GLFW_KEY_PAGEUP: // tilt up
			break;
			case GLFW_KEY_KP_SUBTRACT:
//				mPriFrustumExtension -= 0.1;
				break;
			case GLFW_KEY_KP_ADD:
//				mPriFrustumExtension += 0.1;
				break;
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
			break;
			case 'N': // decrease far-clipping plane
				mPriFarClippingPlane = max(mPriFarClippingPlane/2.0f, 50.0f);
				reshape(mPriWindowWidth, mPriWindowHeight);
			break;
			default:
			break;
		}
	}
	else if (event.instanceOf(DepthBufferEvent::classid())){
		GET_GLERROR(0);

		DepthBufferEvent& dbe = (DepthBufferEvent&)event;
		mPriTileMap[dbe.getMpiRank()].xPos = dbe.getX();
		mPriTileMap[dbe.getMpiRank()].yPos = dbe.getY();
		mPriTileMap[dbe.getMpiRank()].width = dbe.getWidth();
		mPriTileMap[dbe.getMpiRank()].height = dbe.getHeight();

		cout << dbe.getMpiRank() << " ===> new depthbuffer dim: " << dbe.getX() << ", " << dbe.getY() << ", " << dbe.getWidth() << ", " << dbe.getHeight()<< endl;
		bool fboOn = mPriFbo->isBound();
		GET_GLERROR(0);
		if (!fboOn){
			mPriFbo->setDepthTex(mPriDepthTextures[dbe.getMpiRank()]);
			GET_GLERROR(0);
			mPriFbo->bind();
			GET_GLERROR(0);
		}
		GET_GLERROR(0);
		glClearColor(0.0, 1.0, 0.0, 1.0);
		GET_GLERROR(0);
		resizeFrustum(dbe.getX(), dbe.getY(), dbe.getWidth(), dbe.getHeight());
		GET_GLERROR(0);
		mPriFbo->clearDepth();
		GET_GLERROR(0);
		FboFactory::getSingleton()->drawDepthToFb(dbe.getDepth(), 0, 0, dbe.getWidth(), dbe.getHeight());
		GET_GLERROR(0);
//		setupTexture();

		GET_GLERROR(0);
		if (!fboOn){
			mPriFbo->unbind();
		}
		GET_GLERROR(0);
		for (int i=0; i< dbe.getWidth()*dbe.getHeight(); ++i){
			if (dbe.getDepth()[i]>1.0 || dbe.getDepth()[i]<0.0){
				cout << "error in depthbuffer: " << dbe.getDepth()[i]<< endl;
			}
		}
		GET_GLERROR(0);

	}
	else if (event.instanceOf(InfoRequestEvent::classid())){
		stringstream headerS;
		for (int i=0; i < MpiControl::getSingleton()->getRank()*2 + 1; ++i){
			headerS << "-";
		}
		headerS << "> (" << MpiControl::getSingleton()->getRank() << ") - ";
		cout << headerS.str() << "INFO" << endl;
//		GLfloat mat[16];
//		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
//		ooctools::GeometricOps::transposeMat4(mat);
//		for (unsigned i=0; i< 16; i+=4){
//			cout << headerS.str() << "MVP: " << mat[i] << "\t" << mat[i+1] << "\t" << mat[i+2] << "\t" << mat[i+3] << endl;
//		}
		cout << headerS.str() << "nearPlane: " << mPriNearClippingPlane << endl;
		cout << headerS.str() << "farPlane: " << mPriFarClippingPlane << endl;
		cout << headerS.str() << "Frustum-Modifier: " << mPriFrustumExtension << endl;

		cout << "---------------------------------------" << endl;
	}
}
