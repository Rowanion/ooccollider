/**
 * @file	DataCore.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	DataCore class definition.
 */

#include "DataCore.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "mpi.h"

#include <iostream>
#include <string>
#include <sstream>

#include "OOCWindow.h"
#include "DataCoreGlFrame.h"
#include "MpiControl.h"
#include "Message.h"
#include "KillApplicationEvent.h"
#include "WindowResizedEvent.h"
#include "ModelViewMatrixEvent.h"
#include "KeyPressedEvent.h"
#include "DepthBufferEvent.h"
#include "VboEvent.h"
#include "InfoRequestEvent.h"

#define RENDER_NODES 1

using namespace std;

DataCore* DataCore::instance = 0;

DataCore::DataCore(unsigned _width, unsigned _height) : mWindow(0), mRunning(true), mPriDepthBufferCount(0)
{
	DataCore::instance = this;

	//		setupWindow("My rank is NOT 0");
	stringstream title;
	title << "DataNode (" << MpiControl::getSingleton()->getRank() << ")";
	mWindow = new OOCWindow(_width, _height, 8, false, title.str().c_str());
//	mWindow->enableKeyCallback();
	mGlFrame = new DataCoreGlFrame();
	mWindow->attachGlFrame(mGlFrame);
	mGlFrame->init();

	// Main rendering loop
	do {
//		cout << "2 waiting for any..." << endl;
		receiveMethod(MPI::ANY_SOURCE);
		MpiControl::getSingleton()->isend();


//		renderOneFrame();
//		MpiControl::getSingleton()->send();

//		if (!mInQueue.empty()){
//			mRunning = false;
//		}
	} while (mRunning);
	cout << "DC constructor ended" << endl;
}

DataCore::~DataCore() {
	// TODO Auto-generated destructor stub
	mRunning = false;
	delete mGlFrame;
	mGlFrame = 0;
	delete mWindow;
	mWindow = 0;
}

void DataCore::sendMethod(int dest)
{
	MPI::Status status;
	while (!DataCore::instance->mOutQueue.empty()) {
		int tag = 0;
		char* msg = DataCore::instance->mOutQueue.front();
		MPI::COMM_WORLD.Isend(msg, strlen(msg) + 1, MPI::CHAR, dest, tag);
		delete[] msg;
		DataCore::instance->mOutQueue.pop();
//		cout << "DataCore send!!" << endl;
	}
}

void DataCore::renderOneFrame()
{
	mGlFrame->display();
	mWindow->flip();
	mWindow->poll();

}

void DataCore::receiveMethod(int source)
{
	if (MpiControl::getSingleton()->ireceive(source)){
		Message* msg = MpiControl::getSingleton()->pop();
		if (msg != 0){
			if (msg->getType() == KillApplicationEvent::classid()->getShortId()){
//				msg->setDst(source);
//				MpiControl::getSingleton()->push(msg);
//				MpiControl::getSingleton()->sendAll();
				mRunning = false;
				cout << "recveived KILL from " << source << endl;
			}
			else if (msg->getType() == WindowResizedEvent::classid()->getShortId()){
				int w = ((int*)msg->getData())[0];
				int h = ((int*)msg->getData())[1];
				mWindow->resize(w,h);
			}
			//		else if (msg->getType() == MouseButtonEvent::classid()->getShortId()){
			//			int x = ((int*)msg->getData())[0];
			//			int y = ((int*)msg->getData())[1];
			//			int state = ((int*)msg->getData())[2];
			//			MouseButtonEvent mbe = MouseButtonEvent(x,y,state);
			//			oocframework::EventManager::getSingleton()->fire(mbe);
			//		}
			//		else if (msg->getType() == MouseDraggedEvent::classid()->getShortId()){
			//			int x = ((int*)msg->getData())[0];
			//			int y = ((int*)msg->getData())[1];
			//			MouseDraggedEvent mde = MouseDraggedEvent(x,y);
			//			oocframework::EventManager::getSingleton()->fire(mde);
			//		}
			else if (msg->getType() == KeyPressedEvent::classid()->getShortId()) {
				int key = ((int*) msg->getData())[0];
				KeyPressedEvent kpe = KeyPressedEvent(key);
				oocframework::EventManager::getSingleton()->fire(kpe);
			}
			else if (msg->getType() == ModelViewMatrixEvent::classid()->getShortId()){
				ModelViewMatrixEvent mve = ModelViewMatrixEvent(((float*)msg->getData()));
//				cout << "datacore got a matrix!" << endl;
//				mGlFrame->setMvMatrix(mve.getMatrix());
//				cout << "datacore matrix! successfully copied!" << endl;
				oocframework::EventManager::getSingleton()->fire(mve);
			}
			else if (msg->getType() == DepthBufferEvent::classid()->getShortId()){
//				cout << "yeah depthbuffer dot com kam hier an" << endl;
				DepthBufferEvent dbe = DepthBufferEvent(msg);
				oocframework::EventManager::getSingleton()->fire(dbe);
				MpiControl::getSingleton()->clearOutQueue(MpiControl::RENDERER);
				MpiControl::getSingleton()->clearInQueue(MpiControl::RENDERER);
			}
			else if (msg->getType() == NodeRequestEvent::classid()->getShortId()){
				NodeRequestEvent nre = NodeRequestEvent(msg);
				// fetch/reload VBOs and send back
//				cout << "list of node-requests reached DataCore: " << endl;
//				for(unsigned i=0; i< nre.getIdxCount(); ++i){
//					cout << nre.getId(i) << endl;
//				}
				mGlFrame->display(nre);

			}
			else if (msg->getType() == InfoRequestEvent::classid()->getShortId()){
				InfoRequestEvent ire = InfoRequestEvent();
				oocframework::EventManager::getSingleton()->fire(ire);
			}
			delete msg;
		}
	}
	else return;
}

void DataCore::waitForDepthBuffers()
{
	while(mPriDepthBufferCount < RENDER_NODES){
		if(MpiControl::getSingleton()->ireceive(MPI::ANY_SOURCE)){
			Message* msg = MpiControl::getSingleton()->pop();
			if (msg!= 0 && msg->getType() == DepthBufferEvent::classid()->getShortId()){
				mPriDepthBufferCount++;
				DepthBufferEvent dbe = DepthBufferEvent(msg);
				oocframework::EventManager::getSingleton()->fire(dbe);
			}
			delete msg;
		}

	}
	mPriDepthBufferCount = 0;
}
