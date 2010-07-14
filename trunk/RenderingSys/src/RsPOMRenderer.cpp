/**
 * @file	RsPOMRendererImpl.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	RsPOMRendererImpl class definition.
 */


#include "RsPOMRenderer.h"

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
#include "RsWindow.h"

using namespace std;

RsPOMRenderer::RsPOMRenderer()
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
	mPriWalkingSpeed = 1.0f;

	c = 0.0;

	mPriLerp = 0.0;
	mPriUpDir = true;

}

RsPOMRenderer::~RsPOMRenderer()
{
	// TODO Auto-generated destructor stub
	delete mPriFsQuad;
}

void RsPOMRenderer::display()
{

	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	applyKeyEvents();
	glLoadIdentity();									// Reset The Current Modelview Matrix
	mPriCam.calcMatrix();

	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	printf("%s\n", cgGetErrorString(cgGetError()));


	glTranslatef(0.0f, 0.0f, -10.0f);						// Move Right 3 Units

	//glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(315.0f, 0.0f, 1.0f, 0.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriTexture);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mPriTexture2);
	glEnable(GL_TEXTURE_2D);
	cgGLEnableProfile(fprof);
	cgGLBindProgram(pom1Frag);
	glColor3f(1,0,0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(0.0f,0,0.5f);
		glTexCoord2f(0.0f,3.0f);
		glVertex3f(0.0f,1.0f,0.5f);
		glTexCoord2f(3.0f,3.0f);
		glVertex3f(1.0f,1.0f,0.5f);
		glTexCoord2f(3.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.5f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,0);
	cgGLUnbindProgram(fprof);
	cgGLDisableProfile(fprof);



	// ---------------------------
  glutSwapBuffers ( );
  // Swap The Buffers To Not Be Left With A Clear Screen

}

void RsPOMRenderer::reshape(int _w, int _h)
{
	this->mPriWidth = _w;
	this->mPriHeight = _h;
	glViewport( 0, 0, _w, _h );
	glMatrixMode( GL_PROJECTION );  // Select The Projection Matrix
	glLoadIdentity( );                // Reset The Projection Matrix
	if ( _h==0 )  // Calculate The Aspect Ratio Of The Window
		gluPerspective( 25, ( float ) _w, 0.1f, 3000.0 );
	else
		gluPerspective ( 25, ( float ) _w / ( float ) _h, 0.1f, 3000.0 );
	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
	glLoadIdentity ( );    // Reset The Model View Matrix

	// resizing the FBOs as well - which means to destroy them an recreate them with appropiate size.
	//Advice: don't do a lot of resizing! It eats raw fps for breakfast!
	this->mPriFBO1->setSize(_w, _h);
	this->mPriFBO2->setSize(_w, _h);
}


void RsPOMRenderer::keyboard(unsigned char _key, int _x, int _y, int* _present)
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

void RsPOMRenderer::specialKeys(int _key, int _x, int _y, int* _present)
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

void RsPOMRenderer::processMouse(int _button, int _state, int _x, int _y, int* _present)
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

void RsPOMRenderer::processMouseActiveMotion(int _x, int _y, int* _present)
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

void RsPOMRenderer::animator(int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	glutPostRedisplay();
}

void RsPOMRenderer::applyKeyEvents()
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

void RsPOMRenderer::init()
{
	std::cerr << "1" << std::endl;

	glewInit();
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable ( GL_COLOR_MATERIAL );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	std::cerr << "2" << std::endl;

	unsigned indices[4] = {0,1,2,3};
	RsV4T2 data[4] = {RsV4T2(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f), RsV4T2(1.0f, -1.0f, 0.0f, 1.0f ,0.0f), RsV4T2(1.0f, 1.0f, 0.0f, 1.0f, 1.0f), RsV4T2(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f)};
//	RsV4T2 data[4] = {RsV4T2(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f), RsV4T2(1.0f, -1.0f, -1.0f, 1.0f, 0.0f), RsV4T2(1.0f, 1.0f, -1.0f, 1.0f, 1.0f), RsV4T2(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f)};

	mPriFsQuad = new RsVboV4T2(4, indices, 4, data, GL_QUADS);

	boost::filesystem::path texFile = boost::filesystem::path("images/wall3_combined.tga");
	boost::filesystem::path texFile2 = boost::filesystem::path("images/wall3_normal.tga");
	RsImageTools* iTools = RsImageTools::getSingleton();
	RsTGAimage img;
	iTools->loadTGA(&texFile, &img);
//	iTools->loadTGA("D:\\blender-2.49b-windows\\.blender\\crate2.tga", &img);

	glGenTextures(1, &mPriTexture);
	glBindTexture(GL_TEXTURE_2D, mPriTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	gluBuild2DMipmaps( GL_TEXTURE_2D, img.intlFormat, img.width, img.height, img.format, GL_UNSIGNED_BYTE, img.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	iTools->loadTGA(&texFile2, &img);
	glGenTextures(1, &mPriTexture2);
	glBindTexture(GL_TEXTURE_2D, mPriTexture2);
//	glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	gluBuild2DMipmaps( GL_TEXTURE_2D, img.intlFormat, img.width, img.height, img.format, GL_UNSIGNED_BYTE, img.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
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
	std::cerr << "6" << std::endl;

	fshader = cgCreateProgram(context, CG_SOURCE, code.c_str(), fprof, "main", 0);
	cgError = cgGetError();
	if (cgError == CG_COMPILER_ERROR){
		cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(fshader, CG_COMPILED_PROGRAM) << "---- PROGRAM END ----" << endl;
		exit(0);
	}
//	cerr << cgGetLastListing(context) << "----" << endl;
//	cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(fshader, CG_COMPILED_PROGRAM) << "---- PROGRAM END ----" << endl;
	vshader = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/simpleVert.cg", vprof, "main",0);
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
//	cgSetErrorHandler(cgErrorHandler, 0);
	std::cerr << "7" << std::endl;

	// -------------------------------------------
	vpLight = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/vp_phong.cg", vprof, "main",0);
	cgGLLoadProgram(vpLight);
	fpLight = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_OnlyDiss.cg", fprof, "main",0);
	cgGLLoadProgram(fpLight);
	fpLight2 = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_phong.cg", fprof, "main",0);
	cgGLLoadProgram(fpLight2);
	cgError = cgGetError();
	if (cgError == CG_COMPILER_ERROR){
		cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(vshader, CG_PROGRAM_SOURCE) << "---- PROGRAM END ----" << endl;
		exit(0);
	}
	std::cerr << "8" << std::endl;

	pom1Frag = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_pom1.cg", fprof, "main",0);
	cgGLLoadProgram(pom1Frag);
	cgTex1 = cgGetNamedParameter(pom1Frag, "tex1");
	cgTex2 = cgGetNamedParameter(pom1Frag, "tex2");
	cgGLSetTextureParameter(cgTex1, mPriTexture);
	cgGLEnableTextureParameter(cgTex1);
	cgGLSetTextureParameter(cgTex2, mPriTexture2);
	cgGLEnableTextureParameter(cgTex2);

	lightTex = cgGetNamedParameter(fpLight, "tex1");
	lightNoiseTex = cgGetNamedParameter(fpLight, "tex2");
	lightLerp = cgGetNamedParameter(fpLight, "lerpVal");
	cgGLSetTextureParameter(lightTex, mPriTexture2);
	cgGLEnableTextureParameter(lightTex);
	cgGLSetTextureParameter(lightNoiseTex, mPriTexture4);
	cgGLEnableTextureParameter(lightNoiseTex);

	lightTex2 = cgGetNamedParameter(fpLight2, "tex1");
	lightNoiseTex2 = cgGetNamedParameter(fpLight2, "tex2");
	lightLerp2 = cgGetNamedParameter(fpLight2, "lerpVal");
	cgGLSetTextureParameter(lightTex2, mPriTexture2);
	cgGLEnableTextureParameter(lightTex2);
	cgGLSetTextureParameter(lightNoiseTex2, mPriTexture4);
	cgGLEnableTextureParameter(lightNoiseTex2);

	vpGlow = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_glow.cg", vprof, "vpPassThrough",0);
	cgGLLoadProgram(vpGlow);
	std::cerr << "12" << std::endl;

	fpGlow1 = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_glow.cg", fprof, "fpGaussianPassH",0);
	cgGLLoadProgram(fpGlow1);
	glowTex1 = cgGetNamedParameter(fpGlow1, "srcSampler");
	cgGLEnableTextureParameter(glowTex1);
	std::cerr << "13" << std::endl;

	fpGlow2 = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_glow.cg", fprof, "fpGaussianPassV",0);
	cgGLLoadProgram(fpGlow2);
	glowTex2 = cgGetNamedParameter(fpGlow2, "srcSampler");
	cgGLEnableTextureParameter(glowTex2);
	std::cerr << "14" << std::endl;

	fpGlow3 = cgCreateProgramFromFile(context, CG_SOURCE, "shaders/fp_glow.cg", fprof, "fpFinalCompositing",0);
	cgGLLoadProgram(fpGlow3);
	glowTex3 = cgGetNamedParameter(fpGlow3, "tempSampler");
	cgGLEnableTextureParameter(glowTex3);
	std::cerr << "15" << std::endl;

	cgError = cgGetError();
	if (cgError == CG_COMPILER_ERROR){
		cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(fpGlow3, CG_PROGRAM_SOURCE) << "---- PROGRAM END ----" << endl;
		cgGetLastListing(context);
		exit(0);
	}
	std::cerr << "16" << std::endl;

	mPriFBO1 = new RsFbo(RsWindow::getSingleton()->getWindowWidth(), RsWindow::getSingleton()->getWindowHeight());
	mPriFBO1->createAndAddDepthBuf();
	mPriFBO1->createAndAddColorTex();

	mPriFBO2 = new RsFbo(RsWindow::getSingleton()->getWindowWidth(), RsWindow::getSingleton()->getWindowHeight());
	mPriFBO2->createAndAddDepthBuf();
	mPriFBO2->createAndAddColorTex();
}

void RsPOMRenderer::debug()
{
	std::cerr << "in ImplClass" << std::endl;
}

void RsPOMRenderer::drawFSQuad()
{
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode (GL_PROJECTION);
			glPushMatrix ();
				glLoadIdentity ();
				mPriFsQuad->draw();
			glPopMatrix ();
			glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}
