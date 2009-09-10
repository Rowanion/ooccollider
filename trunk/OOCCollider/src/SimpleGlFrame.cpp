/**
 * @file	SimpleGlFrame.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	SimpleGlFrame class declaration.
 */

#include "SimpleGlFrame.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <cmath>
#include <iostream>
#include <cstring>

#include <X11/Xlib.h>
#include <spnav.h>

#include "GeometricOps.h"
#include "V3f.h"
#include "EventManager.h"
#include "MouseDraggedEvent.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "ModelViewMatrixEvent.h"
#include "KeyPressedEvent.h"
#include "Message.h"
#include "MouseWheelEvent.h"
#include "FboFactory.h"
#include "ColorBufferEvent.h"
#include "KillApplicationEvent.h"
#include "InfoRequestEvent.h"


using namespace std;
using namespace ooctools;
using namespace oocframework;

SimpleGlFrame::SimpleGlFrame() :
	nearPlane(0.1f), farPlane(3200.0f), scale(1.0f), avgFps(0.0f), time(0.0), frame(0), mPriVboMan(0), mPriCgt(0), mPriIVbo(0), walkingSpeed(0.3f),
	mPriFbo(0), mPriColorBuffer(0), mPriCBufWidth(0), mPriCBufHeight(0), mPriCBufX(0), mPriCBufY(0), mPriUseSpaceNav(false), mPrilockTrans(false), mPrilockRot(false)
{
	// TODO Auto-generated constructor stub

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}


	// cam setup
	xmove = 0.0;
	ymove = 0.0;
	zmove = 0.0;

	myXRot = 0.0f;
	myYRot = 0.0f;
	myZRot = 0.0f;

	myTranslateMatrix = new float[16];
	myGLRotMatrix = new float[16];

	first = true;
	myQuatRotMat  = new float[16];
	myTempMat = new float[16];
	localQuat = Quaternion();
	totalQuat = Quaternion();


	oocframework::EventManager::getSingleton()->addListener(this, ColorBufferEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseDraggedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseButtonEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseWheelEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, InfoRequestEvent::classid());
}

SimpleGlFrame::~SimpleGlFrame() {

	delete[] myTranslateMatrix;
	delete[] myGLRotMatrix;

	oocframework::EventManager::getSingleton()->removeListener(this, ColorBufferEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseDraggedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseButtonEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseWheelEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, InfoRequestEvent::classid());

}

void SimpleGlFrame::init() {
	glewInit();
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	camObj.positionCamera(0.0,0.0,5.0,
			0.0,0.0,-3.0,
			0,1,0);

	GET_GLERROR(0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	initMatrices();

	if(spnav_open()==-1) {
		fprintf(stderr, "failed to connect to the space navigator daemon\n");
	}
	else{
		mPriUseSpaceNav=true;
	}
	loadCameraPositions();
}

void SimpleGlFrame::display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (mPriButtonActions[0] == GLFW_PRESS && mPriButtonActions[1] == GLFW_PRESS){
    	zmove -= (0.1*walkingSpeed);
	}
	else if (mPriButtonActions[1] == GLFW_PRESS){
    	zmove += (0.1*walkingSpeed);
	}
	camObj.applyToGL();
	calcMatrix();

//	char* data = new char[16*sizeof(float)];
//	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)data);
//	if (MpiControl::getSingleton()->outQueueEmpty()){
//		Message* msg = new Message(ModelViewMatrixEvent::classid()->getShortId(),16*sizeof(float),1,data);
//		MpiControl::getSingleton()->push(msg);
//	}

	if (mPriColorBuffer != 0){
		FboFactory::getSingleton()->drawColorToFb(mPriColorBuffer, mPriCBufX, mPriCBufY, mPriCBufWidth, mPriCBufHeight);
		GET_GLERROR(0);
	}
	pollSpaceNav();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);

}

void SimpleGlFrame::reshape(int width, int height) {
	mPriCBufWidth = width;
	mPriCBufHeight = height;
	int w = width;
	int h = height;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, (GLint)w, (GLint)h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.01f, 4000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
}

/* CAMERAPART BEGIN */

void SimpleGlFrame::initMatrices(){
	myTranslateMatrix[0]  = 1.0f;
	myTranslateMatrix[1]  = 0;
	myTranslateMatrix[2]  = 0;
	myTranslateMatrix[3]  = 0;

	myTranslateMatrix[4]  = 0;
	myTranslateMatrix[5]  = 1;
	myTranslateMatrix[6]  = 0;
	myTranslateMatrix[7]  = 0;

	myTranslateMatrix[8]  = 0;
	myTranslateMatrix[9]  = 0;
	myTranslateMatrix[10] = 1;
	myTranslateMatrix[11] = 0;

	myTranslateMatrix[12] = 0;
	myTranslateMatrix[13] = 0;
	myTranslateMatrix[14] = 0;
	myTranslateMatrix[15] = 1;

	myGLRotMatrix[0]  = 1;
	myGLRotMatrix[1]  = 0;
	myGLRotMatrix[2]  = 0;
	myGLRotMatrix[3]  = 0;

	myGLRotMatrix[4]  = 0;
	myGLRotMatrix[5]  = 1;
	myGLRotMatrix[6]  = 0;
	myGLRotMatrix[7]  = 0;

	myGLRotMatrix[8]  = 0;
	myGLRotMatrix[9]  = 0;
	myGLRotMatrix[10] = 1;
	myGLRotMatrix[11] = 0;

	myGLRotMatrix[12] = 0;
	myGLRotMatrix[13] = 0;
	myGLRotMatrix[14] = 0;
	myGLRotMatrix[15] = 1;
}

void SimpleGlFrame::calcMatrix(){
	localQuat.fromEulerAngles(myYRot, myZRot, myXRot);
	totalQuat.reset();
	totalQuat = localQuat * totalQuat;
	totalQuat.getRotationMatrix(myQuatRotMat);

	multMatrix(myQuatRotMat, myGLRotMatrix, myTempMat);

        myTranslateMatrix[12] = xmove;
        myTranslateMatrix[13] = ymove;
        myTranslateMatrix[14] = zmove;

	multMatrix(myTranslateMatrix, myTempMat, myGLRotMatrix);

	glMultMatrixf(myGLRotMatrix);

	myXRot = myYRot = myZRot = 0.0f;
	xmove = ymove = zmove = 0.0f;
}

void SimpleGlFrame::multMatrix(float *m1, float *m2, float *res){
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j){
			res[i*4 + j] = m1[j]*m2[i*4] + m1[j+4]*m2[i*4+1] + m1[j+8]*m2[i*4+2] + m1[j+12]*m2[i*4+3];
		}
	}
}

/* CAMERAPART END */

void SimpleGlFrame::pollSpaceNav()
{
	spnav_event sev;
	if (mPriUseSpaceNav){
		while(spnav_poll_event(&sev)) {
			if(sev.type == SPNAV_EVENT_MOTION) {
//				printf("got motion event: t(%d, %d, %d) ", sev.motion.x, sev.motion.y, sev.motion.z);
//				printf("r(%d, %d, %d)\n", sev.motion.rx, sev.motion.ry, sev.motion.rz);
				xmove -= 0.1*sev.motion.x;
				ymove -= 0.1*sev.motion.y;
				zmove += 0.1*sev.motion.z;
				myXRot = 0.005f*sev.motion.rx;
				myYRot = 0.01f*sev.motion.ry;
				myZRot = -0.005f*sev.motion.rz;
			} else {	/* SPNAV_EVENT_BUTTON */
//				printf("got button %s event b(%d)\n", sev.button.press ? "press" : "release", sev.button.bnum);
				// TODO use lock rotation/lock translation function
			}
		}

	}
}

void SimpleGlFrame::loadCameraPositions()
{
	//TODO verify if this works
	fs::ifstream camPosFile;
	camPosFile.open(fs::path("camerapositions.data"), ios::binary);
	camPosFile.seekg(0, ios::beg);
	unsigned matrixCount = 0;
	while (!camPosFile.eof()){
		camPosFile.read((char*)mPriSavedPositions[matrixCount], 16*sizeof(float));
		matrixCount++;
		camPosFile.seekg(16*sizeof(float)*matrixCount, ios::beg);
	}
	camPosFile.close();
	cout << "loaded camera positions from file" << endl;
}

void SimpleGlFrame::writeCameraPositions()
{
	//TODO verify if this works
	fs::ofstream camPosFile;
	camPosFile.open(fs::path("./camerapositions.data"), ios::binary);
	camPosFile.seekp(0, ios::beg);
	for (unsigned i=0; i<10; ++i){
		camPosFile.write((char*)mPriSavedPositions[i], 16*sizeof(float));
		camPosFile.seekp(0, ios::end);
	}
	camPosFile.close();
	cout << "written camera positions to file" << endl;
}

void SimpleGlFrame::cameraInit()
{
	for (unsigned i=0; i<10; ++i){
		if (i == 2){
			mPriSavedPositions[i][0] = -0.78640f;
			mPriSavedPositions[i][1] = -0.47611f;
			mPriSavedPositions[i][2] = -0.393593f;
			mPriSavedPositions[i][3] = 0.0f;
			mPriSavedPositions[i][4] = -0.40381f;
			mPriSavedPositions[i][5] = -0.0859697f;
			mPriSavedPositions[i][6] = 0.910807f;
			mPriSavedPositions[i][7] = 0.0f;
			mPriSavedPositions[i][8] = -0.467475f;
			mPriSavedPositions[i][9] = 0.875187f;
			mPriSavedPositions[i][10] = -0.124651f;
			mPriSavedPositions[i][11] = 0.0f;
			mPriSavedPositions[i][12] = 15.3023f;
			mPriSavedPositions[i][13] = 38.3899f;
			mPriSavedPositions[i][14] = 160.453f;
			mPriSavedPositions[i][15] = 1.0f;
		}
		else {
			mPriSavedPositions[i][0] = 1.0f;
			mPriSavedPositions[i][1] = 0.0f;
			mPriSavedPositions[i][2] = 0.0f;
			mPriSavedPositions[i][3] = 0.0f;
			mPriSavedPositions[i][4] = 0.0f;
			mPriSavedPositions[i][5] = 1.0f;
			mPriSavedPositions[i][6] = 0.0f;
			mPriSavedPositions[i][7] = 0.0f;
			mPriSavedPositions[i][8] = 0.0f;
			mPriSavedPositions[i][9] = 0.0f;
			mPriSavedPositions[i][10] = 1.0f;
			mPriSavedPositions[i][11] = 0.0f;
			mPriSavedPositions[i][12] = 0.0f;
			mPriSavedPositions[i][13] = 0.0f;
			mPriSavedPositions[i][14] = -5.0f;
			mPriSavedPositions[i][15] = 1.0f;
		}
	}
	writeCameraPositions();
}

void SimpleGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(MouseDraggedEvent::classid())){
		MouseDraggedEvent& mde = (MouseDraggedEvent&)event;
//		cout << "MouseDragged: " << mde.getX() << ", " << mde.getY() << endl;
		myXRot = (oldPosY - mde.getY());
		myYRot = (oldPosX - mde.getX());

	    oldPosX = mde.getX();
	    oldPosY = mde.getY();

//		float matrix[16];
//		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
//
//		ModelViewMatrixEvent mve = ModelViewMatrixEvent(mModelViewMatrix);
//		oocframework::EventManager::getSingleton()->fire(mve);
//	    mPriCamHasMoved = true;
	}
	else if (event.instanceOf(MouseButtonEvent::classid())){
		MouseButtonEvent& mbe = (MouseButtonEvent&)event;
		if (mPriButtonActions[mbe.getButton()] != mbe.getState()){
			mPriButtonActions[mbe.getButton()] = mbe.getState();

//			if (mbe.getButton() == 0 && mPriButtonActions[1] == GLFW_RELEASE){
				oldPosX = mbe.getX();
				oldPosY = mbe.getY();
//			}

		}

	}
	if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& mde = (KeyPressedEvent&)event;
	    switch (mde.getKey()) {
			case GLFW_KEY_PAGEUP: // tilt up
				ymove -= 0.1;
				break;

			case GLFW_KEY_PAGEDOWN: // tilt down
				ymove += 0.1;
			break;

			case GLFW_KEY_UP: // walk forward (bob head)
				myXRot = 1.0;
			break;

			case GLFW_KEY_DOWN: // walk back (bob head)
				myXRot = -1.0;
			break;

			case GLFW_KEY_LEFT: // look left(int)
				myYRot = 1.0;
			break;

			case GLFW_KEY_RIGHT: // look right
				myYRot = -1.0;
			break;
		    case (int)'W':
		    	zmove += (0.1*walkingSpeed);
		    	break;
		    case (int)'S':
		    	zmove -= (0.1*walkingSpeed);
		        break;
		    case (int)'A':
		    	xmove += (0.1*walkingSpeed);
		        break;
		    case (int)'D':
		    	xmove -= (0.1*walkingSpeed);
		        break;
		    case (int)'Q':
		    	myZRot = 1.0;
		        break;
		    case (int)'E':
		    	myZRot = -1.0;
		        break;
			case 'C':{ // dump current matrix and exit
				cout << "dumping current matrix...." << endl;
				for(unsigned i=0; i<16; ++i){
					cout << mPriModelViewMatrix[i] << endl;
				}
				KillApplicationEvent kae = KillApplicationEvent();
				oocframework::EventManager::getSingleton()->fire(kae);
				break;}
//			case 'L':{ // dump current matrix and exit
//				cout << "loading matrix...." << endl;
//				initMatrices();
//				myGLRotMatrix[0] = -0.78640;
//				myGLRotMatrix[1] = -0.47611;
//				myGLRotMatrix[2] = -0.393593;
//				myGLRotMatrix[3] = 0;
//				myGLRotMatrix[4] = -0.40381;
//				myGLRotMatrix[5] = -0.0859697;
//				myGLRotMatrix[6] = 0.910807;
//				myGLRotMatrix[7] = 0;
//				myGLRotMatrix[8] = -0.467475;
//				myGLRotMatrix[9] = 0.875187;
//				myGLRotMatrix[10] = -0.124651;
//				myGLRotMatrix[11] = 0;
//				myGLRotMatrix[12] = 15.3023;
//				myGLRotMatrix[13] = 38.3899;
//				myGLRotMatrix[14] = 160.453;
//				myGLRotMatrix[15] = 1;
//				break;}
			case '0':{
				// save current camera-position to 'pos'
				unsigned pos = 0;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '1':{
				// save current camera-position to 'pos'
				unsigned pos = 1;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '2':{
				// save current camera-position to 'pos'
				unsigned pos = 2;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '3':{
				// save current camera-position to 'pos'
				unsigned pos = 3;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '4':{
				// save current camera-position to 'pos'
				unsigned pos = 4;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '5':{
				// save current camera-position to 'pos'
				unsigned pos = 5;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '6':{
				// save current camera-position to 'pos'
				unsigned pos = 6;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '7':{
				// save current camera-position to 'pos'
				unsigned pos = 7;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '8':{
				// save current camera-position to 'pos'
				unsigned pos = 8;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			case '9':{
				// save current camera-position to 'pos'
				unsigned pos = 9;
				if (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS){
					glGetFloatv(GL_MODELVIEW_MATRIX, mPriSavedPositions[pos]);
					writeCameraPositions();
				}
				// read camera-position from 'pos', and set it to it.
				else {
					initMatrices();
					memcpy(myGLRotMatrix, mPriSavedPositions[pos], 16*sizeof(float));
				}
				break;}
			default:
			break;
		}
	}
	else if (event.instanceOf(MouseWheelEvent::classid())){
		MouseWheelEvent& mwe = (MouseWheelEvent&)event;
		if (mwe.getDirection() == MouseWheelEvent::UP){
			walkingSpeed += (0.6f);
		}
		else if  (mwe.getDirection() == MouseWheelEvent::DOWN){
			walkingSpeed -= (0.6f);
		}
	}
	if (event.instanceOf(ColorBufferEvent::classid())){
		ColorBufferEvent& cbe = (ColorBufferEvent&)event;
		if (mPriColorBuffer==0){
			mPriColorBuffer = new GLubyte[cbe.getHeight()*cbe.getWidth()*4];
			cout << "der Master hat nen colorbuffer initialisiert...." << cbe.getHeight() << " " << cbe.getWidth() << endl;
			cout << "pre init cbuf: " << (int64_t)mPriColorBuffer << endl;
//			for (unsigned i=0; i<cbe.getHeight()*cbe.getWidth()*4; i+=4){
//				mPriColorBuffer[i] = 255;
//				mPriColorBuffer[i+1] = 0;
//				mPriColorBuffer[i+2] = 0;
//				mPriColorBuffer[i+3] = 255;

	//		}
			cout << "post init cbuf: " << (int64_t)mPriColorBuffer << endl;
		}
//		exit(0);
		memcpy(mPriColorBuffer, cbe.getPixel(), cbe.getHeight()*cbe.getWidth()*sizeof(GLubyte)*4);
	}
	else if (event.instanceOf(InfoRequestEvent::classid())){
		cout << "(" << MpiControl::getSingleton()->getRank() << ") - INFO" << endl;
		cout << "---------------------------------------" << endl;
	}


}