/*
 * RsRendererImpl.cpp
 *
 *  Created on: 12.03.2010
 *      Author: TheAvatar
 */

#include "RsRendererImpl.h"

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <cstdlib>
#include <iostream>

#include "RsStructs.h"
#include "RsImageTools.h"
#include "RsMeshTools.h"

using namespace std;

RsRendererImpl::RsRendererImpl()
{
	mPriStepForward = false;
	mPriStepBackward = false;
	mPriStepLeft = false;
	mPriStepRight = false;
	mPriRollLeft = false;
	mPriRollRight = false;
	mPriTiltUp = false;
	mPriTiltDown = false;
	mPriTurnLeft = false;
	mPriTurnRight = false;
	mPriTurnUp = false;
	mPriTurnDown = false;
	mPriPrevMouseX = 0;
	mPriPrevMouseY = 0;
	mPriOldMousePosX = 0;
	mPriOldMousePosY = 0;

	mPriCam.initMatrices();
	mPriWalkingSpeed = 10.0f;

	c = 0.0;

}

RsRendererImpl::~RsRendererImpl()
{
	// TODO Auto-generated destructor stub
	FTGL::ftglDestroyFont((FTGL::FTGLfont*)font);
	delete font;
}

void RsRendererImpl::display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	applyKeyEvents();
	glLoadIdentity();									// Reset The Current Modelview Matrix
	mPriCam.calcMatrix();
	cgGLEnableProfile(fprof);
	cgGLEnableProfile(vprof);
	cgGLBindProgram(shader);
	glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	glBegin(GL_TRIANGLES);								// Drawing Using Triangles
//		glColor3f(1.0f,0.0f,0.0f);						// Set The Color To Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// Top
//		glColor3f(0.0f,1.0f,0.0f);						// Set The Color To Green
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
//		glColor3f(0.0f,0.0f,1.0f);						// Set The Color To Blue
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
	glEnd();											// Finished Drawing The Triangle

	cgGLUnbindProgram(fprof);
	cgGLUnbindProgram(vprof);
	cgGLDisableProfile(fprof);
	cgGLDisableProfile(vprof);
//	printf("%s\n", cgGetErrorString(cgGetError()));

	glTranslatef(3.0f,0.0f,0.0f);						// Move Right 3 Units
//	glColor3f(0.5f,0.5f,1.0f);							// Set The Color To Blue One Time Only
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);									// Draw A Quad
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
		glTexCoord2f(1.0f,1.0f);
		glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
		glTexCoord2f(1.0f,0.0f);
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
	glEnd();											// Done Drawing The Quad
	glDisable(GL_TEXTURE_2D);



	c+=0.001;
	glGetDoublev(GL_MODELVIEW_MATRIX, mv);
	glGetDoublev(GL_PROJECTION_MATRIX, pr);
	glGetIntegerv(GL_VIEWPORT, vp);
	gluProject(-1.0, 1.0, 0.0, mv, pr,vp, &wp[0], &wp[1], &wp[2]);
	textPoint = FTPoint(wp[0], wp[1], wp[2]);
	font->Render("-1, 1, 0", -1,textPoint);
	gluProject(1.0, 1.0, 0.0, mv, pr,vp, &wp[0], &wp[1], &wp[2]);
	textPoint = FTPoint(wp[0], wp[1], wp[2]);
	font->Render("1, 1, 0", -1,textPoint);
	gluProject(1.0, -1.0, 0.0, mv, pr,vp, &wp[0], &wp[1], &wp[2]);
	textPoint = FTPoint(wp[0], wp[1], wp[2]);
	font->Render("1, -1, 0", -1,textPoint);
	gluProject(-1.0, -1.0, 0.0, mv, pr,vp, &wp[0], &wp[1], &wp[2]);
	textPoint = FTPoint(wp[0], wp[1], wp[2]);
	font->Render("-1, -1, 0", -1,textPoint);

  glutSwapBuffers ( );
  // Swap The Buffers To Not Be Left With A Clear Screen
}

void RsRendererImpl::reshape(int _w, int _h)
{
	glViewport( 0, 0, _w, _h );
	glMatrixMode( GL_PROJECTION );  // Select The Projection Matrix
	glLoadIdentity( );                // Reset The Projection Matrix
	if ( _h==0 )  // Calculate The Aspect Ratio Of The Window
		gluPerspective( 25, ( float ) _w, 0.1f, 3000.0 );
	else
		gluPerspective ( 25, ( float ) _w / ( float ) _h, 0.1f, 3000.0 );
	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
	glLoadIdentity ( );    // Reset The Model View Matrix

}


void RsRendererImpl::keyboard(unsigned char _key, int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	switch ( _key ) {
	case 27:        // When Escape Is Pressed...
//		exit ( 0 );   // Exit The Program
		glutLeaveMainLoop();
		break;        // Ready For Next Case
	case (int)'w':
		mPriStepForward = true;
	break;
	case (int)'s':
		mPriStepBackward = true;
	break;
	case (int)'a':
		mPriStepLeft = true;
	break;
	case (int)'d':
		mPriStepRight = true;
	break;
	case (int)'q':
		mPriRollLeft = true;
	break;
	case (int)'e':
		mPriRollRight = true;
	break;
	case (int)'f':
		glutFullScreenToggle();
	break;
	case (int)'c':
		glutReshapeWindow(256, 256);
	break;
	default:        // Now Wrap It Up
		break;
	}

}

void RsRendererImpl::specialKeys(int _key, int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	switch ( _key ) {
	case GLUT_KEY_PAGE_UP: // tilt up
		mPriTiltUp = true;
		break;
	case GLUT_KEY_PAGE_DOWN: // tilt down
		mPriTiltDown = true;
		break;

	case GLUT_KEY_UP: // walk forward (bob head)
		mPriTurnUp = true;
		break;

	case GLUT_KEY_DOWN: // walk back (bob head)
		mPriTurnDown = true;
		break;

	case GLUT_KEY_LEFT: // look left(int)
		mPriTurnLeft = true;
		break;

	case GLUT_KEY_RIGHT: // look right
		mPriTurnRight = true;
		break;
	default:
		break;
	}
}

void RsRendererImpl::processMouse(int _button, int _state, int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	mPriOldMousePosX = _x;
	mPriOldMousePosY = _y;

	if (_state == GLUT_DOWN){
		if (_button == GLUT_LEFT_BUTTON){
		}
		else if (_button == GLUT_RIGHT_BUTTON){
			mPriOldMousePosX = _x;
			mPriOldMousePosY = _y;
		}
	}
}

void RsRendererImpl::processMouseActiveMotion(int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	// setting the window coordinate-sys to
	/*          +y
	 *          |
	 *     -x---+---+x
	 *          |
	 *          -y
	 */

	mPriCam.setXRot(mPriOldMousePosY - _y);
	mPriCam.setYRot(mPriOldMousePosX - _x);

    mPriOldMousePosX = _x;
    mPriOldMousePosY = _y;
}

void RsRendererImpl::animator(int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	glutPostRedisplay();
}

void RsRendererImpl::applyKeyEvents()
{
	if (mPriStepForward){
		mPriCam.incZMove(0.1*mPriWalkingSpeed);
		mPriStepForward = false;
	}
	if (mPriStepBackward){
		mPriCam.decZMove(0.1*mPriWalkingSpeed);
		mPriStepBackward = false;
	}
	if (mPriStepLeft){
		mPriCam.incXMove(0.01*mPriWalkingSpeed);
		mPriStepLeft = false;
	}
	if (mPriStepRight){
		mPriCam.decXMove(0.01*mPriWalkingSpeed);
		mPriStepRight = false;
	}
	if (mPriRollLeft){
		mPriCam.setZRot(-1.0);
		mPriRollLeft = false;
	}
	if (mPriRollRight){
		mPriCam.setZRot(1.0);
		mPriRollRight = false;
	}
	if (mPriTiltUp){
		mPriCam.decYMove(0.1*mPriWalkingSpeed);
		mPriTiltUp = false;
	}
	if (mPriTiltDown){
		mPriCam.incYMove(0.1*mPriWalkingSpeed);
		mPriTiltDown = false;
	}
	if (mPriTurnLeft){
		mPriCam.setYRot(1.0);
		mPriTurnLeft = false;
	}
	if (mPriTurnRight){
		mPriCam.setYRot(-1.0);
		mPriTurnRight = false;
	}
	if (mPriTurnUp){
		mPriCam.setXRot(1.0);
		mPriTurnUp = false;
	}
	if (mPriTurnDown){
		mPriCam.setXRot(-1.0);
		mPriTurnDown = false;
	}
}

void RsRendererImpl::init()
{

	glewInit();
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable ( GL_COLOR_MATERIAL );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLuint buf;
	glGenBuffers(1, &buf);
	glDeleteBuffers(1, &buf);

	boost::filesystem::path texFile = boost::filesystem::path("/home/ava/crate.tga");
	RsImageTools* iTools = RsImageTools::getSingleton();
	RsTGAimage img;
	iTools->loadTGA(&texFile, &img);
//	iTools->loadTGA("D:\\blender-2.49b-windows\\.blender\\crate2.tga", &img);

//	boost::filesystem::path meshFile = boost::filesystem::path("D:\\blender-2.49b-windows\\.blender\\box.obj");
	boost::filesystem::path meshFile = boost::filesystem::path("/media/ClemensHDD/meshes/bunny.obj");
//	RsMeshTools* mTools = RsMeshTools::getSingleton();
//	mTools->loadObj(&meshFile);

	glGenTextures(1, &mPriTexture);
	glBindTexture(GL_TEXTURE_2D, mPriTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// -------------------------------------------
	std::string code = std::string("void main(in float4 inCol:COLOR0,out float4 color  :   COLOR0)\n {\n color = lerp(float4(1.0,0.0,1.0,1.0), inCol, 0.5);\n }");
	context = cgCreateContext();
	cgSetErrorHandler(cgCompileErrorHandler, 0);
	fprof = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	vprof = cgGLGetLatestProfile(CG_GL_VERTEX);
	CGerror cgError;

	fshader = cgCreateProgram(context, CG_SOURCE, code.c_str(), fprof, "main", 0);
	cgError = cgGetError();
	if (cgError == CG_COMPILER_ERROR){
		cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(fshader, CG_COMPILED_PROGRAM) << "---- PROGRAM END ----" << endl;
		exit(0);
	}
//	cerr << cgGetLastListing(context) << "----" << endl;
//	cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(fshader, CG_COMPILED_PROGRAM) << "---- PROGRAM END ----" << endl;
	vshader = cgCreateProgramFromFile(context, CG_SOURCE, "simpleVert.cg", vprof, "main",0);
	cgError = cgGetError();
	if (cgError == CG_COMPILER_ERROR){
		cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(vshader, CG_PROGRAM_SOURCE) << "---- PROGRAM END ----" << endl;
		exit(0);
	}
//	printf("%s\n", cgGetErrorString(cgGetError()));
//	shader = cgCreateProgramFromFile(context, CG_SOURCE, "simpleFrag.cg", prof, "main",0);
	shader = cgCombinePrograms2(fshader, vshader);
	cgDestroyProgram(fshader);
	cgDestroyProgram(vshader);
	cgGLLoadProgram(shader);
//	printf("%s\n", cgGetErrorString(cgGetError()));
	cgSetErrorHandler(cgErrorHandler, 0);

	// -------------------------------------------

	// Create a pixmap font from a TrueType file.
	font = new FTBitmapFont("/usr/share/fonts/truetype/msttcorefonts/arial.ttf");

	// If something went wrong, bail out.
	if(font->Error())
		exit(0);

	// Set the font size and render a small text.
	font->FaceSize(9);
//	font.Depth(10.0);
//	font->UseDisplayList(true);

}

void RsRendererImpl::debug()
{
	std::cerr << "in ImplClass" << std::endl;
}
