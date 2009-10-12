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
#include "EndTransmissionEvent.h"
#include "EndOfFrameEvent.h"


using namespace std;

RenderCore* RenderCore::instance = 0;

RenderCore::RenderCore(unsigned _width, unsigned _height, unsigned _finalWidth, unsigned _finalHeight) : mWindow(0), mRunning(true), mPriGotMatrix(false)
{

	RenderCore::instance = this;
	mPriMpiCon = MpiControl::getSingleton();

	//		setupWindow("My rank is NOT 0");
	stringstream title;
	title << "RenderNode (" << MpiControl::getSingleton()->getRank() << ")";
	mWindow = new OOCWindow(_width, _height, 8, false, title.str().c_str());
//	mWindow->enableKeyCallback();
	mPriGlFrame = new RenderCoreGlFrame(_width, _height, _finalWidth, _finalHeight);
	mWindow->attachGlFrame(mPriGlFrame);
	GET_GLERROR(0);

	// get the initial tile-dimensions
	MpiControl::getSingleton()->receive(0);
	while(!MpiControl::getSingleton()->inQueueEmpty()){ //
		Message* msg = MpiControl::getSingleton()->pop();
		if (msg->getType() == ChangeTileDimensionsEvent::classid()->getShortId()){
			ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent(msg);
			mPriGlFrame->setTileDimensions(ctde.getTileXPos(), ctde.getTileYPos(), ctde.getTileWidth(), ctde.getTileWidth());
		}
		delete msg;
		msg = 0;
	}

	mPriGlFrame->init();
	GET_GLERROR(0);
	mPriGlFrame->reshape(_width, _height);
	GET_GLERROR(0);

	// Main rendering loop
	unsigned frames = 0;
	mPriMpiCon->barrier();
	do {
//		mPriGotMatrix = false;
//		while(mRunning){ // receive the matrix from 0
		cout << "0 <-- renderer checking for matrix" << endl;
		while(!mPriMpiCon->probe(0, ModelViewMatrixEvent::classid())){
		}
		mPriMpiCon->ireceive(0, ModelViewMatrixEvent::classid());
		cout << "[0] renderer got matrix" << endl;

		cout << "1 --> renderer sending tile" << endl;
		mPriMpiCon->isend(new Message(mPriGlFrame->getColorBufferEvent(), 0));
		cout << "[1] renderer sent tile" << endl;
		mPriMpiCon->iCheck();
		while(!mPriMpiCon->inQueueEmpty()){
			handleMsg(MpiControl::getSingleton()->pop());
		}

		cout << "2 --> renderer performing culling" << endl;
		mPriGlFrame->cullFrustum();
		cout << "[2] renderer done with culling" << endl;

		mPriMasterDone = false;
		cout << "3 <-- renderer waiting for any (data and master)" << endl;
		while(!mPriMasterDone){
			mPriMpiCon->ireceive(MPI_ANY_SOURCE);
			if (!mPriMpiCon->inQueueEmpty()){
				handleMsg(mPriMpiCon->pop());
			}
		}
		cout << "[3] renderer got all from any (data and master)" << endl;

//		cout << "matrix arrived at renderer" << endl;
//		MpiControl::getSingleton()->ireceive(MpiControl::DATA);
//		while(!MpiControl::getSingleton()->inQueueEmpty()){ // ireceive everything from data-nodes
//			Message* msg = MpiControl::getSingleton()->pop();
//			handleMsg(msg);
//		}
//		cout << "waiting for matrix from 0..." << endl;
//		receiveMethod(0);
		if (mRunning){
			GET_GLERROR(0);
			mPriGlFrame->display();
//			cout << "sending the outqueue of renderer" << endl;
//			cout << "renderer sending non-empty outqueue" << endl;
//			while(!MpiControl::getSingleton()->outQueueEmpty()){ // send everything
////				cout << "renderer found that his outqueue is not empty.....sending...." << endl;
//				MpiControl::getSingleton()->send();
//			}
			cout << "renderer REPEAT" << endl;
		}

//		cout << "4 <-- renderer waiting for frameend" << endl;
//		mPriMpiCon->receive(0, EndOfFrameEvent::classid());
//		while (!mPriMpiCon->inQueueEmpty()){
//			handleMsg(mPriMpiCon->pop());
//		}
//		cout << "[4] renderer got frameend" << endl;
		// Swap front and back buffers (we use a double buffered display)
		mWindow->flip();
		mWindow->poll();

		++frames;
		if (frames >= 100){
			frames = 0;
			stringstream ss;
			ss << "RenderNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << mPriGlFrame->getFrames();
			mWindow->setTitle(ss.str());
		}
	} while (mRunning);
	cout << "RC constructor ended" << endl;
}

RenderCore::~RenderCore() {
	// TODO Auto-generated destructor stub
	mRunning = false;
	delete mWindow;
	mWindow = 0;
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

void RenderCore::handleMsg(Message* msg)
{
	if (msg != 0){
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()){
//			msg->setDst(msg->getSrc());
//			msg->setSrc(MpiControl::getSingleton()->getRank());
//			MpiControl::getSingleton()->push(msg);
//			MpiControl::getSingleton()->sendAll();
			mRunning = false;
			cout << "recveived KILL from 0! " << msg->getSrc() << endl;
		}
		else if (msg->getType() == WindowResizedEvent::classid()->getShortId()){
			int w = ((int*)msg->getData())[0];
			int h = ((int*)msg->getData())[1];
			mWindow->resize(w,h);
		}
		else if (msg->getType() == KeyPressedEvent::classid()->getShortId()) {
			int key = ((int*) msg->getData())[0];
			KeyPressedEvent kpe = KeyPressedEvent(key);
			oocframework::EventManager::getSingleton()->fire(kpe);
		}
		else if (msg->getType() == ModelViewMatrixEvent::classid()->getShortId()){
			mPriGotMatrix = true;
			cout << "rendercore got a matrix from 0" << endl;
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
			cout << "render " << mPriMpiCon->getRank() << " waiting at barrier before deptbuffers" << endl;
			mPriMpiCon->barrier();
			cout << "render " << mPriMpiCon->getRank() << " continuing" << endl;
			mPriGlFrame->depthPass();
		}
		else if (msg->getType() == EndTransmissionEvent::classid()->getShortId()){
			cout << "MASTER DONE EVENT " << EndTransmissionEvent::classid()->getShortId() << endl;
			mPriMasterDone = true;
		}
		delete msg;
		msg = 0;
	}
}
