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

namespace fs = boost::filesystem;
using namespace ooctools;
using namespace oocformats;
using namespace std;

RenderMasterCore* RenderMasterCore::instance = 0;

RenderMasterCore::RenderMasterCore(unsigned _width, unsigned _height) : mWindow(0), mRunning(true), mTerminateApplication(false), mPriOh(OctreeHandler()), mPriLo(0), mPriWindowWidth(_width), mPriWindowHeight(_height)
{

	RenderMasterCore::instance = this;
	mWindow = new OOCWindow(_width, _height, 8, false, "MASTER_NODE");
	mWindow->enableKeyCallback();
	mWindow->enableMouseCallback();
	SimpleGlFrame* glFrame = new SimpleGlFrame();
//	RenderMasterCoreGlFrame* glFrame = new RenderMasterCoreGlFrame();
	mWindow->attachGlFrame(glFrame);
	mPriEventMan = oocframework::EventManager::getSingleton();
	mPriEventMan->addListener(this, WindowResizedEvent::classid());
	mPriEventMan->addListener(this, KillApplicationEvent::classid());
	mPriEventMan->addListener(this, MouseButtonEvent::classid());
	mPriEventMan->addListener(this, MouseDraggedEvent::classid());
	mPriEventMan->addListener(this, KeyPressedEvent::classid());
	mPriEventMan->addListener(this, WindowClosedEvent::classid());
	mPriEventMan->addListener(this, InfoRequestEvent::classid());

	glFrame->init();

	if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER) == 2){
		Tile t;
		t.height=mPriWindowHeight/2;
		t.width=mPriWindowWidth;
		t.xPos = 0;
		t.yPos = 0;
		t.renderTime = 0.0;
		mPriTileMap.insert(make_pair(MpiControl::getSingleton()->getRenderGroup()[0], t));
		t.height=mPriWindowHeight;
		t.width=mPriWindowWidth/2;
		t.xPos = mPriWindowWidth/2;
		t.yPos = 0;
		t.renderTime = 0.0;
		mPriTileMap.insert(make_pair(MpiControl::getSingleton()->getRenderGroup()[1], t));

	}
	else if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER) == 4){
		//TODO implement
	}

//	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path("/media/ClemensHDD/Octree/skeleton.bin"));
//	glFrame->setVbo(new IndexedVbo(fs::path("/media/ClemensHDD/B3_SampleTree/data/0/1.idx")));



	// Main rendering loop
	unsigned frames = 0;
	do {
		while(!MpiControl::getSingleton()->outQueueEmpty()){
//			cout << "master found that his outqueue is not empty.....sending...." << endl;
			MpiControl::getSingleton()->send();
		}

		if (!mTerminateApplication){
			//send matrix/camera to when the out-queue is empty
//			cout << "---master sending matrix" << endl;
			for (int i=1; i<MpiControl::getSingleton()->getSize(); ++i){
				Message* msg = new Message(ModelViewMatrixEvent::classid()->getShortId(),16*sizeof(float),i,(char*)glFrame->getMvMatrix());
				MpiControl::getSingleton()->send(msg);
//				cout << "master sent matrix to " << i << endl;
			}
//			cout << "master has sent all matrices" << endl;

			//rcv kacheln from all renderers
//			cout << "---master wating for tiles" << endl;
			MpiControl::getSingleton()->receive(MpiControl::RENDERER);
			while (!MpiControl::getSingleton()->inQueueEmpty()){
				handleMsg(MpiControl::getSingleton()->pop());
			}

//			cout << "---master ENTER display" << endl;
			glFrame->display();
//			cout << "---master EXIT display" << endl;
//			cout << "end of display 0..." << endl;
		}
		else{
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
		++frames;
		if (frames >= 100){
			frames = 0;
			stringstream ss;
			ss << "MasterNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << glFrame->getFrames();
			mWindow->setTitle(ss.str());
		}

	} while (mRunning);
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

	mRunning = false;
	delete mWindow;
	mWindow = 0;
}

void
RenderMasterCore::Event(int event, bool state)
{
	switch (event){
	case EVENT_WINDOW_CLOSED:
		mRunning = false;
		break;
	case GLFW_KEY_F1:
		((RenderMasterCoreGlFrame*)mWindow->getGlFrame())->debug();
		break;
	case EVENT_WINDOW_RESIZED:{
//		int count = 2*sizeof(int);
//		char* data = new char[count];
//		((int*)data)[0] = mWindow->getWidth();
//		((int*)data)[1] = mWindow->getHeight();
//		Message* msg = new Message(WindowResizedEvent::classid()->getShortId(),count,1,data);
//		MpiControl::getSingleton()->push(msg);
//		MpiControl::getSingleton()->sendAll();
		break;}
	default:
//		cout << "Event fired: " << event << " with state: " << state << endl;
		break;
	}
}

MPI::Request RenderMasterCore::sendQueue(int dest)
{
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

void RenderMasterCore::handleMsg(Message* msg)
{
	if (msg != 0){
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()){
			mRunning = false;
			cout << "recveived kill! from " << msg->getSrc() << endl;
		}
		else if (msg->getType() == ColorBufferEvent::classid()->getShortId()){
//			cout << "recveived colorbuffer! " << msg->getSrc() << endl;

			ColorBufferEvent cbe = ColorBufferEvent(msg);
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
		delete msg;
		msg = 0;
	}
}

void RenderMasterCore::adjustTileDimensions()
{
	if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER)==2){
		double	totalTime =
			mPriTileMap[MpiControl::getSingleton()->getRenderGroup()[0]].renderTime
			+ mPriTileMap[MpiControl::getSingleton()->getRenderGroup()[1]].renderTime;
		double halfTime = totalTime/2.0;
		map<int, Tile>::iterator maxTimeIt = mPriTileMap.begin();
		map<int, Tile>::iterator it = mPriTileMap.begin();
		for (; it != mPriTileMap.end(); ++it){
			if (it->second.renderTime > maxTimeIt->second.renderTime)
				maxTimeIt = it;
		}
		double timeDiff = maxTimeIt->second.renderTime - halfTime;
		double timeDiffPerc = 100.0/maxTimeIt->second.renderTime*timeDiff;
		int windowDiff = (int)(maxTimeIt->second.width/100.0*timeDiffPerc);
		maxTimeIt->second.width -= windowDiff;

	}
	else if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER)==4){
		//TODO implement
	}
}

void RenderMasterCore::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(WindowResizedEvent::classid())){
		WindowResizedEvent& rwe = (WindowResizedEvent&)event;

	}
	else if(event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& kpe = (KeyPressedEvent&)event;
		switch (kpe.getKey()){
		case GLFW_KEY_ESC:
			char data;
			MpiControl::getSingleton()->push(new Message(KillApplicationEvent::classid()->getShortId(), 1, 0, &data, MpiControl::DATA));
			MpiControl::getSingleton()->push(new Message(KillApplicationEvent::classid()->getShortId(), 1, 0, &data, MpiControl::RENDERER));
			mTerminateApplication = true;
			break;
		case 'N':
			MpiControl::getSingleton()->push(new Message(kpe,1, MpiControl::ALL));
			break;
		case 'F':
			MpiControl::getSingleton()->push(new Message(kpe,1, MpiControl::ALL));
			break;
		case 'I':{
			cout << "====================================" << endl;
			cout << "Requesting Information from Nodes..." << endl;
			cout << "====================================" << endl;
			InfoRequestEvent ire = InfoRequestEvent();
			MpiControl::getSingleton()->push(new Message(ire,1));
			MpiControl::getSingleton()->push(new Message(ire,2));
			oocframework::EventManager::getSingleton()->fire(ire);
			break;}
		default:
			MpiControl::getSingleton()->push(new Message(kpe,1, MpiControl::RENDERER));
			break;
		}
	}
	else if(event.instanceOf(WindowClosedEvent::classid())){
		char data;
		MpiControl::getSingleton()->push(new Message(KillApplicationEvent::classid()->getShortId(), 1, 0, &data, MpiControl::DATA));
		MpiControl::getSingleton()->push(new Message(KillApplicationEvent::classid()->getShortId(), 1, 0, &data, MpiControl::RENDERER));
		mTerminateApplication = true;
	}
}
