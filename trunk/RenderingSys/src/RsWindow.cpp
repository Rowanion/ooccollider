/*
 * RsWindow.cpp
 *
 *  Created on: 12.03.2010
 *      Author: TheAvatar
 */

#include "RsWindow.h"

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <iostream>

using namespace std;

RsWindow* RsWindow::instance = 0;

RsWindow* RsWindow::getSingleton()
{
	static Guard g;
	if( instance == 0 ){
		instance = new RsWindow();
	}
	return instance;
}

RsWindow::Guard::~Guard()
{
	if( RsWindow::instance != 0 )
		delete RsWindow::instance;
}

RsWindow::RsWindow() : mPriRenderer(0), mPriGlutInited(false){
	if (!mPriGlutInited){
		int argc = 0;
		char* argv [1];
		glutInit( &argc, argv );
		mPriGlutInited = true;
	}
	mPriWindowClosing = false;
	mPriWindowTitle = "RenderingSys";
	mPriWindow = 0;
}

RsWindow::~RsWindow() {
	// TODO Auto-generated destructor stub
}

void RsWindow::display()   // Create The Display Function
{

	instance->mPriRenderer->display();
}

void RsWindow::reshape ( int _w, int _h )   // Create The Reshape Function (the viewport)
{
	instance->mPriRenderer->reshape(_w, _h);
}

void RsWindow::close()
{
	instance->mPriWindowClosing = true;
	glutLeaveMainLoop();
	instance->mPriRenderer->exit();
}

void RsWindow::keyboard(unsigned char _key, int _x, int _y)
{
	instance->mPriRenderer->keyboard(_key, _x, _y);
}

void RsWindow::specialKeys(int _key, int _x, int _y)
{
	instance->mPriRenderer->specialKeys(_key, _x, _y);
}

void RsWindow::processMouse(int _button, int _state, int _x, int _y)
{
	instance->mPriRenderer->processMouse(_button, _state, _x, _y);
}

void RsWindow::processMouseActiveMotion(int _x, int _y)
{
	instance->mPriRenderer->processMouseActiveMotion(_x, _y);
}

void RsWindow::processMousePassiveMotion(int _x, int _y)
{
	instance->mPriRenderer->processMousePassiveMotion(_x, _y);
}

void RsWindow::processMouseEntry(int _state)
{
	instance->mPriRenderer->processMouseEntry(_state);
}

void RsWindow::processMouseWheel( int _wheel, int _direction, int _x, int _y)
{
	instance->mPriRenderer->processMouseWheel(_wheel, _direction, _x, _y);
}

void RsWindow::animator(int _value)
{
	if (!instance->mPriWindowClosing){
		instance->mPriRenderer->animator();
		glutTimerFunc(1, RsWindow::animator, 0);
	}
}

void RsWindow::init()     // Create Some Everyday Functions
{
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
	glutInitWindowSize( 500, 500 ); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow(mPriWindowTitle.c_str()); // Window Title (argv[0] for current directory as title)
	glutDisplayFunc( display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc( reshape );
	glutCloseFunc(close);
	int check = 0;
	mPriRenderer->keyboard(1, 1, 1, &check);
	if (check > 0){
		glutKeyboardFunc( keyboard );
	}
	check = 0;
	mPriRenderer->specialKeys(1, 1, 1, &check);
	if (check > 0){
		glutSpecialFunc( specialKeys );
	}
	check = 0;
	mPriRenderer->processMouse(1, 1, 1, 1, &check);
	if (check > 0){
		glutMouseFunc(processMouse);
	}
	check = 0;
	mPriRenderer->processMouseActiveMotion(1, 1, &check);
	if (check > 0){
		glutMotionFunc(processMouseActiveMotion);
	}
	check = 0;
	mPriRenderer->processMousePassiveMotion(1, 1, &check);
	if (check > 0){
		glutPassiveMotionFunc(processMousePassiveMotion);
	}
	check = 0;
	mPriRenderer->processMouseEntry(1, &check);
	if (check > 0){
		glutEntryFunc(processMouseEntry);
	}
	check = 0;
	mPriRenderer->animator(&check);
	if (check > 0){
		glutTimerFunc(5, animator, 0);
	}
	check = 0;
	mPriRenderer->processMouseWheel(1, 1, 1, 1, &check);
	if (check > 0){
		glutMouseWheelFunc(processMouseWheel);
	}
	check = 0;
}

void RsWindow::attachRenderer(RsAbstractRenderer* _renderer)
{
	if (mPriRenderer != 0){
		detachRenderer();
	}
	mPriRenderer = _renderer;
	//TODO do not reopen the window if already open....
	init();
}

void RsWindow::detachRenderer()
{
	//TODO
	mPriRenderer = 0;
}

void RsWindow::start()
{

	mPriRenderer->init();

	glutMainLoop();
}

void RsWindow::setWindowTitle(const char* _title)
{
	mPriWindowTitle.assign(_title);
	if (mPriWindow != 0)
		glutSetWindowTitle(mPriWindowTitle.c_str());
}

RsAbstractRenderer* RsWindow::getRenderer() const
{
	return mPriRenderer;
}
