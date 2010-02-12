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
#include "MouseButtonEvent.h"
#include "CameraMovedEvent.h"
#include "KeyPressedEvent.h"
#include "WindowClosedEvent.h"
#include "MouseWheelEvent.h"
#include "SimpleGlFrame.h"
#include "InfoRequestEvent.h"
#include "Logger.h"
#include "Log.h"
#include "KeyReleasedEvent.h"

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
			true), mPriFrameCount(DEPTHBUFFER_INTERVAL), mPriRenderTimeCount(0),
			mPriRenderTimes(vector<double> (MpiControl::getSingleton()->getGroupSize(MpiControl::RENDERER), 0.5)),
			mPriDataLoad(map<int, unsigned>()), mPriFrameTick(0),
			mPriMTwister(PRESELECTED_SEED),
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
	mPriEventMan->addListener(this, MouseButtonEvent::classid());
	mPriEventMan->addListener(this, MouseDraggedEvent::classid());
	mPriEventMan->addListener(this, KeyPressedEvent::classid());
	mPriEventMan->addListener(this, KeyReleasedEvent::classid());
	mPriEventMan->addListener(this, WindowClosedEvent::classid());
	mPriEventMan->addListener(this, InfoRequestEvent::classid());
	mPriEventMan->addListener(this, CameraMovedEvent::classid());

	mPriGlFrame->init();



	int id = 0;
	mPriPosition = -1;
	mPriFPSTest = false;
	mPriRequestSave = false;

//	cout << "-----------------------1: " << newCoords[1].xPos << ", " << newCoords[1].yPos << ", " << newCoords[1].width << ", " << newCoords[1].height << endl;
//	cout << "-----------------------2: " << newCoords[2].xPos << ", " << newCoords[2].yPos << ", " << newCoords[2].width << ", " << newCoords[2].height << endl;
//	cout << "-----------------------3: " << newCoords[3].xPos << ", " << newCoords[3].yPos << ", " << newCoords[3].width << ", " << newCoords[3].height << endl;
//	cout << "-----------------------4: " << newCoords[4].xPos << ", " << newCoords[4].yPos << ", " << newCoords[4].width << ", " << newCoords[4].height << endl;
//	exit(0);




	//	glFrame->setVbo(new IndexedVbo(fs::path("/media/ClemensHDD/B3_SampleTree/data/0/1.idx")));




	// Main rendering loop
	unsigned frames = 0;
	double newTime = 0.0;
	double newerTime = 0.0;


	do {

		if (!mTerminateApplication) {
			if (mPriFrameCount >= (DEPTHBUFFER_INTERVAL - 1) && mPriCamHasMoved == true){
//			if (mPriFrameCount >= (DEPTHBUFFER_INTERVAL - 1)){
				newTime = glfwGetTime();
				mPriCamHasMoved = false;
				mPriFrameCount = 0;
				newerTime = glfwGetTime();
//				cout << "[0] time between depthbuffer renewal: " << newerTime-newTime<< endl;
			}

			//send matrix/camera to when the out-queue is empty
			//			cout << "---master sending matrix" << endl;
			//			for (int i=1; i<mPriMpiCon->getSize(); ++i){

			mPriGlFrame->display();

			checkForActiveMovement();
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
			ss << "MasterNode (" << MpiControl::getSingleton()->getRank() << ") - FPS: " << mPriGlFrame->getFrames();
			mWindow->setTitle(ss.str());
		}
	}while (mRunning);
	cout << "end of display loop." << endl;
}

RenderMasterCore::~RenderMasterCore() {
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
	if (event.instanceOf(KeyReleasedEvent::classid())) {
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
			mTerminateApplication = true;
			break;}
		case GLFW_KEY_KP_ADD:
			break;
		case GLFW_KEY_KP_SUBTRACT:
			break;
		case 'N':
			break;
        case 'K':
                mPriFPSTest = !mPriFPSTest;
                break;
		case 'F':
			break;
		case 'I': {
			cout << "====================================" << endl;
			cout << "Requesting Information from Nodes..." << endl;
			cout << "====================================" << endl;
			InfoRequestEvent ire = InfoRequestEvent();
			oocframework::EventManager::getSingleton()->fire(ire);
			break;
		}
		case 'R':
			// do not update the depthbuffer
			break;
		case 'X':{
			if (kpe.isCtrl()){
			}
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
		mTerminateApplication = true;
	}
}

