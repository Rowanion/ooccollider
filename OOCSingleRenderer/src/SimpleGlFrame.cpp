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
#include <sstream>

#include "GeometricOps.h"
#include "V3f.h"
#include "EventManager.h"
#include "MouseDraggedEvent.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "KeyPressedEvent.h"
#include "Message.h"
#include "MouseWheelEvent.h"
#include "FboFactory.h"
#include "InfoRequestEvent.h"
#include "CameraMovedEvent.h"
#include "Logger.h"
#include "Log.h"
#include "KeyReleasedEvent.h"


using namespace std;
using namespace ooctools;
using namespace oocformats;
using namespace oocframework;

SimpleGlFrame::SimpleGlFrame() : AbstractGlFrame(0,0,0,0),
	scale(1.0f), avgFps(0.0f), time(0.0), frame(0), mPriVboMan(0), mPriCgt(0), mPriIVbo(0), mPriCamera(OOCCamera()), walkingSpeed(50.3f),
	mPriFboWire(0), mPriFboSolid(0), mPriColorBuffer(0), mPriCBufWidth(0), mPriCBufHeight(0), mPriCBufX(0), mPriCBufY(0), mPriUseSpaceNav(false),
	mPrilockTrans(false), mPrilockRot(false), mPriStepLeft(false), mPriStepRight(false), mPriStepForward(false), mPriStepBackward(false),
	mPriRollLeft(false), mPriRollRight(false), mPriTiltUp(false), mPriTiltDown(false), mPriTurnLeft(false), mPriTurnRight(false), mPriTurnUp(false),
	mPriTurnDown(false), mPriBBMode(0), mPriOh(OctreeHandler())
{
	// TODO Auto-generated constructor stub

	mPriColorTable = ColorTable(string(BASE_MODEL_PATH) + string("/colortable.bin"));

	for (unsigned i = 0; i < 10; ++i) {
		fps[i] = 0.0f;
	}

	oocframework::EventManager::getSingleton()->addListener(this, MouseDraggedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseButtonEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, MouseWheelEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, KeyReleasedEvent::classid());
	oocframework::EventManager::getSingleton()->addListener(this, InfoRequestEvent::classid());
}

SimpleGlFrame::~SimpleGlFrame() {

	delete mPriFboWire;
	delete mPriFboSolid;

	oocframework::EventManager::getSingleton()->removeListener(this, MouseDraggedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseButtonEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, MouseWheelEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, KeyPressedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, KeyReleasedEvent::classid());
	oocframework::EventManager::getSingleton()->removeListener(this, InfoRequestEvent::classid());
	Logger::getSingleton()->closeAllHandles();
}

void SimpleGlFrame::init() {
	glewInit();
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);




//	camObj.positionCamera(0.0,0.0,5.0,
//			0.0,0.0,-3.0,
//			0,1,0);

	GET_GLERROR(0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	mPriCamera.initMatrices();
	mPriCamera.calcMatrix();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);
	mPriColorTable.setupTexture();

	mPriMinLvl = 7;
	loadCameraPositions();
	mPriFboWire = FboFactory::getSingleton()->createCompleteFbo(mPriCBufWidth,mPriCBufHeight);
	mPriFboSolid = FboFactory::getSingleton()->createCompleteFbo(mPriCBufWidth,mPriCBufHeight);
	mPriCgt = ooctools::CgToolkit::getSingleton();
	setupCg();
	mPriLo = mPriOh.loadLooseRenderOctreeSkeleton(fs::path(string(BASE_MODEL_PATH)+"/skeleton.bin"));


}

void SimpleGlFrame::setupCg()
{
	mPriCgt->initCG(true);

//	cgVertPostProc = mPriCgt->loadCgShader(mPriCgt->cgVertexProfile, "shader/gauss.cg", true, "gaussVP");
	cgFragPostProc = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/gauss.cg", true, "gaussFP");
	cgFragPostProc = mPriCgt->loadCgShader(mPriCgt->cgFragProfile, "shader/fp_boxcar.cg", true, "");
	cgTexture = cgGetNamedParameter(cgFragPostProc,  "texture");
	cgGLSetTextureParameter(cgTexture, mPriFboSolid->getColorTexId());
	cgGLEnableTextureParameter(cgTexture);

}
void SimpleGlFrame::display()
{
//	mPriCamera.setRotationMatrix(mPriModelViewMatrix);

//	glLoadMatrixf(mPriModelViewMatrix);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////	glLoadIdentity();
	applyKeyEvents();
	if (mPriButtonActions[0] == GLFW_PRESS && mPriButtonActions[1] == GLFW_PRESS){
    	mPriCamera.decZMove(0.1*walkingSpeed);
	}
	else if (mPriButtonActions[1] == GLFW_PRESS){
		mPriCamera.incZMove(0.1*walkingSpeed);
	}
//	camObj.applyToGL();
////	mPriCamera.calcMatrix();

//	char* data = new char[16*sizeof(float)];
//	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)data);
//	if (MpiControl::getSingleton()->outQueueEmpty()){
//		Message* msg = new Message(ModelViewMatrixEvent::classid()->getShortId(),16*sizeof(float),1,data);
//		MpiControl::getSingleton()->push(msg);
//	}

//	mPriCgt->startCgShader(mPriCgt->cgVertexProfile, cgVertPostProc);
//	mPriCgt->startCgShader(mPriCgt->cgFragProfile, cgFragPostProc);
	mPriFboWire->bind();
//	glLoadMatrixf(mPriModelViewMatrix);
	glLoadIdentity();
	mPriCamera.calcMatrix();
	glClearColor(0.5490196078f, 0.7607843137f, 0.9803921569f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderBBs(mPriLo);
	mPriFboWire->unbind();
	mPriFboWire->drawColorFSQuad();

//	mPriCgt->stopCgShader(mPriCgt->cgVertexProfile);
//	mPriCgt->stopCgShader(mPriCgt->cgFragProfile);

	double t = glfwGetTime();  // Time (in seconds)


	double diff = t-time;
	fps[frame%10] = 1.0/diff;
	time = t;
	++frame;
	if (frame>=25){
		frame = 0;
	}
	calcFPS();

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
	gluPerspective(45.0f, ratio, 0.1f, 5000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,0.0,5.0,
		      0.0,0.0,-3.0,
			  0.0f,1.0f,0.0f);
}

void SimpleGlFrame::calcFPS() {
	avgFps = 0.0f;
	for (unsigned i = 0; i < 10; ++i) {
		avgFps += fps[i];
	}
	avgFps /= 10.0f;

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

const float* SimpleGlFrame::createMvMatrix()
{
	glLoadIdentity();
	mPriCamera.calcMatrix();
	glGetFloatv(GL_MODELVIEW_MATRIX, mPriModelViewMatrix);
	return mPriModelViewMatrix;
}

void SimpleGlFrame::applyKeyEvents()
{
	if (mPriStepForward){
		mPriCamera.incZMove(0.1*walkingSpeed);
	}
	if (mPriStepBackward){
		mPriCamera.decZMove(0.1*walkingSpeed);
	}
	if (mPriStepLeft){
		mPriCamera.incXMove(0.1*walkingSpeed);
	}
	if (mPriStepRight){
		mPriCamera.decXMove(0.1*walkingSpeed);
	}
	if (mPriRollLeft){
		mPriCamera.setZRot(-1.0);
	}
	if (mPriRollRight){
		mPriCamera.setZRot(1.0);
	}
	if (mPriTiltUp){
		mPriCamera.decYMove(0.1*walkingSpeed);
	}
	if (mPriTiltDown){
		mPriCamera.incYMove(0.1*walkingSpeed);
	}
	if (mPriTurnLeft){
		mPriCamera.setYRot(1.0);
	}
	if (mPriTurnRight){
		mPriCamera.setYRot(-1.0);
	}
	if (mPriTurnUp){
		mPriCamera.setXRot(1.0);
	}
	if (mPriTurnDown){
		mPriCamera.setXRot(-1.0);
	}
}

void SimpleGlFrame::renderBBs(LooseRenderOctree* lro)
{
	glColor3f(1.0f, 0.0f, 0.0f);


//		glBegin(GL_LINES);
//			glVertex3fv(lro->getBb().getMin().getData());
//			glVertex3fv(lro->getBb().getMax().getData());
//		glEnd();
	LooseRenderOctree* child = 0;
	for (unsigned i=0; i< 8; ++i){
		child = lro->getChild(i);
		if (child != 0){
			if (child->getLevel() == mPriMinLvl){
				lro->getBb().draw(1.0f, 0.0f, 0.0f);
			}
			else if (child->hasData()){
				renderBBs(child);
			}
		}
	}

}

void SimpleGlFrame::notify(oocframework::IEvent& event)
{
	if (event.instanceOf(MouseDraggedEvent::classid())){
		MouseDraggedEvent& mde = (MouseDraggedEvent&)event;
//		cout << "MouseDragged: " << mde.getX() << ", " << mde.getY() << endl;
		mPriCamera.setXRot(oldPosY - mde.getY());
		mPriCamera.setYRot(oldPosX - mde.getX());

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
//		cout << "MouseButton" << endl;
		if (mPriButtonActions[mbe.getButton()] != mbe.getState()){
			mPriButtonActions[mbe.getButton()] = mbe.getState();

//			if (mbe.getButton() == 0 && mPriButtonActions[1] == GLFW_RELEASE){
				oldPosX = mbe.getX();
				oldPosY = mbe.getY();
//			}

		}

	}
	if (event.instanceOf(KeyReleasedEvent::classid())){
		KeyReleasedEvent& kre = (KeyReleasedEvent&)event;
//		cout << "KeyReleased" << endl;
	    switch (kre.getKey()) {
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
	    case GLFW_KEY_PAGEUP:
			mPriTiltUp = false;
			break;
	    case GLFW_KEY_PAGEDOWN:
			mPriTiltDown = false;
	        break;
	    case GLFW_KEY_UP:
			mPriTurnUp = false;
	        break;
	    case GLFW_KEY_DOWN:
			mPriTurnDown = false;
	        break;
	    case GLFW_KEY_LEFT:
			mPriTurnLeft = false;
	        break;
	    case GLFW_KEY_RIGHT:
			mPriTurnRight = false;
	        break;
	    case (int)'O':
	    	mPriBBMode++;
			if (mPriBBMode>3){
				mPriBBMode = 0;
			}
	        break;
	    }
	}
	if (event.instanceOf(KeyPressedEvent::classid())){
		KeyPressedEvent& kpe = (KeyPressedEvent&)event;
//		cout << "KeyPressed" << endl;
	    switch (kpe.getKey()) {
			case GLFW_KEY_KP_ADD:
				mPriMinLvl++;
			break;
			case GLFW_KEY_KP_SUBTRACT:
				mPriMinLvl--;
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
			case 'C':{ // dump current matrix and exit
				cout << "dumping current matrix...." << endl;
				for(unsigned i=0; i<16; ++i){
					cout << mPriModelViewMatrix[i] << endl;
				}
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
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
					mPriCamera.initMatrices();
					mPriCamera.setRotationMatrix(mPriSavedPositions[pos]);
				}
				break;}
			default:
			break;
		}
	}
	else if (event.instanceOf(MouseWheelEvent::classid())){
		MouseWheelEvent& mwe = (MouseWheelEvent&)event;
//		cout << "MouseWheel" << endl;
		if (mwe.getDirection() == MouseWheelEvent::UP){
			walkingSpeed += (0.6f);
		}
		else if  (mwe.getDirection() == MouseWheelEvent::DOWN){
			walkingSpeed -= (0.6f);
		}
	}
	else if (event.instanceOf(InfoRequestEvent::classid())){
//		cout << "InfoRequest" << endl;
		stringstream headerS;
		for (int i=0; i < MpiControl::getSingleton()->getRank()*2 +1 ; ++i){
			headerS << "-";
		}
		headerS << "> (" << MpiControl::getSingleton()->getRank() << ") - ";
		cout << headerS.str() << "INFO" << endl;
		cout << headerS.str() << "Current walkingspeed: " << walkingSpeed << endl;
		cout << "---------------------------------------" << endl;
	}


}
