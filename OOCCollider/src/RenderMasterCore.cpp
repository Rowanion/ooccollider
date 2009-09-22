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

namespace fs = boost::filesystem;
using namespace ooctools;
using namespace oocformats;
using namespace std;

RenderMasterCore* RenderMasterCore::instance = 0;

RenderMasterCore::RenderMasterCore(unsigned _width, unsigned _height) :
	mWindow(0), mRunning(true), mTerminateApplication(false),
			mPriCamHasMoved(false), mPriFrameCount(0), mPriRenderTimeCount(0), mPriOh(OctreeHandler()),
			mPriLo(0), mPriWindowWidth(_width), mPriWindowHeight(_height) {

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

	if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER) == 2) {
		Tile t;
		t.height = mPriWindowHeight;
		t.width = mPriWindowWidth / 2;
		t.xPos = 0;
		t.yPos = 0;
		t.renderTime = 0.0;
		mPriTileMap.insert(make_pair(
				MpiControl::getSingleton()->getRenderGroup()[0], t));
		t.height = mPriWindowHeight;
		t.width = mPriWindowWidth / 2;
		t.xPos = mPriWindowWidth / 2;
		t.yPos = 0;
		t.renderTime = 0.0;
		mPriTileMap.insert(make_pair(
				MpiControl::getSingleton()->getRenderGroup()[1], t));

	} else if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER)
			== 4) {
		//TODO implement
	}

	//	mPriLo = mPriOh.loadLooseOctreeSkeleton(fs::path("/media/ClemensHDD/Octree/skeleton.bin"));
	//	glFrame->setVbo(new IndexedVbo(fs::path("/media/ClemensHDD/B3_SampleTree/data/0/1.idx")));


	// Main rendering loop
	unsigned frames = 0;
	do {
		while (!MpiControl::getSingleton()->outQueueEmpty()) {
			//			cout << "master found that his outqueue is not empty.....sending...." << endl;
			MpiControl::getSingleton()->send();
		}

		if (!mTerminateApplication) {
			if (mPriFrameCount >= (DEPTHBUFFER_INTERVAL - 1) && mPriCamHasMoved == true){
				mPriCamHasMoved = false;
				mPriFrameCount = 0;
				DepthBufferRequestEvent dbre = DepthBufferRequestEvent();
				MpiControl::getSingleton()->send(new Message(dbre, 0, MpiControl::RENDERER));
				// go into listenmode to receive the rendering-times
				MpiControl::getSingleton()->receive(MpiControl::RENDERER);
				while (!MpiControl::getSingleton()->inQueueEmpty()) {
					handleMsg(MpiControl::getSingleton()->pop());
				}
			}

			//send matrix/camera to when the out-queue is empty
			//			cout << "---master sending matrix" << endl;
			//			for (int i=1; i<MpiControl::getSingleton()->getSize(); ++i){
			for (unsigned i = 0; i < MpiControl::getSingleton()->getGroupSize(
					MpiControl::DATA); ++i) { // send the matrix to all data-nodes
				Message* msg = new Message(ModelViewMatrixEvent::classid()->getShortId(),16*sizeof(float),MpiControl::getSingleton()->getDataGroup()[i],(char*)glFrame->getMvMatrix());
				MpiControl::getSingleton()->send(msg);
			}
			ModelViewMatrixEvent mve = ModelViewMatrixEvent(glFrame->getMvMatrix());
			for (unsigned i=0; i<MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER); ++i) { // send matrix to all renderers
				Tile t = mPriTileMap[MpiControl::getSingleton()->getRenderGroup()[i]];
				mve.setTileDimension(t);
				ModelViewMatrixEvent mve = ModelViewMatrixEvent(glFrame->getMvMatrix(), t);
				Message* msg = new Message(mve, MpiControl::getSingleton()->getRenderGroup()[i]);
				MpiControl::getSingleton()->send(msg);
			}
			//			cout << "master has sent all matrices" << endl;

			//rcv kacheln from all renderers
			//			cout << "---master wating for tiles" << endl;
			MpiControl::getSingleton()->receive(MpiControl::RENDERER); // receive a tile-image from renderer
			while (!MpiControl::getSingleton()->inQueueEmpty()) {
				handleMsg(MpiControl::getSingleton()->pop());
			}
//			adjustTileDimensions();

			//			cout << "---master ENTER display" << endl;
			glFrame->display();
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
		++frames;
		if (frames >= 100) {
			frames = 0;
			stringstream ss;
			ss << "MasterNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << glFrame->getFrames();
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

	mRunning = false;
	delete mWindow;
	mWindow = 0;
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

void RenderMasterCore::handleMsg(Message* msg) {
	if (msg != 0) {
		if (msg->getType() == KillApplicationEvent::classid()->getShortId()) {
			mRunning = false;
			cout << "recveived kill! from " << msg->getSrc() << endl;
		} else if (msg->getType() == ColorBufferEvent::classid()->getShortId()) {
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
			mPriTileMap[msg->getSrc()].renderTime = ((double*)msg->getData())[0];
			mPriRenderTimeCount++;
			if (mPriRenderTimeCount>= MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER)){
				mPriRenderTimeCount = 0;
				adjustTileDimensions();
				ChangeTileDimensionsEvent ctde = ChangeTileDimensionsEvent();
				map<int, Tile>::iterator it = mPriTileMap.begin();
				for(; it != mPriTileMap.end(); ++it){
					ctde.setTileDimension(it->second);
					MpiControl::getSingleton()->send(new Message(ctde, it->first));
				}
			}
			// store render-time, inc counter
			// check for counter and if = max recalc tile-dims
		}

		delete msg;
		msg = 0;
	}
}

void RenderMasterCore::adjustTileDimensions() {
	if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER) == 2) {
		int renderer0 = MpiControl::getSingleton()->getRenderGroup()[0];
		int renderer1 = MpiControl::getSingleton()->getRenderGroup()[1];
		double totalTime = mPriTileMap[renderer0].renderTime
				+ mPriTileMap[renderer1].renderTime;
		double halfTime = totalTime / 2.0;
		if (mPriTileMap[renderer0].renderTime
				> mPriTileMap[renderer1].renderTime) { // renderer0 is slowest
			double timeDiff = mPriTileMap[renderer0].renderTime - halfTime; // difference from half-time
			double timeDiffPerc = (100.0 / mPriTileMap[renderer0].renderTime)
					* timeDiff; // percent of difference from half-time
			int windowDiff = (int) ((mPriTileMap[renderer0].width / 100.0)
					* timeDiffPerc); // difference in width
			mPriTileMap[renderer0].width -= windowDiff;
			mPriTileMap[renderer1].xPos -= windowDiff;
			mPriTileMap[renderer1].width += windowDiff;

			mPriTileMap[renderer0].width = max(mPriTileMap[renderer0].width, 1);
			mPriTileMap[renderer1].xPos = max(mPriTileMap[renderer1].xPos, 1);
			mPriTileMap[renderer1].width = min(mPriTileMap[renderer1].width,
					mPriWindowWidth - 1);
		} else if (mPriTileMap[renderer1].renderTime
				> mPriTileMap[renderer0].renderTime) { // renderer1 is slowest
			double timeDiff = mPriTileMap[renderer1].renderTime - halfTime; // difference from half-time
			double timeDiffPerc = (100.0 / mPriTileMap[renderer1].renderTime)
					* timeDiff; // percent of difference from half-time
			int windowDiff = (int) ((mPriTileMap[renderer1].width / 100.0)
					* timeDiffPerc); // difference in width
			mPriTileMap[renderer1].width -= windowDiff;
			mPriTileMap[renderer1].xPos += windowDiff;
			mPriTileMap[renderer0].width += windowDiff;

			mPriTileMap[renderer1].width = max(mPriTileMap[renderer1].width, 1);
			mPriTileMap[renderer1].xPos = min(mPriTileMap[renderer1].xPos,
					mPriWindowWidth - 1);
			mPriTileMap[renderer0].width = min(mPriTileMap[renderer0].width,
					mPriWindowWidth - 1);
		}
		//		cout << "new Tile-Dimensions for 1:" << endl;
		//		cout << "x, y: " << mPriTileMap[renderer0].xPos << ", " << mPriTileMap[renderer0].yPos << endl;
		//		cout << "width, height: " << mPriTileMap[renderer0].width << ", " << mPriTileMap[renderer0].height << endl;
		//		cout << "render-time: " << mPriTileMap[renderer0].renderTime << endl;
		//		cout << "new Tile-Dimensions for 2:" << endl;
		//		cout << "x, y: " << mPriTileMap[renderer1].xPos << ", " << mPriTileMap[renderer1].yPos << endl;
		//		cout << "width, height: " << mPriTileMap[renderer1].width << ", " << mPriTileMap[renderer1].height << endl;
		//		cout << "render-time: " << mPriTileMap[renderer1].renderTime << endl;
	} else if (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER) == 4) {
		//TODO implement
		int renderer0 = MpiControl::getSingleton()->getRenderGroup()[0];
		int renderer1 = MpiControl::getSingleton()->getRenderGroup()[1];
		int renderer2 = MpiControl::getSingleton()->getRenderGroup()[2];
		int renderer3 = MpiControl::getSingleton()->getRenderGroup()[3];
		double totalTime = mPriTileMap[renderer0].renderTime
				+ mPriTileMap[renderer1].renderTime
				+ mPriTileMap[renderer2].renderTime
				+ mPriTileMap[renderer3].renderTime;
		double totalXLeftTime = mPriTileMap[renderer0].renderTime
				+ mPriTileMap[renderer2].renderTime;
		double totalXRightTime = mPriTileMap[renderer1].renderTime
				+ mPriTileMap[renderer3].renderTime;
		double halfTime = totalTime / 2.0;
		double halfXLeftTime = totalXLeftTime / 2.0;
		double halfXRightTime = totalXRightTime / 2.0;

		 // left side
		if (mPriTileMap[renderer0].renderTime
				> mPriTileMap[renderer2].renderTime){ // renderer0 is slowest

		}
		else if (mPriTileMap[renderer0].renderTime
				< mPriTileMap[renderer2].renderTime){ // renderer2 is slowest

		}
		 // right side
		if (mPriTileMap[renderer1].renderTime
				> mPriTileMap[renderer3].renderTime){ // renderer1 is slowest

		}
		else if (mPriTileMap[renderer1].renderTime
				< mPriTileMap[renderer3].renderTime){ // renderer3 is slowest

		}
		// the y-border
		if (totalXLeftTime > totalXRightTime) { // left ist slowest
			double timeDiff = halfXLeftTime - halfTime; // difference from half-time
			double timeDiffPerc = (100.0 / totalXLeftTime)
					* timeDiff; // percent of difference from half-time
			int windowDiff = (int) ((mPriTileMap[renderer0].width / 100.0)
					* timeDiffPerc); // difference in width
			mPriTileMap[renderer0].width -= windowDiff;
			mPriTileMap[renderer2].width -= windowDiff;
			mPriTileMap[renderer1].xPos -= windowDiff;
			mPriTileMap[renderer1].width += windowDiff;
			mPriTileMap[renderer3].xPos -= windowDiff;
			mPriTileMap[renderer3].width += windowDiff;

			mPriTileMap[renderer0].width = max(mPriTileMap[renderer0].width, 1);
			mPriTileMap[renderer2].width = max(mPriTileMap[renderer0].width, 1);
			mPriTileMap[renderer1].xPos = max(mPriTileMap[renderer1].xPos, 1);
			mPriTileMap[renderer1].width = min(mPriTileMap[renderer1].width,
					mPriWindowWidth - 1);
			mPriTileMap[renderer3].xPos = max(mPriTileMap[renderer1].xPos, 1);
			mPriTileMap[renderer3].width = min(mPriTileMap[renderer1].width,
					mPriWindowWidth - 1);
		} else if (totalXLeftTime < totalXRightTime) { // renderer1 is slowest || right ist slowest
			double timeDiff = halfXRightTime - halfTime; // difference from half-time
			double timeDiffPerc = (100.0 / totalXRightTime)
					* timeDiff; // percent of difference from half-time
			int windowDiff = (int) ((mPriTileMap[renderer1].width / 100.0)
					* timeDiffPerc); // difference in width
			mPriTileMap[renderer1].width -= windowDiff;
			mPriTileMap[renderer1].xPos += windowDiff;
			mPriTileMap[renderer3].width -= windowDiff;
			mPriTileMap[renderer3].xPos += windowDiff;
			mPriTileMap[renderer0].width += windowDiff;
			mPriTileMap[renderer2].width += windowDiff;

			mPriTileMap[renderer1].width = max(mPriTileMap[renderer1].width, 1);
			mPriTileMap[renderer1].xPos = min(mPriTileMap[renderer1].xPos,
					mPriWindowWidth - 1);
			mPriTileMap[renderer3].width = max(mPriTileMap[renderer1].width, 1);
			mPriTileMap[renderer3].xPos = min(mPriTileMap[renderer1].xPos,
					mPriWindowWidth - 1);
			mPriTileMap[renderer0].width = min(mPriTileMap[renderer0].width,
					mPriWindowWidth - 1);
			mPriTileMap[renderer2].width = min(mPriTileMap[renderer0].width,
					mPriWindowWidth - 1);
		}

	}
}

void RenderMasterCore::notify(oocframework::IEvent& event) {
	if (event.instanceOf(WindowResizedEvent::classid())) {
		WindowResizedEvent& rwe = (WindowResizedEvent&) event;

	} else if (event.instanceOf(KeyPressedEvent::classid())) {
		KeyPressedEvent& kpe = (KeyPressedEvent&) event;
		switch (kpe.getKey()) {
		case GLFW_KEY_ESC:
			char data;
			MpiControl::getSingleton()->push(new Message(
					KillApplicationEvent::classid()->getShortId(), 1, 0, &data,
					MpiControl::DATA));
			MpiControl::getSingleton()->push(new Message(
					KillApplicationEvent::classid()->getShortId(), 1, 0, &data,
					MpiControl::RENDERER));
			mTerminateApplication = true;
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
	else if (event.instanceOf(MouseButtonEvent::classid())){
		mPriCamHasMoved = true;
	}
	else if (event.instanceOf(WindowClosedEvent::classid())) {
		char data;
		MpiControl::getSingleton()->push(new Message(
				KillApplicationEvent::classid()->getShortId(), 1, 0, &data,
				MpiControl::DATA));
		MpiControl::getSingleton()->push(new Message(
				KillApplicationEvent::classid()->getShortId(), 1, 0, &data,
				MpiControl::RENDERER));
		mTerminateApplication = true;
	}
}

