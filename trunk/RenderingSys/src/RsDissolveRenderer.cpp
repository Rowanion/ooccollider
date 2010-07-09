/**
 * @file	RsDissolveRenderer.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	RsDissolveRenderer class definition.
 */


#include "RsDissolveRenderer.h"

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

RsDissolveRenderer::RsDissolveRenderer()
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

RsDissolveRenderer::~RsDissolveRenderer()
{
	// TODO Auto-generated destructor stub
	delete mPriFsQuad;
}

void RsDissolveRenderer::display()
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
	glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(315.0f, 0.0f, 1.0f, 0.0f);


	c+=0.001;

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	cgGLSetParameter1f(lightLerp2, mPriLerp);
	cgGLEnableProfile(fprof);
	cgGLEnableProfile(vprof);
	cgGLBindProgram(vpLight);
	cgGLBindProgram(fpLight2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriTexture2);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mPriTexture4);
	glEnable(GL_TEXTURE_3D);

	model->draw();
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT, GL_FILL);
	cgGLUnbindProgram(fprof);
	cgGLUnbindProgram(vprof);
	cgGLDisableProfile(fprof);
	cgGLDisableProfile(vprof);

	// only dissolve
	mPriFBO1->bind();
	glClearColor(0.0,0.0,0.0,0.0);
	mPriFBO1->clear();
	cgGLSetParameter1f(lightLerp, mPriLerp);
	cgGLEnableProfile(fprof);
	cgGLEnableProfile(vprof);
	cgGLBindProgram(vpLight);
	cgGLBindProgram(fpLight);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriTexture2);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, mPriTexture4);
	glEnable(GL_TEXTURE_3D);

	model->draw();
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT, GL_FILL);
	cgGLUnbindProgram(fprof);
	cgGLUnbindProgram(vprof);
	cgGLDisableProfile(fprof);
	cgGLDisableProfile(vprof);
	mPriFBO1->unbind();

	mPriFBO2->bind();
	cgGLSetTextureParameter(glowTex1, mPriFBO1->getColorTexId());
//	cgGLEnableProfile(vprof);
	cgGLEnableProfile(fprof);
	cgGLBindProgram(fpGlow1);
//	cgGLBindProgram(vpGlow);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriFBO1->getColorTexId());
	glEnable(GL_TEXTURE_2D);
	this->drawFSQuad();
	glDisable(GL_TEXTURE_2D);
	cgGLUnbindProgram(fprof);
	cgGLDisableProfile(fprof);
//	cgGLUnbindProgram(vprof);
//	cgGLDisableProfile(vprof);
	mPriFBO2->unbind();

	mPriFBO1->bind();
//	mPriFBO1->clear();
	cgGLSetTextureParameter(glowTex2, mPriFBO2->getColorTexId());
	cgGLEnableProfile(fprof);
	cgGLBindProgram(fpGlow2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriFBO2->getColorTexId());
	glEnable(GL_TEXTURE_2D);
	this->drawFSQuad();
	glDisable(GL_TEXTURE_2D);
	cgGLUnbindProgram(fprof);
	cgGLDisableProfile(fprof);
	mPriFBO1->unbind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE);
	cgGLSetTextureParameter(glowTex3, mPriFBO1->getColorTexId());
	cgGLEnableProfile(fprof);
	cgGLBindProgram(fpGlow3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriFBO1->getColorTexId());
	glEnable(GL_TEXTURE_2D);
	this->drawFSQuad();
	glDisable(GL_TEXTURE_2D);
	cgGLUnbindProgram(fprof);
	cgGLDisableProfile(fprof);
	glDisable(GL_BLEND);


	// ---------------------------
  glutSwapBuffers ( );
  // Swap The Buffers To Not Be Left With A Clear Screen

  if (mPriUpDir){
	  mPriLerp+=0.001f;
  }
  else{
	  mPriLerp -= 0.001f;
  }
  if (mPriLerp> 1.30f && mPriUpDir){
//	  mPriLerp = -0.1f;
	  mPriUpDir = false;
  }
  else if (mPriLerp < -0.31f && !mPriUpDir){
//	  mPriLerp = 0.0f;
	  mPriUpDir = true;
  }
}

void RsDissolveRenderer::reshape(int _w, int _h)
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


void RsDissolveRenderer::keyboard(unsigned char _key, int _x, int _y, int* _present)
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

void RsDissolveRenderer::specialKeys(int _key, int _x, int _y, int* _present)
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

void RsDissolveRenderer::processMouse(int _button, int _state, int _x, int _y, int* _present)
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

void RsDissolveRenderer::processMouseActiveMotion(int _x, int _y, int* _present)
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

void RsDissolveRenderer::animator(int* _present)
{
	if (_present != 0){
		*_present = 1;
		return;
	}
	glutPostRedisplay();
}

void RsDissolveRenderer::applyKeyEvents()
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

void RsDissolveRenderer::init()
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

	boost::filesystem::path texFile = boost::filesystem::path("plasma.tga");
	boost::filesystem::path texFile2 = boost::filesystem::path("plasma2.tga");
	boost::filesystem::path texFile3 = boost::filesystem::path("noise.tga");
	RsImageTools* iTools = RsImageTools::getSingleton();
	RsTGAimage img;
	iTools->loadTGA(&texFile, &img);
//	iTools->loadTGA("D:\\blender-2.49b-windows\\.blender\\crate2.tga", &img);
#ifdef _WIN32
	boost::filesystem::path meshFile = boost::filesystem::path("D:\\meshes\\mini_obj2.obj");
#elif defined OFFICE
	boost::filesystem::path meshFile = boost::filesystem::path("/home/ava/Diplom/Model/meshes/mini_obj2.obj");
#else
	boost::filesystem::path meshFile = boost::filesystem::path("/media/ClemensHDD/meshes/mini_obj2.obj");
#endif
	RsMeshTools* mTools = RsMeshTools::getSingleton();
	model = mTools->loadObj(&meshFile);
	std::cerr << "3" << std::endl;

	glGenTextures(1, &mPriTexture);
	glBindTexture(GL_TEXTURE_2D, mPriTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	std::cerr << "4" << std::endl;

	iTools->loadTGA(&texFile2, &img);
	glGenTextures(1, &mPriTexture2);
	glBindTexture(GL_TEXTURE_2D, mPriTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	iTools->loadTGA(&texFile3, &img);
	glGenTextures(1, &mPriTexture3);
	glBindTexture(GL_TEXTURE_2D, mPriTexture3);
	glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	std::cerr << "5" << std::endl;

	mPriMath = RsMathTools();
	unsigned char* rTex = mPriMath.generateRandomTexture(128*128,1,false);
	float* rfTex = mPriMath.generatePerlinTexture(128, 128, 128, false);
//	for (unsigned i=0; i<128*128; ++i){
//		cerr << rfTex[i] << endl;
//	}
//	exit(0);
	//TODO create vbo with multitexturing and derive unit-tex-coords generically
	glGenTextures(1, &mPriTexture4);
	glBindTexture(GL_TEXTURE_3D, mPriTexture4);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA16, 128, 128, 128, 0, GL_ALPHA, GL_FLOAT, rfTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	delete[] rTex;
	delete[] rfTex;
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
//	cgSetErrorHandler(cgErrorHandler, 0);
	std::cerr << "7" << std::endl;

	// -------------------------------------------
	vpLight = cgCreateProgramFromFile(context, CG_SOURCE, "vp_phong.cg", vprof, "main",0);
	cgGLLoadProgram(vpLight);
	fpLight = cgCreateProgramFromFile(context, CG_SOURCE, "fp_OnlyDiss.cg", fprof, "main",0);
	cgGLLoadProgram(fpLight);
	fpLight2 = cgCreateProgramFromFile(context, CG_SOURCE, "fp_phong.cg", fprof, "main",0);
	cgGLLoadProgram(fpLight2);
	cgError = cgGetError();
	if (cgError == CG_COMPILER_ERROR){
		cerr << "---- PROGRAM BEGIN ----" << endl << cgGetProgramString(vshader, CG_PROGRAM_SOURCE) << "---- PROGRAM END ----" << endl;
		exit(0);
	}
	std::cerr << "8" << std::endl;

	lerpFrag = cgCreateProgramFromFile(context, CG_SOURCE, "lerpFrag.cg", fprof, "main",0);
	cgGLLoadProgram(lerpFrag);
	cgTex1 = cgGetNamedParameter(lerpFrag, "tex1");
	cgTex2 = cgGetNamedParameter(lerpFrag, "tex2");
	cgNoiseTex = cgGetNamedParameter(lerpFrag, "noiseTex");
	cgRndTex = cgGetNamedParameter(lerpFrag, "rndTex");
	lerpVal = cgGetNamedParameter(lerpFrag, "lerpVal");
	std::cerr << "9" << std::endl;

	cgGLSetTextureParameter(cgTex1, mPriTexture);
	cgGLEnableTextureParameter(cgTex1);
	cgGLSetTextureParameter(cgTex2, mPriTexture2);
	cgGLEnableTextureParameter(cgTex2);
	cgGLSetTextureParameter(cgNoiseTex, mPriTexture3);
	cgGLEnableTextureParameter(cgNoiseTex);
//	cgGLSetTextureParameter(cgRndTex, mPriTexture4);
//	cgGLEnableTextureParameter(cgRndTex);
	std::cerr << "10" << std::endl;

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

	vpGlow = cgCreateProgramFromFile(context, CG_SOURCE, "fp_glow.cg", vprof, "vpPassThrough",0);
	cgGLLoadProgram(vpGlow);
	std::cerr << "12" << std::endl;

	fpGlow1 = cgCreateProgramFromFile(context, CG_SOURCE, "fp_glow.cg", fprof, "fpGaussianPassH",0);
	cgGLLoadProgram(fpGlow1);
	glowTex1 = cgGetNamedParameter(fpGlow1, "srcSampler");
	cgGLEnableTextureParameter(glowTex1);
	std::cerr << "13" << std::endl;

	fpGlow2 = cgCreateProgramFromFile(context, CG_SOURCE, "fp_glow.cg", fprof, "fpGaussianPassV",0);
	cgGLLoadProgram(fpGlow2);
	glowTex2 = cgGetNamedParameter(fpGlow2, "srcSampler");
	cgGLEnableTextureParameter(glowTex2);
	std::cerr << "14" << std::endl;

	fpGlow3 = cgCreateProgramFromFile(context, CG_SOURCE, "fp_glow.cg", fprof, "fpFinalCompositing",0);
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

void RsDissolveRenderer::debug()
{
	std::cerr << "in ImplClass" << std::endl;
}

void RsDissolveRenderer::drawFSQuad()
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
