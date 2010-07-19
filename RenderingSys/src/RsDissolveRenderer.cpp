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

#include <cstdlib>
#include <iostream>

#include "RsStructs.h"
#include "RsImageTools.h"
#include "RsMeshTools.h"
#include "RsWindow.h"
#include "RsCGShaderBuilder.h"

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
	vboIdx = 0;
	renderFull = true;
	drawNormals = false;
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


	// ----------------------
//	if (renderFull){
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
//		RsCGShaderBuilder::getSingleton()->EnableShader(paintNormalTexShader);
//		model->draw();
//		RsCGShaderBuilder::getSingleton()->DisableShader(paintNormalTexShader);
		if (drawNormals){
			RsCGShaderBuilder::getSingleton()->EnableShader(drawNormalLinesShader);
			model->draw();
			RsCGShaderBuilder::getSingleton()->DisableShader(drawNormalLinesShader);
		}
//	}
//	else {
//		glPolygonMode(GL_FRONT, GL_FILL);
//		glPolygonMode(GL_BACK, GL_LINE);
//		RsCGShaderBuilder::getSingleton()->EnableShader(paintNormalTexShader);
//		model->draw(vboIdx);
//		RsCGShaderBuilder::getSingleton()->DisableShader(paintNormalTexShader);
//		if (drawNormals){
//			RsCGShaderBuilder::getSingleton()->EnableShader(drawNormalLinesShader);
//			model->draw(vboIdx);
//			RsCGShaderBuilder::getSingleton()->DisableShader(drawNormalLinesShader);
//		}
//	}

//
//
//	RsCGShaderBuilder::getSingleton()->EnableShader(drawNormalLinesShader);
//	model->draw();
//	RsCGShaderBuilder::getSingleton()->DisableShader(drawNormalLinesShader);
	// ----------------------


//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	cgGLSetParameter1f(lightLerp2, mPriLerp);
	RsCGShaderBuilder::getSingleton()->EnableShader(lightAndDissolveShader);
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
	RsCGShaderBuilder::getSingleton()->DisableShader(lightAndDissolveShader);
//
	// only dissolve
	mPriFBO1->bind();
	glClearColor(0.0,0.0,0.0,0.0);
	mPriFBO1->clear();
	cgGLSetParameter1f(lightLerp, mPriLerp);
	RsCGShaderBuilder::getSingleton()->EnableShader(coloredDissolveShader);
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
	RsCGShaderBuilder::getSingleton()->DisableShader(coloredDissolveShader);
	mPriFBO1->unbind();
//
	mPriFBO2->bind();
	cgGLSetTextureParameter(glowTex1, mPriFBO1->getColorTexId());
	RsCGShaderBuilder::getSingleton()->EnableShader(glowPass1Shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriFBO1->getColorTexId());
	glEnable(GL_TEXTURE_2D);
	this->drawFSQuad();
	glDisable(GL_TEXTURE_2D);
	RsCGShaderBuilder::getSingleton()->DisableShader(glowPass1Shader);
	mPriFBO2->unbind();
//
	mPriFBO1->bind();
//////	mPriFBO1->clear();
	cgGLSetTextureParameter(glowTex2, mPriFBO2->getColorTexId());
	RsCGShaderBuilder::getSingleton()->EnableShader(glowPass2Shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriFBO2->getColorTexId());
	glEnable(GL_TEXTURE_2D);
	this->drawFSQuad();
	glDisable(GL_TEXTURE_2D);
	RsCGShaderBuilder::getSingleton()->DisableShader(glowPass2Shader);
	mPriFBO1->unbind();
//
	glEnable(GL_BLEND);
	cgGLSetTextureParameter(glowTex3, mPriFBO1->getColorTexId());
	RsCGShaderBuilder::getSingleton()->EnableShader(glowPass3Shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriFBO1->getColorTexId());
	glEnable(GL_TEXTURE_2D);
	this->drawFSQuad();
	glDisable(GL_TEXTURE_2D);
	RsCGShaderBuilder::getSingleton()->DisableShader(glowPass3Shader);
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
	case (int)' ':
		vboIdx++;
		vboIdx %= model->getVboCount();
		std::cerr << "rendering VBO " << vboIdx << " / " << model->getVboCount() << std::endl;
	break;
	case (int)'l':
		renderFull = !renderFull;
		if (!renderFull){
			std::cerr << "rendering VBO " << vboIdx << " / " << model->getVboCount() << std::endl;
		}
	break;
	case (int)'n':
		drawNormals = !drawNormals;
	break;
	case (int)'r':
		delete model;
		model = 0;
		model = RsMeshTools::getSingleton()->loadObj(&meshFile);
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

	// GL init stuff
	glewInit();
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable ( GL_COLOR_MATERIAL );
	glBlendFunc(GL_SRC_ALPHA , GL_ONE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	std::cerr << "2" << std::endl;

	unsigned indices[4] = {0,1,2,3};
	RsV4T2 data[4] = {RsV4T2(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f), RsV4T2(1.0f, -1.0f, 0.0f, 1.0f ,0.0f), RsV4T2(1.0f, 1.0f, 0.0f, 1.0f, 1.0f), RsV4T2(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f)};
//	RsV4T2 data[4] = {RsV4T2(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f), RsV4T2(1.0f, -1.0f, -1.0f, 1.0f, 0.0f), RsV4T2(1.0f, 1.0f, -1.0f, 1.0f, 1.0f), RsV4T2(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f)};

	mPriFsQuad = new RsVboV4T2(4, indices, 4, data, GL_QUADS);

#ifdef _WIN32
	meshFile = boost::filesystem::path("D:\\meshes\\mini_obj2.obj");
#elif defined OFFICE
	meshFile = boost::filesystem::path("/home/ava/Diplom/Model/meshes/mini_obj3.obj");
#else
	meshFile = boost::filesystem::path("/home/ava/Diplom/Model/meshes/mini_obj3.obj");
#endif
	RsMeshTools* mTools = RsMeshTools::getSingleton();
	model = mTools->loadObj(&meshFile);

	mPriMath = RsMathTools();

	this->setupTextures();

	this->setupShaders();

	RsCGShaderBuilder::getSingleton()->PrintShadingLanguageString();
	// create and setup FBOs

	mPriFBO1 = new RsFbo(RsWindow::getSingleton()->getWindowWidth(), RsWindow::getSingleton()->getWindowHeight());
	mPriFBO1->createAndAddDepthBuf();
	mPriFBO1->createAndAddColorTex();

	mPriFBO2 = new RsFbo(RsWindow::getSingleton()->getWindowWidth(), RsWindow::getSingleton()->getWindowHeight());
	mPriFBO2->createAndAddDepthBuf();
	mPriFBO2->createAndAddColorTex();

}

void RsDissolveRenderer::setupTextures()
{
	boost::filesystem::path texFile = boost::filesystem::path("images/plasma.tga");
	boost::filesystem::path texFile2 = boost::filesystem::path("images/plasma2.tga");
	boost::filesystem::path texFile3 = boost::filesystem::path("images/noise.tga");
	RsImageTools* iTools = RsImageTools::getSingleton();
	RsTGAimage img;
	iTools->loadTGA(&texFile, &img);
//	iTools->loadTGA("D:\\blender-2.49b-windows\\.blender\\crate2.tga", &img);

	glGenTextures(1, &mPriTexture);
	glBindTexture(GL_TEXTURE_2D, mPriTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, img.intlFormat, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, img.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

}

void RsDissolveRenderer::setupShaders()
{

	lerpFrag  = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/lerpFrag.cg"), "main", RsCGShaderBuilder::FragmentShader);
	cgTex1 = cgGetNamedParameter(lerpFrag, "tex1");
	cgTex2 = cgGetNamedParameter(lerpFrag, "tex2");
	cgNoiseTex = cgGetNamedParameter(lerpFrag, "noiseTex");
	cgRndTex = cgGetNamedParameter(lerpFrag, "rndTex");
	lerpVal = cgGetNamedParameter(lerpFrag, "lerpVal");

	cgGLSetTextureParameter(cgTex1, mPriTexture);
	cgGLEnableTextureParameter(cgTex1);
	cgGLSetTextureParameter(cgTex2, mPriTexture2);
	cgGLEnableTextureParameter(cgTex2);
	cgGLSetTextureParameter(cgNoiseTex, mPriTexture3);
	cgGLEnableTextureParameter(cgNoiseTex);
//	cgGLSetTextureParameter(cgRndTex, mPriTexture4);
//	cgGLEnableTextureParameter(cgRndTex);
	// --------------------------------------------
	glowPass1Shader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/fp_glow.cg"), "fpGaussianPassH", RsCGShaderBuilder::FragmentShader);
	glowTex1 = cgGetNamedParameter(glowPass1Shader, "srcSampler");
	cgGLEnableTextureParameter(glowTex1);
	// --------------------------------------------
	glowPass2Shader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/fp_glow.cg"), "fpGaussianPassV", RsCGShaderBuilder::FragmentShader);
	glowTex2 = cgGetNamedParameter(glowPass2Shader, "srcSampler");
	cgGLEnableTextureParameter(glowTex2);
	// --------------------------------------------
	glowPass3Shader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/fp_glow.cg"), "fpFinalCompositing", RsCGShaderBuilder::FragmentShader);
	glowTex3 = cgGetNamedParameter(glowPass3Shader, "tempSampler");
	cgGLEnableTextureParameter(glowTex3);
	// --------------------------------------------
	lightAndDissolveShader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/vp_phong.cg"), "main",
			std::string("shaders/fp_phong.cg"), "main");
	lightTex2 = cgGetNamedParameter(lightAndDissolveShader, "tex1");
	lightNoiseTex2 = cgGetNamedParameter(lightAndDissolveShader, "tex2");
	lightLerp2 = cgGetNamedParameter(lightAndDissolveShader, "lerpVal");
	cgGLSetTextureParameter(lightTex2, mPriTexture2);
	cgGLEnableTextureParameter(lightTex2);
	cgGLSetTextureParameter(lightNoiseTex2, mPriTexture4);
	cgGLEnableTextureParameter(lightNoiseTex2);

	// ---------------------------------------------
	coloredDissolveShader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/vp_phong.cg"), "main",
			std::string("shaders/fp_OnlyDiss.cg"), "main");
	lightTex = cgGetNamedParameter(coloredDissolveShader, "tex1");
	lightNoiseTex = cgGetNamedParameter(coloredDissolveShader, "tex2");
	lightLerp = cgGetNamedParameter(coloredDissolveShader, "lerpVal");
	cgGLSetTextureParameter(lightTex, mPriTexture2);
	cgGLEnableTextureParameter(lightTex);
	cgGLSetTextureParameter(lightNoiseTex, mPriTexture4);
	cgGLEnableTextureParameter(lightNoiseTex);

	// ------------------
	drawNormalLinesShader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/drawNormals.cg"), "VPpassThrough", "GPshowNormals", 0);
	paintNormalTexShader = RsCGShaderBuilder::getSingleton()->BuildShaderFromFile(std::string("shaders/drawNormals.cg"), "VPfixed", 0, "FPnormal2Color");

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
