/**
 * @file	RenderCore.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderCore class definition.
 */

#include "RenderCore.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "mpi.h"

#include <iostream>
#include <string>
#include <sstream>

#include "OOCWindow.h"
#include "RenderCoreGlFrame.h"
#include "MpiControl.h"
#include "Message.h"
#include "KillApplicationEvent.h"
#include "WindowResizedEvent.h"
#include "ModelViewMatrixEvent.h"
#include "KeyPressedEvent.h"
#include "ColorBufferEvent.h"
#include "DepthBufferEvent.h"
#include "VboEvent.h"
#include "InfoRequestEvent.h"
#include "DepthBufferRequestEvent.h"
#include "ChangeTileDimensionsEvent.h"
#include "AccumulatedRendertimeEvent.h"
#include "OcclusionResultsEvent.h"
#include "MemTools.h"
#include "Logger.h"
#include "Log.h"
#include "EndTransmissionEvent.h"


using namespace std;
using namespace oocframework;

RenderCore* RenderCore::instance = 0;

RenderCore::RenderCore(unsigned _winWidth, unsigned _winHeight, unsigned _targetWidth, unsigned _targetHeight) :
		mWindow(0), mRunning(true), mPriGotMatrix(false)
{
	RenderCore::instance = this;
	mPriMpiCon = MpiControl::getSingleton();

	//		setupWindow("My rank is NOT 0");
	stringstream title;
	title << "RenderNode (" << mPriMpiCon->getRank() << ")";
	mWindow = new OOCWindow(_winWidth, _winHeight, 8, false, title.str().c_str());
//	mWindow->enableKeyCallback();
	mPriGlFrame = new RenderCoreGlFrame(_winWidth, _winHeight, _targetWidth, _targetHeight);
	mWindow->attachGlFrame(mPriGlFrame);
	GET_GLERROR(0);

	// get the initial tile-dimensions
	mPriMpiCon->receive(0);
	while(!mPriMpiCon->inQueueEmpty()){ //
		Message* msg = mPriMpiCon->pop();
		if (msg->getType() == ChangeTileDimensionsEvent::classid()->getShortId()){
			ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent(msg);
			mPriGlFrame->setTileDimensions(ctde.getTileXPos(), ctde.getTileYPos(), ctde.getTileWidth(), ctde.getTileWidth());
		}
		delete msg;
		msg = 0;
	}

	mPriGlFrame->init();
	GET_GLERROR(0);
	mPriGlFrame->reshape(_winWidth, _winHeight);
	mPriGlFrame->initTiles();
	GET_GLERROR(0);

	mPriReloadTest = false;
	mPriReloadStart = 0.0;
	mPriReloadStop = 0.0;

	// Main rendering loop
	unsigned frames = 0;

	oocframework::EventManager::getSingleton()->addListener(this, EndTransmissionEvent::classid());

	cerr << "Mem BEFORE start " << MpiControl::getSingleton()->getRank() <<  ": " << MemTools::getSingleton()->usedMem() << endl;

#ifdef TRANSMIT_DISTRIBUTION
	char* buffer = 0;
	unsigned bufferSize = 0;
	cerr << "Renderer " << mPriMpiCon->getRank() << " standing by for BroadCast-Ignore...." << endl;
	MPI::COMM_WORLD.Bcast(&bufferSize, sizeof(unsigned), MPI_CHAR, 0);
	while(bufferSize != 0){
		// the buffer itself
		buffer = new char[bufferSize];
		MPI::COMM_WORLD.Bcast(buffer, bufferSize, MPI_CHAR, 0);
		delete buffer;
		buffer = 0;
		// the new bufferSize
		MPI::COMM_WORLD.Bcast(&bufferSize, sizeof(unsigned), MPI_CHAR, 0);
	}
	cerr << "Renderer " << mPriMpiCon->getRank() << " successfully ignored all BroadCasts." << endl;
#endif

	mPriMpiCon->barrier();
	do {
		mPriGotMatrix = false;
//		cout << "renderer waiting for matrix 'n stuff" << endl;
		while(!mPriGotMatrix && mRunning){ // receive everything from 0 and finally the matrix
			mPriMpiCon->receive(0);
			handleMsg(mPriMpiCon->pop());
		}
//		cout << "renderer got matrix" << endl;
		if (mRunning){

			//		cout << "renderer sending colorbuffer" << endl;
			mPriMpiCon->isend(new Message(mPriGlFrame->getColorBufferEvent(), 0));
			//		cout << "renderer culling and requesting" << endl;
			mPriGlFrame->cullFrustum();

			//		cout << "renderer checking for data-input" << endl;
			MpiControl::getSingleton()->ireceive(MpiControl::DATA);

			if (frames == 30){
				mPriGlFrame->reloadOnline();
			}

			while(!mPriMpiCon->inQueueEmpty()){ // ireceive everything from data-nodes
				Message* msg = mPriMpiCon->pop();
				handleMsg(msg);
			}
			if (mPriMpiCon->inQueueEmpty() && mPriMpiCon->inRequestsEmpty()){
				if (mPriReloadTest){
					mPriReloadStop = glfwGetTime();
					cerr << "Model reload complete?" << endl;
					cerr << "it took " << mPriReloadStop-mPriReloadStart << " seconds!" << endl;
					mPriReloadTest = false;
					mPriReloadStart = 0.0;
					mPriReloadStop = 0.0;
				}
			}
			//		cout << "waiting for matrix from 0..." << endl;
			//		receiveMethod(0);Offli
			GET_GLERROR(0);
			mPriGlFrame->display();
			mPriGlFrame->occlusionTest();
			//			cout << "sending the outqueue of renderer" << endl;
//			cout << "renderer sending outqueue" << endl;
			while(!mPriMpiCon->outQueueEmpty()){ // send everything
//				cout << "renderer found that his outqueue is not empty.....sending...." << endl;
				mPriMpiCon->send();
			}
		}

		// Swap front and back buffers (we use a double buffered display)
		mWindow->flip();
		mWindow->poll();

		mPriGlFrame->advTick();
		++frames;
		if (frames >= 100){
			frames = 0;
			stringstream ss;
			ss << "RenderNode (" << mPriMpiCon->getRank() << ") - FPS: " << mPriGlFrame->getFrames();
			mWindow->setTitle(ss.str());
		}
//		if (mPriMpiCon->getRank() == 1){
//			cerr << "OQ: " << mPriMpiCon->mOutQueue.size() << endl;
//			cerr << "IQ: " << mPriMpiCon->mInQueue.size() << endl;
//			cerr << "OR: " << mPriMpiCon->mPriOutRequests.size() << endl;
//			cerr << "IR: " << mPriMpiCon->mPriInRequests.size() << endl;
//		}

	} while (mRunning);
	cout << "RC constructor ended" << endl;
}

RenderCore::~RenderCore() {
	oocframework::EventManager::getSingleton()->removeListener(this, EndTransmissionEvent::classid());

	mRunning = false;
	delete mWindow;
	mWindow = 0;
	Logger::getSingleton()->closeAllHandles();
}


void RenderCore::sendMethod(int dest)
{
	MPI::Status status;
	while (!RenderCore::instance->mOutQueue.empty()) {
		int tag = 0;
		char* msg = RenderCore::instance->mOutQueue.front();
		MPI::COMM_WORLD.Isend(msg, strlen(msg) + 1, MPI::CHAR, dest, tag);
		delete[] msg;
		RenderCore::instance->mOutQueue.pop();
//		cout << "RenderCore send!!" << endl;
	}
}

void RenderCore::receiveMethod(int source)
{
	MpiControl::getSingleton()->receive(source);
	Message* msg = MpiControl::getSingleton()->pop();
	handleMsg(msg);
}

bool RenderCore::ireceiveMethod(int source)
{
	bool stat = MpiControl::getSingleton()->ireceive(source);
	Message* msg = MpiControl::getSingleton()->pop();
	handleMsg(msg);
	return stat;

}

void RenderCore::handleMsg(oocframework::Message* msg)
{
	if (msg != 0){
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()){
//			msg->setDst(msg->getSrc());
//			msg->setSrc(MpiControl::getSingleton()->getRank());
//			MpiControl::getSingleton()->push(msg);
//			MpiControl::getSingleton()->sendAll();
			mRunning = false;
			cout << "renderer recveived KILL from 0! " << msg->getSrc() << endl;
		}
		else if (msg->getType() == WindowResizedEvent::classid()->getShortId()){
			int w = ((int*)msg->getData())[0];
			int h = ((int*)msg->getData())[1];
			mWindow->resize(w,h);
		}
		else if (msg->getType() == KeyPressedEvent::classid()->getShortId()) {
			KeyPressedEvent kpe = KeyPressedEvent(msg);
			oocframework::EventManager::getSingleton()->fire(kpe);
		}
		else if (msg->getType() == ModelViewMatrixEvent::classid()->getShortId()){
			mPriGotMatrix = true;
//			cout << "rendercore got a matrix from 0" << endl;
			ModelViewMatrixEvent mve = ModelViewMatrixEvent(msg);
//			cout << "rendercore created an event for the matrix" << endl;
			oocframework::EventManager::getSingleton()->fire(mve);
		}
		else if (msg->getType() == VboEvent::classid()->getShortId()){
			VboEvent ve = VboEvent(msg);
			oocframework::EventManager::getSingleton()->fire(ve);
		}
		else if (msg->getType() == InfoRequestEvent::classid()->getShortId()){
			InfoRequestEvent ire = InfoRequestEvent();
			oocframework::EventManager::getSingleton()->fire(ire);
		}
		else if (msg->getType() == DepthBufferRequestEvent::classid()->getShortId()){
			DepthBufferRequestEvent dbre = DepthBufferRequestEvent(msg);
			ModelViewMatrixEvent mve = ModelViewMatrixEvent(dbre.getMatrix());
			oocframework::EventManager::getSingleton()->fire(mve);
			mPriMpiCon->barrier();
			AccumulatedRendertimeEvent arte = AccumulatedRendertimeEvent(mPriGlFrame->getRenderTime());
			mPriMpiCon->send(new Message(arte, 0));
			mPriGlFrame->resetRenderTime();
			mPriMpiCon->receive(0); // wait for new tiling-instructions
			handleMsg(MpiControl::getSingleton()->pop());
		}
		else if (msg->getType() == ChangeTileDimensionsEvent::classid()->getShortId()){
			ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent(msg);
			mPriGlFrame->setTileDimensions(ctde.getTileXPos(),ctde.getTileYPos(), ctde.getTileWidth(),ctde.getTileHeight());
//			cout << "render " << mPriMpiCon->getRank() << " waiting at barrier before deptbuffers" << endl;
			mPriMpiCon->barrier();
//			cout << "render " << mPriMpiCon->getRank() << " continuing" << endl;
//			mPriGlFrame->depthPass(true);
			mPriGlFrame->readAndSendDepth();
		}
		delete msg;
		msg = 0;
	}
}

void RenderCore::notify(IEvent& event)
{
	if (event.instanceOf(EndTransmissionEvent::classid())){
		EndTransmissionEvent& ete = (EndTransmissionEvent&)event;
		if (mPriReloadTest){
			mPriReloadStop = glfwGetTime();
			cerr << "Model reload complete?" << endl;
			cerr << "it took " << mPriReloadStop-mPriReloadStart << " seconds!" << endl;
			mPriReloadTest = false;
			mPriReloadStart = 0.0;
			mPriReloadStop = 0.0;
		}
	}
}
