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

#include <sys/mman.h>
#include <cstdio>
#include <fcntl.h>

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
#include "OcclusionResultsEvent.h"
#include "Logger.h"
#include "Log.h"
#include "VboDistributionEvent.h"


using namespace std;
using namespace ooctools;
using namespace oocframework;

DataCoreGlFrame::DataCoreGlFrame(unsigned _winWidth, unsigned _winHeight, unsigned _targetWidth, unsigned _targetHeight) :
		AbstractGlFrame(_winWidth, _winHeight, _targetWidth, _targetHeight),
		scale(1.0f), avgFps(0.0f), time(0.0),
		frame(0), mPriCgt(0), mPriDepthBuffer(0), mPriNewDepthBuf(false),
		mPriOccResults(std::map<uint64_t, GLint>()), mPriIdPathMap(std::map<uint64_t, std::string>()),
		mPriCCol(PRESELECTED_SEED, LVL_OF_REDUNDANCY), mPriFbo(0), mPriCamera(OOCCamera())
{

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}
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
	Logger::getSingleton()->closeAllHandles();

}

void DataCoreGlFrame::init() {

	glewInit();
	glDrawBuffer(GL_FRONT_AND_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	GET_GLERROR(0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);

	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);

	glGenQueries(MAX_LOADS_PER_FRAME*4, mPriOccQueries);
	GET_GLERROR(0);

	mPriFbo = new Fbo(mProWindowWidth,mProWindowHeight);
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

	sleep(20);
	mPriLo = mPriOh.loadLooseRenderOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));
	mPriOh.generateIdPathMap(mPriLo, mPriIdPathMap);
	mPriOh.generateIdLoMap(mPriLo, mPriIdLoMap);

#ifdef TRANSMIT_DISTRIBUTION
	// receive your data assignment over the network via mpi
	cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " standing by for Data-BroadCast...." << endl;
	MpiControl::getSingleton()->receive(0);
	Message* msg = 0;
	msg = MpiControl::getSingleton()->pop();
	if (msg!=0 && msg->getType() == VboDistributionEvent::classid()->getShortId()) {
		VboDistributionEvent vde = VboDistributionEvent(msg);
		cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " got " << vde.getIdCount() << " ids." << endl;
		mPriCCol.setMyNodeSet(vde.getIdCount(), vde.getIdArray());
		char* buffer = 0;
		unsigned bufferSize = 0;
		MPI::COMM_WORLD.Bcast(&bufferSize, sizeof(unsigned), MPI_CHAR, 0);
		while(bufferSize != 0){
			// the buffer itself
			cerr << "received new buffersize: " << bufferSize << endl;
			buffer = new char[bufferSize];
			MPI::COMM_WORLD.Bcast(buffer, bufferSize, MPI_CHAR, 0);
			cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " got new buffer of size " << bufferSize << "." << endl;
			parseAndLoadVArrays(buffer, bufferSize, mPriCCol.getMyNodeSet());
			delete buffer;
			buffer = 0;
			// the new bufferSize
			MPI::COMM_WORLD.Bcast(&bufferSize, sizeof(unsigned), MPI_CHAR, 0);
		}
		cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " successfully received all BroadCasts." << endl;
	}
	else {
		cerr << "got a message other than VboDistribution. This should not happen!" << endl;
		exit(0);
	}
	delete msg;
	msg = 0;

#else
	// generate your data assignment yourself by direct file access
	mPriCCol.generateDistribution(mPriLo);
#endif

#if defined(KEEP_VBOS_RESIDENT) & !defined(TRANSMIT_DISTRIBUTION)
	cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " now loading " << mPriCCol.getMyNodeSet().size() << "Vbos...." << endl;
	parseAndLoadVArrays(mPriCCol.getMyNodeSet());
	cerr << "DATANODE "<< MpiControl::getSingleton()->getRank() << " loaded all data, thus having " << mPriVArrayMap.size() << " VArrays resident." << endl;
#elif !defined(KEEP_VBOS_RESIDENT) & !defined(TRANSMIT_DISTRIBUTION)
//	cerr << "elsezweig of vbo resident...." << endl;
	mPriOh.generateIdLocMap(fs::path(string(BASE_MODEL_PATH)+string(MODEL_DIR)), mPriIdLocMap);
#endif

	reshape(mProWindowWidth, mProWindowHeight);
	mPriByteSize = 0;
}

void DataCoreGlFrame::setupCg()
{
#ifdef SHOW_DEPTH_IMAGE
	mPriCgt->initCG(true);
	cgVertexProg = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/vp_depth2color.cg", true, "");
	cgFragmentProg = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_depth2color.cg", true, "");

	cgTexture = cgGetNamedParameter(cgFragmentProg, "depthTex");
//	cgGLSetTextureParameter(cgTexture, mPriFbo->getDepthTexId());
#endif
}

void DataCoreGlFrame::display()
{
	NodeRequestEvent nre = NodeRequestEvent(0);
	std::list<const Quintuple*> quintList = std::list<const Quintuple*>();
	display(MpiControl::getSingleton()->getRenderGroup()[0], &quintList);
}

void DataCoreGlFrame::display(int _destId, std::list<const Quintuple*>* _quintList)
{
	// ------------------------

//	std::map<uint64_t, ooctools::IndexedVertexArray*>::iterator testIt = mPriVArrayMap.find((*quintIt)->id);
	// ------------------------
	GET_GLERROR(0);
	resizeFrustumExt(mPriTileMap[_destId].xPos, mPriTileMap[_destId].yPos, mPriTileMap[_destId].width, mPriTileMap[_destId].height, mProFarClippingPlane);
//	cout << "data resizing frustum to " << mPriTileMap[nre.getRecepient()].xPos << ", " << mPriTileMap[nre.getRecepient()].yPos << ", " << mPriTileMap[nre.getRecepient()].width << ", " << mPriTileMap[nre.getRecepient()].height << endl;
//	resizeWindow(height, width);
//	cout << "starting display of DataCore" << endl;
	// light blue
	glClear(GL_DEPTH_BUFFER_BIT);
	GET_GLERROR(0);
	glLoadIdentity();
	mPriCamera.setRotationMatrix(mPriModelViewMatrix);
	mPriCamera.calcMatrix();
	GET_GLERROR(0);

	// load all requested vbos
//	cout << "DataCore Loading VBOs:" << endl;
	const ooctools::Quintuple* currQuintuple; //= ooctools::Quadruple();
	std::list<const Quintuple*>::iterator quintIt = _quintList->begin();
	for (; quintIt != _quintList->end(); ++quintIt){
		currQuintuple = *quintIt;
		map<uint64_t, std::string>::iterator IT = mPriIdPathMap.find(currQuintuple->id);
		if(IT == mPriIdPathMap.end()) {
			cerr << "ID " << currQuintuple->id << " not in pathmap!" << endl;
			exit(0);
		}

//		cout << "  - VBO " << nre.getId(i) << "," << nre.getByteSize() << ", " << nre.getIdxCount() << endl;
//		cout << "  - PATH " << "/home/ava/Diplom/Model/Octree/data/"+mPriIdPathMap[nre.getId(i)]+".idx" << endl;
//		mPriVboMap.insert(make_pair(currQuintuple->id, new IndexedVbo(fs::path(string(BASE_MODEL_PATH)+"/data/"+mPriIdPathMap[currQuintuple->id]+".idx"), currQuintuple->id, false)));
#if !defined(KEEP_VBOS_RESIDENT) & !defined(TRANSMIT_DISTRIBUTION)
		mPriVArrayMap.insert(make_pair(currQuintuple->id, new IndexedVertexArray(mPriIdLocMap[currQuintuple->id])));
#endif
//		mPriDistanceMap.insert(make_pair(nre.getDistance(i), nre.getId(i)));
//		mPriQuadSet.insert(currQuadruple);
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
				mPriFbo->setDepthTex(mPriDepthTextures[_destId]);
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

				glDisable(GL_CULL_FACE);
				unsigned queryCount = 0;
				glDepthMask(GL_FALSE);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				for(quintIt = _quintList->begin(); quintIt != _quintList->end(); ++quintIt){
					glBeginQuery(GL_SAMPLES_PASSED, mPriOccQueries[queryCount]);
						mPriIdLoMap[(*quintIt)->id]->getBb().drawSolidTriFan();
					glEndQuery(GL_SAMPLES_PASSED);
					queryCount++;
				}
				glDepthMask(GL_TRUE);

				// handle query-results

				unsigned byteSize = 0;
				queryCount = 0;
				glEnable(GL_CULL_FACE);

				for(quintIt = _quintList->begin(); quintIt != _quintList->end(); ++quintIt){
					GLint queryState = GL_FALSE;
					while(queryState != GL_TRUE){
					  glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT_AVAILABLE, &queryState);
					}

					glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT, &mPriOccResults[(*quintIt)->id]);
//					if (true){

					if (mPriOccResults[(*quintIt)->id]>0){
						// add visible VBO to the current DepthBuffer
						mPriVArrayMap[(*quintIt)->id]->managedDraw();
						mPriVisibleVArraysVec.push_back(mPriVArrayMap[(*quintIt)->id]);
						mPriVisibleDistExtVec.push_back(DistExtPair((*quintIt)->dist, (*quintIt)->priority));
						byteSize = mPriVArrayMap[(*quintIt)->id]->getIndexCount()*sizeof(unsigned)+mPriVArrayMap[(*quintIt)->id]->getVertexCount()*sizeof(V4N4);
					}
					queryCount++;
				}
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//				setupTexture();
				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	mPriByteSize = max(mPriByteSize, byteSize);

	if (mPriVisibleDistExtVec.size() > 0){
		// send the visible object to the requester
		VboEvent ve = VboEvent(mPriVisibleVArraysVec, mPriVisibleDistExtVec);
		if ((*_quintList->begin())->priority){
//			cout << "sending CacheVBOS: (" << ve.getNodeId(0) << ") - " << ve.getVboCount() << endl;
		}

		Message* msg = new Message(ve, (*_quintList->begin())->destId);
		MpiControl::getSingleton()->push(msg);
		//  MpiControl::getSingleton()->isend();
	}
	// cleanup

#if !defined(KEEP_VBOS_RESIDENT) & !defined(TRANSMIT_DISTRIBUTION)
	std::map<uint64_t, ooctools::IndexedVertexArray*>::iterator vArrIterator = mPriVArrayMap.begin();
	for (; vArrIterator != mPriVArrayMap.end();){
		delete vArrIterator->second;
		mPriVArrayMap.erase(vArrIterator++);
	}
	mPriVArrayMap.clear();
#endif
	mPriQuintSet.clear();
	mPriVisibleVArraysVec.clear();
	mPriVisibleDistExtVec.clear();

	mPriOccResults.clear();

#ifdef SHOW_DEPTH_IMAGE
	if (_destId == 1){
		// draw the result for debugging
		reshape(mProWindowWidth,mProWindowHeight);
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
#endif
}

void DataCoreGlFrame::occlusionTest(int _destId, std::list<const Quintuple*>* _quintList)
{
	glDisable(GL_CULL_FACE);

	GET_GLERROR(0);
	resizeFrustumExt(mPriTileMap[_destId].xPos, mPriTileMap[_destId].yPos, mPriTileMap[_destId].width, mPriTileMap[_destId].height, mProFarClippingPlane);
//	cout << "data resizing frustum to " << mPriTileMap[nre.getRecepient()].xPos << ", " << mPriTileMap[nre.getRecepient()].yPos << ", " << mPriTileMap[nre.getRecepient()].width << ", " << mPriTileMap[nre.getRecepient()].height << endl;
//	resizeWindow(height, width);
//	cout << "starting display of DataCore" << endl;
	// light blue
	glClear(GL_DEPTH_BUFFER_BIT);
	GET_GLERROR(0);
	glLoadIdentity();
	mPriCamera.setRotationMatrix(mPriModelViewMatrix);
	mPriCamera.calcMatrix();
	GET_GLERROR(0);

	// load all requested vbos
//	cout << "DataCore Loading VBOs:" << endl;
	const ooctools::Quintuple* currQuintuple; //= ooctools::Quadruple();
	std::list<const Quintuple*>::iterator quintIt = _quintList->begin();
	for (; quintIt != _quintList->end(); ++quintIt){
		currQuintuple = *quintIt;
		map<uint64_t, std::string>::iterator IT = mPriIdPathMap.find(currQuintuple->id);
		if(IT == mPriIdPathMap.end()) {
			cerr << "ID " << currQuintuple->id << " not in pathmap!" << endl;
			exit(0);
		}

//		cout << "  - VBO " << nre.getId(i) << "," << nre.getByteSize() << ", " << nre.getIdxCount() << endl;
//		cout << "  - PATH " << "/home/ava/Diplom/Model/Octree/data/"+mPriIdPathMap[nre.getId(i)]+".idx" << endl;
//		mPriVboMap.insert(make_pair(currQuintuple->id, new IndexedVbo(fs::path(string(BASE_MODEL_PATH)+"/data/"+mPriIdPathMap[currQuintuple->id]+".idx"), currQuintuple->id, false)));
//		mPriVboMap.insert(make_pair(currQuintuple->id, new IndexedVbo(mPriIdLocMap[currQuintuple->id], false)));
//		mPriDistanceMap.insert(make_pair(nre.getDistance(i), nre.getId(i)));
//		mPriQuadSet.insert(currQuadruple);
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
				mPriFbo->setDepthTex(mPriDepthTextures[_destId]);
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

				for(quintIt = _quintList->begin(); quintIt != _quintList->end(); ++quintIt, queryCount++){
					glBeginQuery(GL_SAMPLES_PASSED, mPriOccQueries[queryCount]);
						mPriIdLoMap[(*quintIt)->id]->getBb().drawSolidTriFan();
					glEndQuery(GL_SAMPLES_PASSED);
				}
				glDepthMask(GL_TRUE);

				// handle query-results
				queryCount = 0;
				for(quintIt = _quintList->begin(); quintIt != _quintList->end(); ++quintIt, queryCount++){
					GLint queryState = GL_FALSE;
					while(queryState != GL_TRUE){
					  glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT_AVAILABLE, &queryState);
					}

					glGetQueryObjectiv(mPriOccQueries[queryCount], GL_QUERY_RESULT, &mPriOccResults[(*quintIt)->id]);

					if (mPriOccResults[(*quintIt)->id]>0){
						mPriVisibilityList.push_back(ooctools::Visibility((*quintIt)->id, 1));
					}
					else {
						mPriVisibilityList.push_back(ooctools::Visibility((*quintIt)->id, 0));
					}
				}
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				mPriFbo->unbind();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();


	if (!mPriVisibilityList.empty()){
		// send the visible object to the requester
		OcclusionResultsEvent orlte = OcclusionResultsEvent(mPriVisibilityList);

		Message* msg = new Message(orlte, _destId);
		MpiControl::getSingleton()->push(msg);
	}
	// cleanup
//	cerr << "(" << MpiControl::getSingleton()->getRank() << ") iVboMapSize = " << mPriVboMap.size() << endl;

	mPriOccResults.clear();
	mPriVisibilityList.clear();
	glEnable(GL_CULL_FACE);


}

void DataCoreGlFrame::reshape(int width, int height) {
//	cout << "Window resized to: " << width << ", " << height << endl;
//	cout << "SIZE CHANGED" << endl;

	mProWindowWidth = width;
	mProWindowHeight = height;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(mProWindowHeight == 0)
		mProWindowHeight = 1;

	float ratio = 1.0* mProWindowWidth / mProWindowHeight;

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
	gluPerspective(45.0f, ratio, mProNearClippingPlane, mProFarClippingPlane);
//	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
	//resize
	initTiles();
}

//void DataCoreGlFrame::initTiles(bool extendFovy)
//{
//	//resize
//	float fovy = 45;
////	if (extendFovy){
////		fovy = mPriExtendedFovy;
////	}
////	else {
////		fovy = 45.0;
////	}
//
//	GLfloat oppFac = (GLfloat)800 / (GLfloat)640;
//	oppFac *= oppFac;
//	screenYMax = tan(fovy / 360.0 * ooctools::GeometricOps::PI) * mPriNearClippingPlane;
//	screenYMaxH = tan((fovy * ratio) / 360.0 * ooctools::GeometricOps::PI) * mPriNearClippingPlane;
//	screenYMax *= oppFac;
//	screenYMaxH *= oppFac;
//
////	ratio = (GLfloat)mPriWindowWidth / (GLfloat)mPriWindowHeight;
////	ratio = (GLfloat)800 / (GLfloat)600;
//	ratio = (GLfloat)640 / (GLfloat)480;
//
//	screenXMax = screenYMax * ratio;
//	screenYMin = -screenYMax;
//
//	screenXMaxH = screenYMaxH * ratio;
//	screenYMinH = -screenYMaxH;
//
//
//}
//
//void DataCoreGlFrame::resizeFrustum() {
//	this->resizeFrustum(0, 0, mPriWindowWidth, mPriWindowHeight);
//}
//
//void DataCoreGlFrame::resizeFrustum(unsigned _width, unsigned _height) {
//	this->resizeFrustum(0, 0, _width, _height);
//}
//
//void DataCoreGlFrame::resizeFrustum(unsigned tileXPos, unsigned tileYPos, unsigned tileswidth, unsigned tilesheight, bool extendFrustum)
//{
//	GET_GLERROR(0);
//
//
//	if (tilesheight == 0)
//		tilesheight = 1;
//	if (tileswidth == 0)
//		tileswidth = 1;
//	worldTopLine = (GLdouble) tileYPos / (GLdouble) mPriWindowHeight;
//	worldBottomLine = (GLdouble) (tileYPos + tilesheight) / (GLdouble) mPriWindowHeight;
//	worldLeftLine = (GLdouble) tileXPos / (GLdouble) mPriWindowWidth;
//	worldRightLine = (GLdouble) (tileXPos + tileswidth) / (GLdouble) mPriWindowWidth;
//
//	GET_GLERROR(0);
//	glViewport(0, 0, (GLint) 800, (GLint) 600);
////	glViewport(0, 0, (GLint) 640, (GLint) 480);
//	GET_GLERROR(0);
////	glViewport(0, 0, (GLint) tileswidth, (GLint) tilesheight);
//	GET_GLERROR(0);
//	glMatrixMode(GL_PROJECTION);
//	GET_GLERROR(0);
//	glLoadIdentity();
//	GET_GLERROR(0);
//
//	worldTopLine = screenYMin + ((screenYMax - screenYMin) * worldTopLine);
//	worldBottomLine = screenYMin
//			+ ((screenYMax - screenYMin) * worldBottomLine);
//
//	worldLeftLine = screenYMinH + ((screenYMaxH - screenYMinH) * worldLeftLine);
//	worldRightLine = screenYMinH + ((screenYMaxH - screenYMinH)
//			* worldRightLine);
//
//	GET_GLERROR(0);
//	glFrustum(worldLeftLine, worldRightLine, worldTopLine, worldBottomLine,
//			mPriNearClippingPlane, mPriFarClippingPlane);
//
//	GET_GLERROR(0);
//	glMatrixMode(GL_MODELVIEW);
//	GET_GLERROR(0);
//
//}

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

void DataCoreGlFrame::parseAndLoadVArraysMmap(const std::set<uint64_t>& _idSet)
{
	// --------------------------------------------
	fs::path path = fs::path(string(BASE_MODEL_PATH)+string(MODEL_DIR));
	unsigned fSize = 0;
	int fHandle = 0;
	char* map = 0;
	char* ptr = 0;
	uint64_t id = 0;
	unsigned indexCount = 0;
	unsigned vertexCount = 0;

	set<uint64_t>::iterator idIt;
	fs::directory_iterator end_itr; // default construction yields past-the-end
	for (fs::directory_iterator itr(path); itr != end_itr; ++itr) {
		if (fs::is_directory(itr->status())) {
			parseAndLoadVArraysMmap(_idSet);
		}
		else if (itr->path().extension() == ".bin") {
			fSize = fs::file_size(path);
			map = mapFile(itr->path(),fSize, fHandle);
			ptr = map;
			while (ptr < (map+fSize)){
				id = ((uint64_t*)ptr)[0];
				idIt = _idSet.find(id);
				if (idIt != _idSet.end()){
					mPriVArrayMap.insert(make_pair(id, new IndexedVertexArray(ptr)));
				}
				indexCount = ((unsigned*)(ptr+sizeof(uint64_t)))[0];
				vertexCount = ((unsigned*)(ptr+sizeof(uint64_t)))[1];
				ptr += sizeof(uint64_t)+(sizeof(unsigned)*2) + (indexCount*sizeof(unsigned)) + (vertexCount*sizeof(V4N4));
			}
			umapFile(map, fSize, fHandle);
		}
	}
}

void DataCoreGlFrame::parseAndLoadVArrays(const std::set<uint64_t>& _idSet)
{
	fs::path path = fs::path(string(BASE_MODEL_PATH)+string(MODEL_DIR));
	fs::ifstream inFile;
	unsigned pos = 0;
	unsigned size=0;
	ooctools::Location loc;
	uint64_t id = 0;
	unsigned indexCount = 0;
	unsigned vertexCount = 0;
	set<uint64_t>::iterator idIt;
	fs::directory_iterator end_itr; // default construction yields past-the-end
	for (fs::directory_iterator itr(path); itr != end_itr; ++itr) {
		if (fs::is_directory(itr->status())) {
			parseAndLoadVArrays(_idSet);
		}
		else if (itr->path().extension() == ".bin") {
//			cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " parsing file " << itr->path() << "..." << endl;
			inFile.open(itr->path(), ios::binary);
			inFile.seekg(0, ios::end);
			loc.path = itr->path();
			size = fs::file_size(itr->path());
			inFile.seekg(0, ios::beg);
			pos = 0;
			while (pos < size){
				inFile.read((char*)&id, sizeof(uint64_t));

				loc.position = pos;
//				cerr << "path " << loc.path << endl;
//				cerr << "pos " << loc.position << endl;
//				cerr << "size " << size << endl;
				idIt = _idSet.find(id);
//				cerr << "searching for ID " << id << "..." << endl;
				if (idIt != _idSet.end()){
					mPriVArrayMap.insert(make_pair(id, new IndexedVertexArray(&inFile, pos)));
//					cerr << "FOUND!" << endl;
				}

				inFile.seekg(pos+sizeof(uint64_t), ios::beg);
				inFile.read((char*)&indexCount, sizeof(unsigned));
				inFile.seekg(pos+sizeof(uint64_t)+(sizeof(unsigned)), ios::beg);
				inFile.read((char*)&vertexCount, sizeof(unsigned));
				pos += sizeof(uint64_t)+(sizeof(unsigned)*2) + (indexCount*sizeof(unsigned)) + (vertexCount*sizeof(V4N4));
//				cerr << itr->path() << ": jumping to position - " << pos << "/" << size << endl;
//				cerr << "indices: " << indexCount << endl;
//				cerr << "vertices: " << vertexCount << endl;
//				cerr << "size-sum: " << sizeof(uint64_t)+(sizeof(unsigned)*2) + (indexCount*sizeof(unsigned)) + (vertexCount*sizeof(V4N4)) << endl;
//				cerr << "new position: " << pos << endl;
				inFile.seekg(pos, ios::beg);
			}
			inFile.close();
			if (pos > size){
				cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " tries to jump to " << pos << "/" << size << endl;
			}
//			cerr << "DataNode " << MpiControl::getSingleton()->getRank() << " DONE with " << itr->path() << "..." << endl;
		}
	}
}

void DataCoreGlFrame::parseAndLoadVArrays(const char* _data, unsigned _arraySize, const std::set<uint64_t>& _idSet)
{
	uint64_t id = 0;
	unsigned indexCount = 0;
	unsigned vertexCount = 0;
	set<uint64_t>::iterator idIt;
	const char* dataPtr = _data;
	while (dataPtr < (_data+_arraySize)){
		id = ((uint64_t*)dataPtr)[0];
		idIt = _idSet.find(id);
		if (idIt != _idSet.end()){
			mPriVArrayMap.insert(make_pair(id, new IndexedVertexArray(dataPtr)));
		}
		indexCount = ((unsigned*)(dataPtr+sizeof(uint64_t)))[0];
		vertexCount = ((unsigned*)(dataPtr+sizeof(uint64_t)))[1];
		dataPtr += sizeof(uint64_t)+(sizeof(unsigned)*2) + (indexCount*sizeof(unsigned)) + (vertexCount*sizeof(V4N4));
	}
//	cerr << "datanode is DONE with this file!" << endl;
}

void DataCoreGlFrame::setMvMatrix(const float* matrix)
{
	memcpy(mPriModelViewMatrix, matrix, 16*sizeof(float));
}

char* DataCoreGlFrame::mapFile(fs::path _path, unsigned _fileSize, int& _fHandle)
{
	char *map;  /* mmapped array of int's */

	_fHandle = open(_path.string().c_str(), O_RDONLY);
	if (_fHandle == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
	map = (char*)mmap(0, _fileSize, PROT_READ, MAP_SHARED, _fHandle, 0);
	if (map == MAP_FAILED) {
		close(_fHandle);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	return map;
}

void DataCoreGlFrame::umapFile(char* _map, unsigned _fileSize, int& _fHandle)
{
	if (munmap(_map, _fileSize) == -1) {
		perror("Error un-mmapping the file");
	}

	close(_fHandle);
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
				mProFarClippingPlane*=2.0f;
				reshape(mProWindowWidth, mProWindowHeight);
			break;
			case 'N': // decrease far-clipping plane
				mProFarClippingPlane = max(mProFarClippingPlane/2.0f, 50.0f);
				reshape(mProWindowWidth, mProWindowHeight);
			break;
			default:
			break;
		}
	}
	else if (event.instanceOf(DepthBufferEvent::classid())){
		GET_GLERROR(0);

		DepthBufferEvent& dbe = (DepthBufferEvent&)event;
		mPriTileMap[dbe.getMpiRank()] = dbe.getTile();

//		cout << dbe.getMpiRank() << " ===> new depthbuffer dim: " << dbe.getX() << ", " << dbe.getY() << ", " << dbe.getWidth() << ", " << dbe.getHeight()<< endl;
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
//		cout << "(" << dbe.getMpiRank() << ") Tile: " << dbe.getTile().xPos << ", " << dbe.getTile().yPos << ", " << dbe.getTile().width << ", " <<dbe.getTile().height << endl;
//		cout << "(" << dbe.getMpiRank() << ") Depthbuffer: " << 0 << ", " << 0 << ", " << dbe.getWidth() << ", " <<dbe.getHeight() << endl;
		resizeFrustumExt(dbe.getTile().xPos, dbe.getTile().yPos, dbe.getTile().width, dbe.getTile().height, mProFarClippingPlane);
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
		cout << headerS.str() << "nearPlane: " << mProNearClippingPlane << endl;
		cout << headerS.str() << "farPlane: " << mProFarClippingPlane << endl;

		cout << "---------------------------------------" << endl;
	}
}
