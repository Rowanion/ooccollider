/**
 * @file	RenderMasterCore.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderMasterCore class definition.
 */

#include "RenderMasterCore.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "mpi.h"

#include <iostream>
#include <string>
#include <sstream>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "OOCWindow.h"
#include "MpiControl.h"
#include "Message.h"
#include "KillApplicationEvent.h"
#include "WindowResizedEvent.h"
#include "MouseButtonEvent.h"
#include "ModelViewMatrixEvent.h"
#include "KeyPressedEvent.h"
#include "WindowClosedEvent.h"
#include "ColorBufferEvent.h"
#include "MouseWheelEvent.h"
#include "SimpleGlFrame.h"
#include "InfoRequestEvent.h"
#include "DepthBufferRequestEvent.h"
#include "AccumulatedRendertimeEvent.h"
#include "ChangeTileDimensionsEvent.h"
#include "CameraMovedEvent.h"
#include "NodeRequestEvent.h"
#include "EndTransmissionEvent.h"
#include "JobDoneEvent.h"
#include "OcclusionRequestEvent.h"
#include "MemTools.h"
#include "Logger.h"
#include "Log.h"
#include "VboDistributionEvent.h"
#include "SceneCompletionEvent.h"
#include "KeyReleasedEvent.h"
#include "ResultsEvent.h"

#include <sys/mman.h>
#include <cstdio>
#include <fcntl.h>

namespace fs = boost::filesystem;
using namespace ooctools;
using namespace oocformats;
using namespace oocframework;
using namespace std;

RenderMasterCore* RenderMasterCore::instance = 0;

RenderMasterCore::RenderMasterCore(unsigned _width, unsigned _height) :
	mWindow(0), mRunning(true), mTerminateApplication(false), mPriCamHasMoved(
			true), mPriFrameCount(DEPTHBUFFER_INTERVAL), mPriRenderTimeCount(0), mPriOh(
			OctreeHandler()), mPriLo(0), mPriSTree(0),
			mPriRenderTimes(vector<double> (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER), 0.5)),
			mPriMpiCon(0), mPriDataLoad(map<int, unsigned>()), mPriFrameTick(0), mPriCCollisionTime(0.0),
			mPriQuintVec(std::vector<Quintuple>()), mPriOcclusionVec(std::vector<Quintuple>()),
			mPriMTwister(PRESELECTED_SEED), mPriCCol(PRESELECTED_SEED, LVL_OF_REDUNDANCY),
			mPriWindowWidth(_width), mPriWindowHeight(_height), mPriStepLeft(false), mPriStepRight(false),
			mPriStepForward(false), mPriStepBackward(false), mPriRollLeft(false), mPriRollRight(false),
			mPriTiltUp(false), mPriTiltDown(false), mPriTurnLeft(false), mPriTurnRight(false), mPriTurnUp(false),
			mPriTurnDown(false) {

	RenderMasterCore::instance = this;
	mWindow = new OOCWindow(_width, _height, 8, false, "MASTER_NODE");
	mWindow->enableKeyCallback();
	mWindow->enableMouseCallback();
	mPriGlFrame = new SimpleGlFrame();
	//	RenderMasterCoreGlFrame* glFrame = new RenderMasterCoreGlFrame();
	mWindow->attachGlFrame(mPriGlFrame);
	mPriEventMan = oocframework::EventManager::getSingleton();
	mPriEventMan->addListener(this, WindowResizedEvent::classid());
	mPriEventMan->addListener(this, KillApplicationEvent::classid());
	mPriEventMan->addListener(this, MouseButtonEvent::classid());
	mPriEventMan->addListener(this, MouseDraggedEvent::classid());
	mPriEventMan->addListener(this, KeyPressedEvent::classid());
	mPriEventMan->addListener(this, KeyReleasedEvent::classid());
	mPriEventMan->addListener(this, WindowClosedEvent::classid());
	mPriEventMan->addListener(this, InfoRequestEvent::classid());
	mPriEventMan->addListener(this, CameraMovedEvent::classid());

	mPriGlFrame->init();


	mPriMpiCon = MpiControl::getSingleton();
	for (unsigned i=0; i< mPriMpiCon->getGroupSize(MpiControl::DATA); ++i){
		mPriDataLoad.insert(make_pair(mPriMpiCon->getDataGroup()[i],0));
	}

	for (unsigned i=0; i< mPriMpiCon->getGroupSize(MpiControl::DATA); ++i){
		mPriNodeReqMap.insert(make_pair(mPriMpiCon->getDataGroup()[i], set<Quintuple>()));
	}

	int id = 0;
	mPriPosition = -1;
	mPriFPSTest = false;

	mPriRootTile.xPos = mPriRootTile.yPos = 0;
	mPriRootTile.width = _width;
	mPriRootTile.height = _height;
	mPriSTree = new Splittree(mPriMpiCon->getGroupSize(MpiControl::RENDERER),Splittree::VERTICAL,0,0,_width,_height,id);
	mPriSTree->split(mPriRenderTimes, mPriRootTile,mPriTileMap);

//	cout << "-----------------------1: " << newCoords[1].xPos << ", " << newCoords[1].yPos << ", " << newCoords[1].width << ", " << newCoords[1].height << endl;
//	cout << "-----------------------2: " << newCoords[2].xPos << ", " << newCoords[2].yPos << ", " << newCoords[2].width << ", " << newCoords[2].height << endl;
//	cout << "-----------------------3: " << newCoords[3].xPos << ", " << newCoords[3].yPos << ", " << newCoords[3].width << ", " << newCoords[3].height << endl;
//	cout << "-----------------------4: " << newCoords[4].xPos << ", " << newCoords[4].yPos << ", " << newCoords[4].width << ", " << newCoords[4].height << endl;
//	exit(0);

	mPriLo = mPriOh.loadLooseRenderOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));

	mPriCCol.generateDistribution(mPriLo);

#ifdef FPS_TEST
	stringstream fNameStream;
	fNameStream << "FPSWalkthroughTest_Redundance" << LVL_OF_REDUNDANCY << "_R" << mPriMpiCon->getGroupSize(MpiControl::RENDERER) << "_D" << mPriMpiCon->getGroupSize(MpiControl::DATA);
	fs::path logFile = fs::path(fNameStream.str() + ".log");
	Log l = Logger::getSingleton()->newLog(logFile);
	l.addCommentLine(string("fps each 20 frames"));
#endif

	//	glFrame->setVbo(new IndexedVbo(fs::path("/media/ClemensHDD/B3_SampleTree/data/0/1.idx")));


	// init tile-dimensions
	ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent();
	map<int, Tile>::iterator it = mPriTileMap.begin();
	for(; it != mPriTileMap.end(); ++it){
		ctde.setTileDimension(it->second);
		MpiControl::getSingleton()->send(new Message(ctde, it->first));
	}

#ifdef TRANSMIT_DISTRIBUTION

	transmitAssignments();

#endif



	// Main rendering loop
	unsigned frames = 0;
	mPriMpiCon->barrier();
	double newTime = 0.0;
	double newerTime = 0.0;


	do {
#ifdef DEBUG_INITIAL_SEND
			double newTime = glfwGetTime();
#endif

//		cout << "master sends outQueue and matrix" << endl;
		while (!mPriMpiCon->outQueueEmpty()) {
			//			cout << "master found that his outqueue is not empty.....sending...." << endl;
			mPriMpiCon->send();
		}
#ifdef DEBUG_INITIAL_SEND
			double newerTime = glfwGetTime();
			mPriCCollisionTime += (newerTime-newTime);
			if (mPriFrameTick % CCOLLISION_AVG == 0){
				cout << "(" << mPriMpiCon->getRank() << ") initial send took " << mPriCCollisionTime/CCOLLISION_AVG << " secs avg. over " << CCOLLISION_AVG << " frames." << endl;
				mPriCCollisionTime = 0.0;
			}
#endif
		if (!mTerminateApplication) {
			if (mPriFrameCount >= (DEPTHBUFFER_INTERVAL - 1) && mPriCamHasMoved == true){
//			if (mPriFrameCount >= (DEPTHBUFFER_INTERVAL - 1)){
#ifdef DEBUG_DEPTH_RESEND
			double newTime = glfwGetTime();
#endif
				newTime = glfwGetTime();
				mPriCamHasMoved = false;
				mPriFrameCount = 0;
				DepthBufferRequestEvent dbre = DepthBufferRequestEvent(mPriGlFrame->createMvMatrix());
				mPriMpiCon->send(new Message(dbre, 0, MpiControl::ALL));
//				cerr << "DepthBuffer RESEND initated by Master!" << endl;
				mPriMpiCon->barrier();
				// go into listenmode to receive the rendering-times
				mPriMpiCon->receive(MpiControl::RENDERER);
				while (!mPriMpiCon->inQueueEmpty()) {
					handleMsg(mPriMpiCon->pop());
				}
				newerTime = glfwGetTime();
//				cout << "[0] time between depthbuffer renewal: " << newerTime-newTime<< endl;
#ifdef DEBUG_DEPTH_RESEND
			double newerTime = glfwGetTime();
			mPriCCollisionTime += (newerTime-newTime);
			if (mPriFrameTick % DEPTH_AVG == 0){
				cout << "(" << mPriMpiCon->getRank() << ") Depth-resend took " << mPriCCollisionTime/DEPTH_AVG << " secs avg. over " << DEPTH_AVG << " frames." << endl;
				mPriCCollisionTime = 0.0;
			}
#endif
			}

			//send matrix/camera to when the out-queue is empty
			//			cout << "---master sending matrix" << endl;
			//			for (int i=1; i<mPriMpiCon->getSize(); ++i){
			ModelViewMatrixEvent mve = ModelViewMatrixEvent(mPriGlFrame->createMvMatrix());
			mPriMpiCon->send(new Message(mve, 0, MpiControl::RENDERER));
//			for (unsigned i = 0; i < mPriMpiCon->getGroupSize(
//					MpiControl::DATA); ++i) { // send the matrix to all data-nodes
//				Message* msg = new Message(ModelViewMatrixEvent::classid()->getShortId(),16*sizeof(float),MpiControl::getSingleton()->getDataGroup()[i],(char*)mPriGlFrame->getMvMatrix());
//				MpiControl::getSingleton()->send(msg);
//			}
//			ModelViewMatrixEvent mve = ModelViewMatrixEvent(mPriGlFrame->getMvMatrix());
//			for (unsigned i=0; i<MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER); ++i) { // send matrix to all renderers
//				Message* msg = new Message(mve, MpiControl::getSingleton()->getRenderGroup()[i]);
//				MpiControl::getSingleton()->send(msg);
//			}
			//			cout << "master has sent all matrices" << endl;

			//rcv kacheln from all renderers
			//			cout << "---master wating for tiles" << endl;

//			MpiControl::getSingleton()->receive(MpiControl::RENDERER); // receive a tile-image from renderer
//			while (!MpiControl::getSingleton()->inQueueEmpty()) {
//				handleMsg(MpiControl::getSingleton()->pop());
//			}
			manageCCollision(); // takes also care of tiles
//			cout << "[0] time for tile-sending and c-collision: " << newerTime-newTime<< endl;

//			adjustTileDimensions();

			//			cout << "---master ENTER display" << endl;
			mPriGlFrame->display();

			checkForActiveMovement();
#if defined(RENDER_TIME_TEST)
			mPriMpiCon->ireceive(MpiControl::RENDERER);
			mPriMpiCon->ireceive(MpiControl::DATA);
			if (!mPriMpiCon->inQueueEmpty()){
				handleMsg(mPriMpiCon->pop(), true);
			}
#endif
			mPriFrameCount++;


			//			cout << "---master EXIT display" << endl;
			//			cout << "end of display 0..." << endl;
		}
		else {
			mRunning=false;
		}
		//		for (int i=1; i<MpiControl::getSingleton()->getSize(); ++i){
		//			receiveQueue(MPI_ANY_SOURCE);
		//			if (mRunning)
		//				MpiControl::getSingleton()->send();
		//		}
		// Swap front and back buffers (we use a double buffered display)
		mWindow->flip();
		mWindow->poll();
		++mPriFrameTick;
		mPriFrameTick %= MODULO_FRAMECOUNT;
		++frames;
		if (frames >= 20) {
			frames = 0;
			stringstream ss;
#ifdef FPS_TEST
           if (mPriFPSTest){
              float fps = mPriGlFrame->getFrames();
              l.newTest();
              l << fps;
           }
#endif
			ss << "MasterNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << mPriGlFrame->getFrames();
			mWindow->setTitle(ss.str());
		}
//		mPriCCol.debug();
	}while (mRunning);
	cout << "end of display loop." << endl;
}

RenderMasterCore::~RenderMasterCore() {
	mPriEventMan->removeListener(this, WindowResizedEvent::classid());
	mPriEventMan->removeListener(this, KillApplicationEvent::classid());
	mPriEventMan->removeListener(this, MouseButtonEvent::classid());
	mPriEventMan->removeListener(this, MouseDraggedEvent::classid());
	mPriEventMan->removeListener(this, KeyPressedEvent::classid());
	mPriEventMan->removeListener(this, KeyReleasedEvent::classid());
	mPriEventMan->removeListener(this, WindowClosedEvent::classid());
	mPriEventMan->removeListener(this, InfoRequestEvent::classid());
	mPriEventMan->removeListener(this, CameraMovedEvent::classid());

	mRunning = false;
	delete mWindow;
	mWindow = 0;
	delete mPriSTree;
	mPriSTree = 0;
	Logger::getSingleton()->closeAllHandles();
}

void RenderMasterCore::Event(int event, bool state) {
	switch (event) {
	case EVENT_WINDOW_CLOSED:
		mRunning = false;
		break;
	case EVENT_WINDOW_RESIZED: {
		//		int count = 2*sizeof(int);
		//		char* data = new char[count];
		//		((int*)data)[0] = mWindow->getWidth();
		//		((int*)data)[1] = mWindow->getHeight();
		//		Message* msg = new Message(WindowResizedEvent::classid()->getShortId(),count,1,data);
		//		MpiControl::getSingleton()->push(msg);
		//		MpiControl::getSingleton()->sendAll();
		break;
	}
	default:
		//		cout << "Event fired: " << event << " with state: " << state << endl;
		break;
	}
}

MPI::Request RenderMasterCore::sendQueue(int dest) {
	MPI::Status status = MPI::Status();
	MPI::Request req;
	while (!RenderMasterCore::instance->mOutQueue.empty()) {
		//		cout << "RenderMasterCore sending..." << endl;
		int tag = 0;
		char* msg = RenderMasterCore::instance->mOutQueue.front();
		req = MPI::COMM_WORLD.Isend(msg, strlen(msg) + 1, MPI::CHAR, dest, tag);
		delete[] msg;
		RenderMasterCore::instance->mOutQueue.pop();
		//		cout << "RenderMasterCore send!!" << endl;
	}
	return req;

	//	cout << "sendThread killed." << endl;
}

void RenderMasterCore::handleMsg(oocframework::Message* msg, bool debug) {
	if (msg != 0) {
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()) {
//			cout << "KillApplicationRequest" << endl;
			mRunning = false;
			cout << "recveived kill! from " << msg->getSrc() << endl;
		} else if (msg->getType() == ColorBufferEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got ColorBuffer from" << msg->getSrc() << "!" << endl;
			}
//			cout << "ColorBufferRequest" << endl;
			//			cout << "recveived colorbuffer! " << msg->getSrc() << endl;

			ColorBufferEvent cbe = ColorBufferEvent(msg);
//			mPriTileMap[msg->getSrc()].renderTime = cbe.getRenderTime();
			oocframework::EventManager::getSingleton()->fire(cbe);

			//			const char* dat = msg->getData();
			//			std::cout << "w: " << ((int*)dat)[0] << std::endl;
			//			std::cout << "h: " << ((int*)dat)[1] << std::endl;
			//			std::cout << "x: " << ((int*)dat)[2] << std::endl;
			//			std::cout << "y: " << ((int*)dat)[3] << std::endl;
			//			std::cout << (int)((unsigned char*)(dat+4*sizeof(int)))[0] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[1] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[2] << std::endl;

			//			cout << "w: " << ((int*)msg->getData())[0] << endl;
			//			cout << "w: " << cbe.getWidth() << ", h: " << cbe.getHeight() << ", x: " << cbe.getX() << ", y: " << cbe.getY() << endl;
			//			mRunning = false;
		}
		else if (msg->getType() == SceneCompletionEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got SceneCompletion from" << msg->getSrc() << "!" << endl;
			}
			SceneCompletionEvent sce = SceneCompletionEvent(msg);
			// msg from a renderer that all requests have been sent
			mPriRendererComplete.push_back(sce.getSrc());
		}
		else if (msg->getType() == ResultsEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got Results from" << msg->getSrc() << "!" << endl;
			}
			ResultsEvent sce = ResultsEvent(msg);
#ifdef RENDER_TIME_TEST
			mPriSceneCompletion.push_back(sce.getResult(0));
			if (mPriSceneCompletion.size() >= mPriMpiCon->getGroupSize(MpiControl::DATA)){
				stringstream ss;
				ss << "ReloadTest_Redundance" << LVL_OF_REDUNDANCY << "_R" << mPriMpiCon->getGroupSize(MpiControl::RENDERER) << "_D" << mPriMpiCon->getGroupSize(MpiControl::DATA);
				fs::path logFile = fs::path(ss.str() + ".log");
				Log l = Logger::getSingleton()->newLog(logFile);
				l.addCommentLine(string("CamPos,DataNode_i,...,DataNode_n,avgTime,medianTime"));
				l.newTest();
				vector<double>::iterator vIt = mPriSceneCompletion.begin();
				cerr << "Render-Time-Test completed!" << endl;
				l << mPriPosition;
				double avg = 0.0;
				double median = 0.0;
				sort(mPriSceneCompletion.begin(),mPriSceneCompletion.end());
				unsigned size = mPriSceneCompletion.size();
				if (size%2 == 0){
					median = (mPriSceneCompletion[size/2] + mPriSceneCompletion[(size/2)-1])/2.0;
				}
				else {
					median = mPriSceneCompletion[(size-1)/2];
				}

				for (; vIt != mPriSceneCompletion.end(); ++vIt){
//					cerr << "DataNode " << mIt->first << ": " << mIt->second << " seconds." << endl;
					double tmp = *vIt;
					l << tmp;
					avg += *vIt;
				}
				avg = (avg/mPriMpiCon->getGroupSize(MpiControl::DATA));
				l << avg;
				l << median;
//				l << NL;
				mPriSceneCompletion.clear();
			}
#endif
		}
		else if (msg->getType() == AccumulatedRendertimeEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got AccumulatedRenderTime from" << msg->getSrc() << "!" << endl;
			}

//			cout << "AccumulatedRendertimeRequest" << endl;
			mPriRenderTimes[msg->getSrc()-1] = ((double*)msg->getData())[0];
//			mPriTileMap[msg->getSrc()].renderTime = ((double*)msg->getData())[0];
			mPriRenderTimeCount++;
			if (mPriRenderTimeCount>= MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER)){
				mPriRenderTimeCount = 0;
				mPriTileMap.clear();
				mPriSTree->split(mPriRenderTimes, mPriRootTile, mPriTileMap);
//				adjustTileDimensions();
				ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent();
				map<int, Tile>::iterator it = mPriTileMap.begin();
				for(; it != mPriTileMap.end(); ++it){
					ctde.setTileDimension(it->second);
					MpiControl::getSingleton()->send(new Message(ctde, it->first));
				}
//				cout << "Master " << mPriMpiCon->getRank() << " waiting at barrier bevor deptbuffers" << endl;
				mPriMpiCon->barrier();
//				cout << "Master " << mPriMpiCon->getRank() << " continuing" << endl;


			}
			// store render-time, inc counter
			// check for counter and if = max recalc tile-dims
		}
		else if (msg->getType() == NodeRequestEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got NodeRequest from" << msg->getSrc() << "!" << endl;
			}
//			cout << "NodeRequest" << endl;
			//TODO implement c-Collision
			// at the moment it just passes the request to the only datanode
			NodeRequestEvent nre = NodeRequestEvent(msg);
			for (unsigned i = 0; i< nre.getIdxCount(); ++i){
				mPriQuintVec.push_back(*nre.getQuintuple(i));
			}
		}
		else if (msg->getType() == OcclusionRequestEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got OcclusionRequest from" << msg->getSrc() << "!" << endl;
			}
//			cout << "NodeRequest" << endl;
			//TODO implement c-Collision
			// at the moment it just passes the request to the only datanode
			OcclusionRequestEvent ore = OcclusionRequestEvent(msg);
			for (unsigned i = 0; i< ore.getIdxCount(); ++i){
				mPriOcclusionVec.push_back(*ore.getQuintuple(i));
			}
		}
		else if (msg->getType() == EndTransmissionEvent::classid()->getShortId()) {
			if (debug){
				cerr << "Master got EndTransmission from" << msg->getSrc() << "!" << endl;
			}
//			cout << "EndTransmission" << endl;
			mPriRendererDoneCount++;
		}
		else if (msg->getType() == JobDoneEvent::classid()->getShortId()) {
			//			cout << "MASTER got JobDone MESSAGE!!!" << endl;
			JobDoneEvent jde = JobDoneEvent(msg);
			mPriDataLoad[msg->getSrc()] -= jde.getJobCount();
			cout << "jobs pending: " << mPriDataLoad[msg->getSrc()] << endl;
		}
		else{
			cout << "MASTER got unclassified MESSAGE!!!" << endl;
		}
		delete msg;
		msg = 0;
	}
}

void RenderMasterCore::manageCCollision()
{
	mPriRendererDoneCount = 0;
	while (mPriRendererDoneCount < (mPriMpiCon->getGroupSize(MpiControl::RENDERER))){
//	while (mPriRendererDoneCount < (mPriMpiCon->getGroupSize(MpiControl::RENDERER)*2)){
		mPriMpiCon->ireceive(MpiControl::ANY);
		while(!mPriMpiCon->inQueueEmpty()){
			handleMsg(mPriMpiCon->pop());
		}
	}
	mPriMpiCon->completeWaitingReceives();
	while(!mPriMpiCon->inQueueEmpty()){
		handleMsg(mPriMpiCon->pop());
	}
#ifdef DEBUG_CCOLLISION
			double newTime = glfwGetTime();
#endif

	// distribution of requests from here
	if (!mPriQuintVec.empty()){
		if (mPriRequestSave){
			unsigned length = mPriQuintVec.size();
			mPriRequestOutFile->write((char*)&length, sizeof(unsigned));
			for (unsigned i=0; i<length; ++i){
				mPriRequestOutFile->write((char*)&(mPriQuintVec[i]), sizeof(ooctools::Quintuple));
			}
			mPriRequestOutFile->flush();
		}
#ifdef C_COLLISION_BALANCE_TEST
		stringstream ss;
		ss << "LoadBalanceTest_Redundance" << LVL_OF_REDUNDANCY << "_R" << mPriMpiCon->getGroupSize(MpiControl::RENDERER) << "_D" << mPriMpiCon->getGroupSize(MpiControl::DATA);
		fs::path logFile = fs::path(ss.str() + ".log");
		Log l = Logger::getSingleton()->newLog(logFile);
		l.addCommentLine(string("pos, loadDataNode_0, loadDataNode_1,.., loadDataNode_n, avgLoad, ..."));
		l << mPriPosition;
		mPriCCol.doCCollision(&mPriQuintVec, &mPriNodeReqMap, l);
#else
		mPriCCol.doCCollision(&mPriQuintVec, &mPriNodeReqMap);
#endif
		map<int, set<Quintuple> >::iterator intQuintIt = mPriNodeReqMap.begin();
		for (; intQuintIt != mPriNodeReqMap.end(); ++intQuintIt){
			NodeRequestEvent nre = NodeRequestEvent(intQuintIt->second, mPriGlFrame->getMvMatrix());
			mPriMpiCon->isend(new Message(nre,intQuintIt->first));
			mPriDataLoad[intQuintIt->first]+=nre.getIdxCount();
		}
		mPriNodeReqMap.clear();
		mPriQuintVec.clear();
		mPriQuintVec.resize(0);
	}
	if (!mPriRendererComplete.empty()){
		list<int>::iterator lIt = mPriRendererComplete.begin();
		for (; lIt != mPriRendererComplete.end();){
			SceneCompletionEvent sce = SceneCompletionEvent(*lIt);
			mPriMpiCon->isend(new Message(sce,0, MpiControl::DATA));
			mPriRendererComplete.erase(lIt++);
		}
	}

	// distribution of re-requests from here
//	if (!mPriOcclusionVec.empty()){
//		mPriCCol.doCCollision(&mPriOcclusionVec, &mPriOcclusionReqMap);
//		map<int, set<Quintuple> >::iterator intQuintIt = mPriOcclusionReqMap.begin();
//		for (; intQuintIt != mPriOcclusionReqMap.end(); ++intQuintIt){
//			OcclusionRequestEvent ore = OcclusionRequestEvent(intQuintIt->second, mPriGlFrame->getMvMatrix());
//			mPriMpiCon->isend(new Message(ore,intQuintIt->first));
//			mPriDataLoad[intQuintIt->first]+=ore.getIdxCount();
//		}
//		mPriOcclusionReqMap.clear();
//		mPriOcclusionVec.clear();
//	}

#ifdef DEBUG_CCOLLISION
			double newerTime = glfwGetTime();
			mPriCCollisionTime += (newerTime-newTime);
			if (mPriFrameTick % CCOLLISION_AVG == 0){
				cout << "(" << mPriMpiCon->getRank() << ") c-collision took " << mPriCCollisionTime/CCOLLISION_AVG << " secs avg. over " << CCOLLISION_AVG << " frames." << endl;
				mPriCCollisionTime = 0.0;
			}
#endif


}

char* RenderMasterCore::mapFile(fs::path _path, unsigned _fileSize, int& _fHandle)
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

void RenderMasterCore::umapFile(char* _map, unsigned _fileSize, int& _fHandle)
{
	if (munmap(_map, _fileSize) == -1) {
		perror("Error un-mmapping the file");
	}

	close(_fHandle);
}

void RenderMasterCore::transmitAssignments()
{
	cerr << "transmit enabled!" << endl;
	// submit all assigned node-ids to the respective data-nodes.
	for (unsigned i=0; i <mPriMpiCon->getDataGroup().size(); ++i){
		VboDistributionEvent vde = VboDistributionEvent(mPriCCol.getNodeSet(mPriMpiCon->getDataGroup()[i]));
		mPriMpiCon->isend(new Message(vde,mPriMpiCon->getDataGroup()[i]));
		cerr << "master send " << vde.getIdCount() << " ds to datanode " << mPriMpiCon->getDataGroup()[i] << endl;
		for (uint64_t i=0; i<10; ++i){
			cerr << "master: id " << vde.getId(i) << endl;
		}
	}

	// send the actual data
	// iterate over all data-files, map them and send them
	fs::path path = fs::path(string(BASE_MODEL_PATH)+string(MODEL_DIR));
	fs::directory_iterator end_itr; // default construction yields past-the-end
	unsigned fSize = 0;
	int fHandle;
	char *map = 0;
	for (fs::directory_iterator itr(path); itr != end_itr; ++itr) {
		if (itr->path().extension() == ".bin") {
			fSize = fs::file_size(itr->path());
			cerr << "mappping file " << itr->path() << "..." << endl;
			map = mapFile(itr->path(), fSize, fHandle);
			cerr << "done. now send it....." << endl;
			MPI::COMM_WORLD.Bcast(&fSize, sizeof(unsigned), MPI_CHAR, 0);
			MPI::COMM_WORLD.Bcast(map, fSize, MPI_CHAR, 0);
			cerr << "done. now unmap it....." << endl;
			umapFile(map, fSize, fHandle);
		}
	}
	fSize = 0;
	MPI::COMM_WORLD.Bcast(&fSize, sizeof(unsigned), MPI_CHAR, 0);


}

void RenderMasterCore::checkForActiveMovement()
{
	if (mPriTiltUp || mPriTiltDown || mPriTurnUp || mPriTurnDown ||
			mPriTurnLeft || mPriTurnRight || mPriStepForward || mPriStepBackward ||
			mPriStepLeft || mPriStepRight || mPriRollLeft ||
			mPriRollRight){
		mPriCamHasMoved = true;
	}
}

void RenderMasterCore::notify(oocframework::IEvent& event) {
	if (event.instanceOf(WindowResizedEvent::classid())) {
		WindowResizedEvent& rwe = (WindowResizedEvent&) event;

	} else if (event.instanceOf(KeyReleasedEvent::classid())) {
		KeyReleasedEvent& kre = (KeyReleasedEvent&) event;
		switch (kre.getKey()){
			case GLFW_KEY_PAGEUP: // tilt up
				mPriTiltUp = false;
			break;
			case GLFW_KEY_PAGEDOWN: // tilt down
				mPriTiltDown = false;
			break;

			case GLFW_KEY_UP: // walk forward (bob head)
				mPriTurnUp = false;
			break;

			case GLFW_KEY_DOWN: // walk back (bob head)
				mPriTurnDown = false;
			break;

			case GLFW_KEY_LEFT: // look left(int)
				mPriTurnLeft = false;
			break;

			case GLFW_KEY_RIGHT: // look right
				mPriTurnRight = false;
			break;
		    case (int)'W':
				mPriStepForward = false;
		    break;
		    case (int)'S':
				mPriStepBackward = false;
		        break;
		    case (int)'A':
				mPriStepLeft = false;
		        break;
		    case (int)'D':
				mPriStepRight = false;
		        break;
		    case (int)'Q':
				mPriRollLeft = false;
		        break;
		    case (int)'E':
				mPriRollRight = false;
	        break;
		}
	} else if (event.instanceOf(KeyPressedEvent::classid())) {
		KeyPressedEvent& kpe = (KeyPressedEvent&) event;
		switch (kpe.getKey()) {
        case '0':
                 mPriPosition = 0;
                 cerr << "Jumping to camera position " << 10 << endl;
                 break;
         case '1':
                 mPriPosition = 1;
                 cerr << "Jumping to camera position " << 1 << endl;
                 break;
         case '2':
                 mPriPosition = 2;
                 cerr << "Jumping to camera position " << 2 << endl;
                 break;
         case '3':
                 mPriPosition = 3;
                 cerr << "Jumping to camera position " << 3 << endl;
                 break;
         case '4':
                 mPriPosition = 4;
                 cerr << "Jumping to camera position " << 4 << endl;
                 break;
         case '5':
                 mPriPosition = 5;
                 cerr << "Jumping to camera position " << 5 << endl;
			break;
		case '6':
			mPriPosition = 6;
            cerr << "Jumping to camera position " << mPriPosition << endl;
			break;
		case '7':
			mPriPosition = 7;
            cerr << "Jumping to camera position " << mPriPosition << endl;
			break;
		case '8':
			mPriPosition = 8;
            cerr << "Jumping to camera position " << mPriPosition << endl;
			break;
		case '9':
			mPriPosition = 9;
            cerr << "Jumping to camera position " << mPriPosition << endl;
			break;
		case GLFW_KEY_ESC:{
			KillApplicationEvent kae = KillApplicationEvent();
			mPriMpiCon->push(new Message(kae, 0, MpiControl::DATA));
			mPriMpiCon->push(new Message(kae, 0, MpiControl::RENDERER));
			mTerminateApplication = true;
			break;}
		case GLFW_KEY_KP_ADD:
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::DATA));
			break;
		case GLFW_KEY_KP_SUBTRACT:
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::DATA));
			break;
		case 'N':
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::ALL));
			mPriCamHasMoved = true;
			break;
        case 'K':
                mPriFPSTest = !mPriFPSTest;
                break;
		case 'F':
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::ALL));
			mPriCamHasMoved = true;
			break;
		case 'I': {
			cout << "====================================" << endl;
			cout << "Requesting Information from Nodes..." << endl;
			cout << "====================================" << endl;
			InfoRequestEvent ire = InfoRequestEvent();
			mPriMpiCon->push(new Message(ire, 0,
					MpiControl::ALL));
			oocframework::EventManager::getSingleton()->fire(ire);
			break;
		}
		case 'R':
			// do not update the depthbuffer
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			break;
		case 'X':{
			if (kpe.isCtrl()){
//				// RELOADTEST
//				mPriSceneCompletion.clear();
//				for (unsigned i = 0; i < mPriMpiCon->getDataGroup().size(); i++){
//					mPriSceneCompletion.insert(make_pair(mPriMpiCon->getDataGroup()[i], glfwGetTime()));
//				}
//				mPriSceneCompletionCount = 0;
			}
			mPriCamHasMoved = true;
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::ALL));
//			SceneCompletionEvent sce = SceneCompletionEvent();
//			mPriMpiCon->push(new Message(sce, 1, MpiControl::DATA));
			break;}
		case 'Y':
			// code to start/stop request-save
			if (!mPriRequestSave){
				cerr << "STARTing request-recording." << endl;
				mPriRequestOutFile = new fs::ofstream();
				mPriRequestOutFile->open(fs::path("requestFile.bin"), ios::binary);
				mPriRequestOutFile->seekp(0, ios::beg);
			}
			else {
				cerr << "STOPing request-recording." << endl;
				mPriRequestOutFile->flush();
				mPriRequestOutFile->close();
				delete mPriRequestOutFile;
			}
			mPriRequestSave = !mPriRequestSave;
			break;
		case GLFW_KEY_PAGEUP: // tilt up
			mPriTiltUp = true;
			break;
		case GLFW_KEY_PAGEDOWN: // tilt down
			mPriTiltDown = true;
			break;

		case GLFW_KEY_UP: // walk forward (bob head)
			mPriTurnUp = true;
			break;

		case GLFW_KEY_DOWN: // walk back (bob head)
			mPriTurnDown = true;
			break;

		case GLFW_KEY_LEFT: // look left(int)
			mPriTurnLeft = true;
			break;

		case GLFW_KEY_RIGHT: // look right
			mPriTurnRight = true;
			break;
		case (int)'W':
			mPriStepForward = true;
		break;
		case (int)'S':
			mPriStepBackward = true;
		break;
		case (int)'A':
			mPriStepLeft = true;
		break;
		case (int)'D':
			mPriStepRight = true;
		break;
		case (int)'Q':
			mPriRollLeft = true;
		break;
		case (int)'E':
			mPriRollRight = true;
		break;

		default:
			mPriMpiCon->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			mPriCamHasMoved = true;
			break;
		}
	}
	else if (event.instanceOf(MouseDraggedEvent::classid())){
		mPriCamHasMoved = true;
	}
	else if (event.instanceOf(CameraMovedEvent::classid())){
		mPriCamHasMoved = true;
	}
	else if (event.instanceOf(MouseButtonEvent::classid())){
		mPriCamHasMoved = true;
	}
	else if (event.instanceOf(WindowClosedEvent::classid())) {
		KillApplicationEvent kae = KillApplicationEvent();
		mPriMpiCon->push(new Message(kae, 0, MpiControl::DATA));
		mPriMpiCon->push(new Message(kae, 0, MpiControl::RENDERER));
		mTerminateApplication = true;
	}
}

