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


using namespace std;

RenderCore* RenderCore::instance = 0;

RenderCore::RenderCore(unsigned _width, unsigned _height) : mWindow(0), mRunning(true), mPriGotMatrix(false)
{
	RenderCore::instance = this;

	//		setupWindow("My rank is NOT 0");
	stringstream title;
	title << "RenderNode (" << MpiControl::getSingleton()->getRank() << ")";
	mWindow = new OOCWindow(_width, _height, 8, false, title.str().c_str());
//	mWindow->enableKeyCallback();
	RenderCoreGlFrame* glFrame = new RenderCoreGlFrame();
	mWindow->attachGlFrame(glFrame);
	glFrame->init();

	// Main rendering loop
	unsigned frames = 0;
	do {
		mPriGotMatrix = false;
		while(!mPriGotMatrix && mRunning){
			MpiControl::getSingleton()->receive(0);
			handleMsg(MpiControl::getSingleton()->pop());
//			cout << "matrix arrived at renderer" << endl;
		}
		MpiControl::getSingleton()->ireceive(MpiControl::DATA);
		while(!MpiControl::getSingleton()->inQueueEmpty()){
			handleMsg(MpiControl::getSingleton()->pop());
		}
//		cout << "waiting for matrix from 0..." << endl;
//		receiveMethod(0);
		if (mRunning){
			glFrame->display();
//			cout << "sending the outqueue of renderer" << endl;
			while(!MpiControl::getSingleton()->outQueueEmpty()){
//				cout << "renderer found that his outqueue is not empty.....sending...." << endl;
				MpiControl::getSingleton()->send();
			}
		}

		// Swap front and back buffers (we use a double buffered display)
		mWindow->flip();
		mWindow->poll();

		++frames;
		if (frames >= 100){
			frames = 0;
			stringstream ss;
			ss << "RenderNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << glFrame->getFrames();
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
//			cout << "rendercore got a matrix from 0" << endl;
			ModelViewMatrixEvent mve = ModelViewMatrixEvent(((float*)msg->getData()));
//			cout << "rendercore created an event for the matrix" << endl;
			oocframework::EventManager::getSingleton()->fire(mve);
		}
		else if (msg->getType() == VboEvent::classid()->getShortId()){
//			cout << "rendercore got a vbo from a data node" << endl;
			VboEvent ve = VboEvent(msg);
			oocframework::EventManager::getSingleton()->fire(ve);
		}
		else if (msg->getType() == InfoRequestEvent::classid()->getShortId()){
			InfoRequestEvent ire = InfoRequestEvent();
			oocframework::EventManager::getSingleton()->fire(ire);
		}
		delete msg;
	}
}
