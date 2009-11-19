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
#include "RenderMasterCoreGlFrame.h"
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
			mPriQuintVec(std::vector<Quintuple>()), mPriMTwister(PRESELECTED_SEED), mPriCCol(PRESELECTED_SEED, 2),
			mPriWindowWidth(_width), mPriWindowHeight(_height) {

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

	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));

	mPriCCol.generateDistribution(mPriLo);


	//	glFrame->setVbo(new IndexedVbo(fs::path("/media/ClemensHDD/B3_SampleTree/data/0/1.idx")));


	// init tile-dimensions
	ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent();
	map<int, Tile>::iterator it = mPriTileMap.begin();
	for(; it != mPriTileMap.end(); ++it){
		ctde.setTileDimension(it->second);
		MpiControl::getSingleton()->send(new Message(ctde, it->first));
	}



	// Main rendering loop
	unsigned frames = 0;
	mPriMpiCon->barrier();
	double newTime = 0.0;
	double newerTime = 0.0;


	do {
//		cout << "master sends outQueue and matrix" << endl;
		while (!mPriMpiCon->outQueueEmpty()) {
			//			cout << "master found that his outqueue is not empty.....sending...." << endl;
			mPriMpiCon->send();
		}

		if (!mTerminateApplication) {
			if (mPriFrameCount >= (DEPTHBUFFER_INTERVAL - 1) && mPriCamHasMoved == true){
				newTime = glfwGetTime();
				mPriCamHasMoved = false;
				mPriFrameCount = 0;
				DepthBufferRequestEvent dbre = DepthBufferRequestEvent();
				mPriMpiCon->send(new Message(dbre, 0, MpiControl::ALL));
				mPriMpiCon->barrier();
				// go into listenmode to receive the rendering-times
				mPriMpiCon->receive(MpiControl::RENDERER);
				while (!mPriMpiCon->inQueueEmpty()) {
					handleMsg(mPriMpiCon->pop());
				}
				newerTime = glfwGetTime();
//				cout << "[0] time between depthbuffer renewal: " << newerTime-newTime<< endl;
			}

			//send matrix/camera to when the out-queue is empty
			//			cout << "---master sending matrix" << endl;
			//			for (int i=1; i<mPriMpiCon->getSize(); ++i){
			ModelViewMatrixEvent mve = ModelViewMatrixEvent(mPriGlFrame->getMvMatrix());
			mPriMpiCon->send(new Message(mve, 0, MpiControl::ALL));
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
		if (frames >= 100) {
			frames = 0;
			stringstream ss;
			ss << "MasterNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << mPriGlFrame->getFrames();
			mWindow->setTitle(ss.str());
		}

	}while (mRunning);
	cout << "end of display loop." << endl;
}

RenderMasterCore::~RenderMasterCore() {
	mPriEventMan->removeListener(this, WindowResizedEvent::classid());
	mPriEventMan->removeListener(this, KillApplicationEvent::classid());
	mPriEventMan->removeListener(this, MouseButtonEvent::classid());
	mPriEventMan->removeListener(this, MouseDraggedEvent::classid());
	mPriEventMan->removeListener(this, KeyPressedEvent::classid());
	mPriEventMan->removeListener(this, WindowClosedEvent::classid());
	mPriEventMan->removeListener(this, InfoRequestEvent::classid());
	mPriEventMan->removeListener(this, CameraMovedEvent::classid());

	mRunning = false;
	delete mWindow;
	mWindow = 0;
	delete mPriSTree;
	mPriSTree = 0;
}

void RenderMasterCore::Event(int event, bool state) {
	switch (event) {
	case EVENT_WINDOW_CLOSED:
		mRunning = false;
		break;
	case GLFW_KEY_F1:
		((RenderMasterCoreGlFrame*) mWindow->getGlFrame())->debug();
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

void RenderMasterCore::handleMsg(oocframework::Message* msg) {
	if (msg != 0) {
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()) {
//			cout << "KillApplicationRequest" << endl;
			mRunning = false;
			cout << "recveived kill! from " << msg->getSrc() << endl;
		} else if (msg->getType() == ColorBufferEvent::classid()->getShortId()) {
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
		else if (msg->getType() == AccumulatedRendertimeEvent::classid()->getShortId()) {
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
//			cout << "NodeRequest" << endl;
			//TODO implement c-Collision
			// at the moment it just passes the request to the only datanode
			NodeRequestEvent nre = NodeRequestEvent(msg);
			for (unsigned i = 0; i< nre.getIdxCount(); ++i){
				mPriQuintVec.push_back(*nre.getQuintuple(i));
			}
		}
		else if (msg->getType() == EndTransmissionEvent::classid()->getShortId()) {
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
	while (mPriRendererDoneCount<mPriMpiCon->getGroupSize(MpiControl::RENDERER)){
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
		mPriCCol.doCCollision(&mPriQuintVec, &mPriNodeReqMap);
		map<int, set<Quintuple> >::iterator intQuintIt = mPriNodeReqMap.begin();
		for (; intQuintIt != mPriNodeReqMap.end(); ++intQuintIt){
			NodeRequestEvent nre = NodeRequestEvent(intQuintIt->second);
			mPriMpiCon->isend(new Message(nre,intQuintIt->first));
			mPriDataLoad[intQuintIt->first]+=nre.getIdxCount();
		}
		mPriNodeReqMap.clear();
		mPriQuintVec.clear();

//		// simple netdistribution with modulo 2
//		vector<Quintuple> quintSet0 = vector<Quintuple>();
//		vector<Quintuple> quintSet1 = vector<Quintuple>();
//		vector<Quintuple>::iterator quintIt = mPriQuintVec.begin();
//		for (; quintIt != mPriQuintVec.end(); ++quintIt){
//			if (quintIt->id%2 == 0){
//				quintSet0.push_back(*quintIt);
//			}
//			else {
//				quintSet1.push_back(*quintIt);
//			}
//		}
//		if (!quintSet0.empty()){
//			NodeRequestEvent nre = NodeRequestEvent(quintSet0);
//			mPriMpiCon->isend(new Message(nre,mPriMpiCon->getDataGroup()[0]));
//			mPriDataLoad[mPriMpiCon->getDataGroup()[0]]+=nre.getIdxCount();
//		}
//		if (!quintSet1.empty()){
//			NodeRequestEvent nre = NodeRequestEvent(quintSet1);
//			mPriMpiCon->isend(new Message(nre,mPriMpiCon->getDataGroup()[1]));
//			mPriDataLoad[mPriMpiCon->getDataGroup()[0]]+=nre.getIdxCount();
//		}
////		cout << "assigning " << nre.getIdxCount() << " jobs" << endl;
//		mPriQuintVec.clear();
	}
#ifdef DEBUG_CCOLLISION
			double newerTime = glfwGetTime();
			mPriCCollisionTime += (newerTime-newTime);
			if (mPriFrameTick % CCOLLISION_AVG == 0){
				cout << "(" << mPriMpiCon->getRank() << ") c-collision took " << mPriCCollisionTime/CCOLLISION_AVG << " secs avg. over " << CCOLLISION_AVG << " frames." << endl;
				mPriCCollisionTime = 0.0;
			}
#endif


}

void RenderMasterCore::notify(oocframework::IEvent& event) {
	if (event.instanceOf(WindowResizedEvent::classid())) {
		WindowResizedEvent& rwe = (WindowResizedEvent&) event;

	} else if (event.instanceOf(KeyPressedEvent::classid())) {
		KeyPressedEvent& kpe = (KeyPressedEvent&) event;
		switch (kpe.getKey()) {
		case GLFW_KEY_ESC:{
			KillApplicationEvent kae = KillApplicationEvent();
			MpiControl::getSingleton()->push(new Message(kae, 0, MpiControl::DATA));
			MpiControl::getSingleton()->push(new Message(kae, 0, MpiControl::RENDERER));
			mTerminateApplication = true;
			break;}
		case GLFW_KEY_KP_ADD:
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::DATA));
			break;
		case GLFW_KEY_KP_SUBTRACT:
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::DATA));
			break;
		case 'N':
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::ALL));
			mPriCamHasMoved = true;
			break;
		case 'F':
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::ALL));
			mPriCamHasMoved = true;
			break;
		case 'I': {
			cout << "====================================" << endl;
			cout << "Requesting Information from Nodes..." << endl;
			cout << "====================================" << endl;
			InfoRequestEvent ire = InfoRequestEvent();
			MpiControl::getSingleton()->push(new Message(ire, 0,
					MpiControl::ALL));
			oocframework::EventManager::getSingleton()->fire(ire);
			break;
		}
		case 'R':
			// do not update the depthbuffer
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::RENDERER));
			break;
		default:
			mPriCamHasMoved = true;
			MpiControl::getSingleton()->push(new Message(kpe, 1,
					MpiControl::RENDERER));
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
		MpiControl::getSingleton()->push(new Message(kae, 0, MpiControl::DATA));
		MpiControl::getSingleton()->push(new Message(kae, 0, MpiControl::RENDERER));
		mTerminateApplication = true;
	}
}

