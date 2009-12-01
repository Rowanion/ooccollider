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
#include "DepthBufferRequestEvent.h"
#include "JobDoneEvent.h"
#include "OcclusionRequestEvent.h"

#define RENDER_NODES 1

using namespace std;
using namespace oocframework;

DataCore* DataCore::instance = 0;

DataCore::DataCore(unsigned _winWidth, unsigned _winHeight, unsigned _targetWidth, unsigned _targetHeight) :
	mWindow(0), mRunning(true), mPriDepthBufferCount(0), mPriQuintMapList(std::map<int, std::list<const ooctools::Quintuple*> >()),
	mPriMTwister(PRESELECTED_SEED), mPriFrameTick(0), mPriRequestTime(0.0)
{
	DataCore::instance = this;
	mPriMpiCon = oocframework::MpiControl::getSingleton();

	//		setupWindow("My rank is NOT 0");
	stringstream title;
	title << "DataNode (" << mPriMpiCon->getRank() << ")";
	mWindow = new OOCWindow(_winWidth, _winHeight, 8, false, title.str().c_str());
//	mWindow->enableKeyCallback();
	GET_GLERROR(0);
	mGlFrame = new DataCoreGlFrame(_winWidth, _winHeight, _targetWidth, _targetHeight);
	GET_GLERROR(0);
	mWindow->attachGlFrame(mGlFrame);
	GET_GLERROR(0);
	mGlFrame->init();
	GET_GLERROR(0);
	const std::vector<int> dataGrp = mPriMpiCon->getDataGroup();
	for (unsigned i=0; i< dataGrp.size(); ++i){
		mPriQuintMapList.insert(make_pair(dataGrp[i], std::list<const ooctools::Quintuple*>()));
	}
	// Main rendering loop

	mPriMpiCon->barrier();
	do {
//		cout << "2 waiting for any..." << endl;
		mPriMpiCon->ireceive(MPI::ANY_SOURCE);
		if (!mPriMpiCon->inQueueEmpty()){
			handleMsg(mPriMpiCon->pop());
		}
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

void DataCore::handleMsg(Message* msg){
	if (msg != 0){
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()){
			//				msg->setDst(source);
			//				MpiControl::getSingleton()->push(msg);
			//				MpiControl::getSingleton()->sendAll();
			mRunning = false;
			cout << "data recveived KILL from " << msg->getSrc() << endl;
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
//							cout << "yeah depthbuffer dot com kam an! " << msg->getSrc() << " -> " << MpiControl::getSingleton()->getRank() << endl;
			DepthBufferEvent dbe = DepthBufferEvent(msg);
			oocframework::EventManager::getSingleton()->fire(dbe);
			mPriDepthBufferCount++;
//			cout << "datacore has now " << mPriDepthBufferCount << " of " << MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER)<< endl;
		}
		else if (msg->getType() == NodeRequestEvent::classid()->getShortId()){

#ifdef DEBUG_DATAREQUEST
			double newTime = glfwGetTime();
#endif
			NodeRequestEvent nre = NodeRequestEvent(msg);
			mGlFrame->setMvMatrix(nre.getMatrix());

			// fetch/reload VBOs and send back
//			cout << "starting " << nre.getIdxCount() << " jobs" << endl;

			// sort Quadruples into map based on their destination id
			for (unsigned i=0; i< nre.getIdxCount(); ++i){
				Quintuple* quint = nre.getQuintuple(i);
				mPriQuintMapList[quint->destId].push_back(quint);
			}
			// call display for each entry in map and clear list afterwards
			map<int, list<const Quintuple*> >::iterator mapListIt = mPriQuintMapList.begin();
			for (; mapListIt != mPriQuintMapList.end(); ++mapListIt){
				if (!mapListIt->second.empty()){
					mGlFrame->display(mapListIt->first, &(mapListIt->second));
				}
				mapListIt->second.clear();
			}
//			cout << "finished " << nre.getIdxCount() << " jobs" << endl;
//			JobDoneEvent jde = JobDoneEvent(nre.getIdxCount());
//			mPriMpiCon->isend(new Message(jde, 0));
			// clear the lists in the map;
			// for each non-empty map -> call display with ref toi this map
			GET_GLERROR(0);
//			mGlFrame->display(nre);

#ifdef DEBUG_DATAREQUEST
			double newerTime = glfwGetTime();
			mPriRequestTime += (newerTime-newTime);
			if (mPriFrameTick % OCULLING_AVG == 0){
				cout << "(" << MpiControl::getSingleton()->getRank() << ") DataRequest took " << mPriRequestTime/OCULLING_AVG << " secs avg. over " << DISPLAY_AVG << " frames." << endl;
				mPriRequestTime = 0.0;
			}
#endif
			++mPriFrameTick;
			mPriFrameTick %= MODULO_FRAMECOUNT;
		}
		else if (msg->getType() == OcclusionRequestEvent::classid()->getShortId()){
			cerr << "" << endl;
			OcclusionRequestEvent ore = OcclusionRequestEvent(msg);
			mGlFrame->setMvMatrix(ore.getMatrix());

			// fetch/reload VBOs and send back
//			cout << "starting " << nre.getIdxCount() << " jobs" << endl;

			// sort Quadruples into map based on their destination id
			for (unsigned i=0; i< ore.getIdxCount(); ++i){
				Quintuple* quint = ore.getQuintuple(i);
				mPriQuintMapList[quint->destId].push_back(quint);
			}
			// call display for each entry in map and clear list afterwards
			map<int, list<const Quintuple*> >::iterator mapListIt = mPriQuintMapList.begin();
			for (; mapListIt != mPriQuintMapList.end(); ++mapListIt){
				if (!mapListIt->second.empty()){
					mGlFrame->occlusionTest(mapListIt->first, &(mapListIt->second));
				}
				mapListIt->second.clear();
			}
//			cout << "finished " << nre.getIdxCount() << " jobs" << endl;
//			JobDoneEvent jde = JobDoneEvent(nre.getIdxCount());
//			mPriMpiCon->isend(new Message(jde, 0));
			// clear the lists in the map;
			// for each non-empty map -> call display with ref toi this map
			GET_GLERROR(0);
//			mGlFrame->display(nre);

			++mPriFrameTick;
			mPriFrameTick %= MODULO_FRAMECOUNT;
		}
		else if (msg->getType() == DepthBufferRequestEvent::classid()->getShortId()){
			mPriMpiCon->barrier();
			mPriMpiCon->clearOutQueue(MpiControl::RENDERER);

			mPriMpiCon->completeWaitingReceives(DepthBufferEvent::classid());
			Message* newMsg = 0;
			while(!mPriMpiCon->inQueueEmpty()){
				newMsg = mPriMpiCon->pop();
				if (newMsg->getType() == DepthBufferEvent::classid()->getShortId()){
					handleMsg(newMsg);
				}
				else if (newMsg->getSrc() == 0){
					handleMsg(newMsg);
				}
				else {
					delete newMsg;
					newMsg = 0;
				}
			}
//			cout << "data waiting at barrier before depthbuffers" << endl;
			mPriMpiCon->barrier();
//			cout << "data continuing" << endl;

			while(mPriDepthBufferCount < mPriMpiCon->getGroupSize(MpiControl::RENDERER)){
				mPriMpiCon->ireceive(MpiControl::RENDERER);
				while(!mPriMpiCon->inQueueEmpty()){
					handleMsg(mPriMpiCon->pop());
				}
			}

			mPriDepthBufferCount = 0;
//			cout << "datacore has received all depthbuffers" << endl;
		}
		else if (msg->getType() == InfoRequestEvent::classid()->getShortId()){
			InfoRequestEvent ire = InfoRequestEvent();
			oocframework::EventManager::getSingleton()->fire(ire);
		}
		delete msg;
		msg = 0;
	}
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
