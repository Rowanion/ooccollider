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
			frame(0), mPriVboMan(0), mPriCgt(0), mPriFbo(0),
			mPriWindowWidth(0), mPriWindowHeight(0), mPriPixelBuffer(0), mPriDepthBuffer(0), mPriNewDepthBuf(false),
			mPriOccResults(std::map<uint64_t, GLint>()), mPriIdPathMap(std::map<uint64_t, std::string>()), mPriDistanceMap(std::multimap<float, uint64_t>()), mPriFarClippingPlane(100.0f), mPriNearClippingPlane(0.1f) {

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

	delete mPriFbo;
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
	camObj.positionCamera(0.0,0.0,5.0,
			0.0,0.0,-3.0,
			0,1,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glGenQueries(100, mPriOccQueries);

	mPriFbo = FboFactory::getSingleton()->createDepthOnlyFbo(mPriWindowWidth,mPriWindowHeight);
	setupCg();
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);

	mPriFbo->bind();
	mPriFbo->clearDepth();
	mPriFbo->clearColor();
	mPriFbo->unbind();
	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));
	mPriOh.generateIdPathMap(mPriLo, mPriIdPathMap);
	mPriOh.generateIdLoMap(mPriLo, mPriIdLoMap);
}

void DataCoreGlFrame::setupCg()
{
	mPriCgt->initCG(true);
	cgVertexProg = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/vp_depth2color.cg", true, "");
	cgFragmentProg = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_depth2color.cg", true, "");

	cgTexture = cgGetNamedParameter(cgFragmentProg, "depthTex");
	cgGLSetTextureParameter(cgTexture, mPriFbo->getDepthTexId());
}

void DataCoreGlFrame::display()
{
	NodeRequestEvent nre = NodeRequestEvent();
	display(nre);
}

void DataCoreGlFrame::display(NodeRequestEvent& nre)
{
//	resizeWindow(height, width);
//	cout << "starting display of DataCore" << endl;
	// light blue
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);
	GET_GLERROR(0);
	glClear(GL_DEPTH_BUFFER_BIT);
	GET_GLERROR(0);
	glLoadIdentity();
	GET_GLERROR(0);
	glMultMatrixf(mPriModelViewMatrix);
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
//	cout << "query: " << queryCount << " vs " << mPriVisibleDistVec.size() << endl;
/*
	// send all vbos that passed the occlusion-tests to the requester
	std::map<uint64_t, ooctools::IndexedVbo*>::iterator vboIt = mPriVbosInFrustum.begin();
	for (; vboIt!= mPriVbosInFrustum.end(); ++vboIt){
		VboEvent ve = VboEvent(vboIt->second, vboIt->first);
		for (unsigned j=0; j < ve.getVertexCount(); ++j){
			for(unsigned i=0; i<4; ++i){
				if (vboIt->second->getVertexData()[j].v[i] != ve.getVertexArray()[j].v[i]){
					cout << "LKHG" << endl;
					exit(0);
				}
			}
		}
//		if (vboIt->second->getVertexCount() != ve.getVertexCount()){
//			cout << "missmatch vbo-vertexCount and vboevent-vertex-count!" << endl;
//			cout << vboIt->second->getVertexCount() << " - " << ve.getVertexCount() << endl;
//			exit(0);
//		}
//		if (vboIt->second->getIndexCount() != ve.getIndexCount()){
//			cout << "missmatch vbo-IndexCount and vboevent-index-count!" << endl;
//			cout << vboIt->second->getIndexCount() << " - " << ve.getIndexCount() << endl;
//			exit(0);
//		}
//		for (unsigned i=0; i< vboIt->second->getIndexCount(); ++i){
//			if (vboIt->second->getIndexData()[i] != ve.getIndexArray()[i]){
//				cout << "missmatch vbo-indexData and vboevent-indexData! ID: " << vboIt->first << " - " << mPriIdPathMap[vboIt->first] << endl;
//				cout << vboIt->second->getIndexData()[i] << " - " << ve.getIndexArray()[i] << endl;
//				exit(0);
//			}
//			if (vboIt->second->getVertexData()[i].v[0] != ve.getVertexArray()[i].v[0] || vboIt->second->getVertexData()[i].v[1] != ve.getVertexArray()[i].v[1] || vboIt->second->getVertexData()[i].v[2] != ve.getVertexArray()[i].v[2]){
//				cout << "missmatch vbo-indexData and vboevent-indexData! ID: " << vboIt->first << " - " << mPriIdPathMap[vboIt->first] << endl;
//				cout << vboIt->second->getVertexData()[i].v[0] << " - " << ve.getVertexArray()[i].v[0] << endl;
//				cout << vboIt->second->getVertexData()[i].v[1] << " - " << ve.getVertexArray()[i].v[1] << endl;
//				cout << vboIt->second->getVertexData()[i].v[2] << " - " << ve.getVertexArray()[i].v[2] << endl;
//				cout << vboIt->second->getVertexData()[i].v[3] << " - " << ve.getVertexArray()[i].v[3] << endl;
//				exit(0);
//			}
//		}
//		cout << "first vertex AFTER loading - " << mPriVbosInFrustum.begin()->first << endl;
//		cout << mPriVbosInFrustum.begin()->second->getVertexData()[0].v[0] << endl;
//		cout << mPriVbosInFrustum.begin()->second->getVertexData()[0].v[1] << endl;
//		cout << mPriVbosInFrustum.begin()->second->getVertexData()[0].v[2] << endl;
//		cout << mPriVbosInFrustum.begin()->second->getVertexData()[0].v[3] << endl;
//		cout << (int)mPriVbosInFrustum.begin()->second->getVertexData()[0].n[0] << endl;
//		cout << (int)mPriVbosInFrustum.begin()->second->getVertexData()[0].n[1] << endl;
//		cout << (int)mPriVbosInFrustum.begin()->second->getVertexData()[0].n[2] << endl;
//		cout << (int)mPriVbosInFrustum.begin()->second->getVertexData()[0].n[3] << endl;
//		cout << "first vertex AFTER event - " << mPriVbosInFrustum.begin()->first << endl;
//		cout << ve.getVertexArray()[0].v[0] << endl;
//		cout << ve.getVertexArray()[0].v[1] << endl;
//		cout << ve.getVertexArray()[0].v[2] << endl;
//		cout << ve.getVertexArray()[0].v[3] << endl;
//		cout << (int)ve.getVertexArray()[0].n[0] << endl;
//		cout << (int)ve.getVertexArray()[0].n[1] << endl;
//		cout << (int)ve.getVertexArray()[0].n[2] << endl;
//		cout << (int)ve.getVertexArray()[0].n[3] << endl;
//		exit(0);
		Message* msg = new Message(ve, nre.getRecepient());
		MpiControl::getSingleton()->send(msg);
	}
	MpiControl::getSingleton()->send(new Message(EndTransmissionEvent::classid()->getShortId(), 0, nre.getRecepient(), 0));


*/

	// send the visible object to the requester
	VboEvent ve = VboEvent(mPriVisibleVbosVec, mPriVisibleDistVec);
	Message* msg = new Message(ve, nre.getRecepient());
	MpiControl::getSingleton()->push(msg);
//  MpiControl::getSingleton()->isend();

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

	// draw the result for debugging
	cgGLEnableTextureParameter(cgTexture);
	mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertexProg);
	mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragmentProg);

	mPriFbo->drawDepthFSQuad();

	mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
	mPriCgt->stopCgShader(mPriCgt->cgFragProfile);
	cgGLDisableTextureParameter(cgTexture);
}

void DataCoreGlFrame::reshape(int width, int height) {
	cout << "Window resized to: " << width << ", " << height << endl;
	cout << "SIZE CHANGED" << endl;

	if (mPriPixelBuffer==0|| mPriWindowWidth != width || mPriWindowHeight != height){
		delete[] mPriPixelBuffer;
		mPriPixelBuffer = new GLubyte[width*height*4];
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
	gluPerspective(45.0f, ratio, mPriNearClippingPlane, mPriFarClippingPlane);
//	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
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
		DepthBufferEvent& dbe = (DepthBufferEvent&)event;
		bool fboOn = mPriFbo->isBound();
		if (!fboOn){
			mPriFbo->bind();
		}
		glClearColor(0.0, 1.0, 0.0, 1.0);
//		glClear(GL_DEPTH_BUFFER_BIT);
//		glClear(GL_COLOR_BUFFER_BIT);
//		mPriFbo->clearDepth();
//		glPushAttrib(GL_ALL_ATTRIB_BITS);
//			glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
			FboFactory::getSingleton()->drawDepthToFb(dbe.getDepth(), dbe.getX(), dbe.getY(), dbe.getWidth(), dbe.getHeight());
		glDepthFunc(GL_LEQUAL);
//			glEnable(GL_DEPTH_TEST);
//		glPopAttrib();
//		setupTexture();

		if (!fboOn){
			mPriFbo->unbind();
		}
		for (int i=0; i< dbe.getWidth()*dbe.getHeight(); ++i){
			if (dbe.getDepth()[i]>1.0 || dbe.getDepth()[i]<0.0){
				cout << "error in depthbuffer: " << dbe.getDepth()[i]<< endl;
			}
		}

	}
	else if (event.instanceOf(InfoRequestEvent::classid())){
		stringstream headerS;
		for (int i=0; i < MpiControl::getSingleton()->getRank()*2 + 1; ++i){
			headerS << "-";
		}
		headerS << "> (" << MpiControl::getSingleton()->getRank() << ") - ";
		cout << headerS.str() << "INFO" << endl;
		GLfloat mat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		ooctools::GeometricOps::transposeMat4(mat);
		for (unsigned i=0; i< 16; i+=4){
			cout << headerS.str() << "MVP: " << mat[i] << "\t" << mat[i+1] << "\t" << mat[i+2] << "\t" << mat[i+3] << endl;
		}
		cout << headerS.str() << "nearPlane: " << mPriNearClippingPlane << endl;
		cout << headerS.str() << "farPlane: " << mPriFarClippingPlane << endl;

		cout << "---------------------------------------" << endl;
	}
}
