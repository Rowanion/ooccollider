/**
 * @file	RsDissolveRenderer.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	RsDissolveRenderer class declaration.
 */

#ifndef RSDISSOLVERENDERER_H_
#define RSDISSOLVERENDERER_H_

#include "RsAbstractRenderer.h"
#include "RsCamera.h"

#include "GL/glew.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "RsMathTools.h"
#include "RsObjModel.h"
#include "RsVboV4T2.h"
#include "RsFbo.h"

#define GET_GLERROR(ret) \
{ \
         GLenum err = glGetError(); \
         if (err != GL_NO_ERROR) { \
                 fprintf(stderr, "[%s line %d] GL Error: %s\n", \
                 __FILE__, __LINE__, gluErrorString(err)); \
                 fflush(stderr); \
         } \
}

/**
 * @class RsDissolveRenderer
 * @brief This is an exemplary implementation of a renderer which displays a shaded, lit and textured model, which slowly dissolves into the void...
 * This implementation simply loads a mesh-model from an OVJ-file, and uses some cg-shaders to slowly
 * dissolve the model, using perlin noise and glowing.
 */
class RsDissolveRenderer: public RsAbstractRenderer
{
public:
	RsDissolveRenderer();
	virtual ~RsDissolveRenderer();

	/**
	 * @brief Preliminary init-Method - load/generate textures, setup shaders, etc
	 */
	virtual void init();

	/**
	 * @brief The render-loop - this is where the magick happens
	 */
	virtual void display();

	/**
	 * @brief Default glut-method; Called whenever the window is resized.
	 * The aspect-ratio is kept here and a frustum is created.
	 * @param _w window width
	 * @param _h window height
	 */
	virtual void reshape(int _w, int _h);

	/**
	 * @brief default glut-function for regular key events.
	 * @param _key Which key was pressed?
	 * @param _x mouse-x-coordinate relative to window at the time the key was pressed.
	 * @param _y mouse-y-coordinate relative to window at the time the key was pressed.
	 * @param _present Field specifies if this method is actually present in this renderer implementation.
	 */
	virtual void keyboard(unsigned char _key, int _x, int _y, int* _present = 0);

	/**
	 * @brief default glut-function for special key events.
	 * @param _key Which key was pressed?
	 * @param _x mouse-x-coordinate relative to window at the time the key was pressed.
	 * @param _y mouse-y-coordinate relative to window at the time the key was pressed.
	 * @param _present Field specifies if this method is actually present in this renderer implementation.
	 */
	virtual void specialKeys(int _key, int _x, int _y, int* _present = 0);

	/**
	 * @brief default glut-function for mouse input events.
	 * @param _button Which button was pressed?
	 * @param _state Button up or down?
	 * @param _x mouse-x-coordinate relative to window at the time the key was pressed.
	 * @param _y mouse-y-coordinate relative to window at the time the key was pressed.
	 * @param _present Field specifies if this method is actually present in this renderer implementation.
	 */
	virtual void processMouse(int _button, int _state, int _x, int _y, int* _present = 0);

	/**
	 * @brief default glut-function for mouse input events, a.k.a. dragging
	 * @param _x mouse-x-coordinate relative to window at the time the key was pressed.
	 * @param _y mouse-y-coordinate relative to window at the time the key was pressed.
	 * @param _present Field specifies if this method is actually present in this renderer implementation.
	 */
	virtual void processMouseActiveMotion(int _x, int _y, int* _present = 0);

	/**
	 * @brief This baby keeps the animation going. It calls the glutPostRedisplay() over and over and over and over......
	 * @param _present Field specifies if this method is actually present in this renderer implementation.
	 */
	virtual void animator(int* _present = 0);

	/**
	 * @brief Function applies the collected movement changes to the camera-matrix.
	 */
	void applyKeyEvents();
private:

	/**
	 * @brief Loading and creation of texture objects.
	 */
	void setupTextures();

	/**
	 * @brief Loading and setup of used shaders.
	 */
	void setupShaders();

	/**
	 * @brief As the function name suggests, this actually draws a fullscreen quad.
	 */
	void drawFSQuad();

	RsCamera mPriCam;
	float mPriWalkingSpeed;
	GLuint mPriTexture;
	GLuint mPriTexture2;
	GLuint mPriTexture3;
	GLuint mPriTexture4;
	int mPriPrevMouseX;
	int mPriPrevMouseY;
	int mPriOldMousePosX;
	int mPriOldMousePosY;

	bool mPriStepForward;
	bool mPriStepBackward;
	bool mPriStepLeft;
	bool mPriStepRight;
	bool mPriRollLeft;
	bool mPriRollRight;
	bool mPriTiltUp;
	bool mPriTiltDown;
	bool mPriTurnLeft;
	bool mPriTurnRight;
	bool mPriTurnUp;
	bool mPriTurnDown;

	CGparameter mvp;

	CGprogram lerpFrag;
	CGparameter lerpVal;
	CGparameter cgTex1;
	CGparameter cgTex2;
	CGparameter cgNoiseTex;
	CGparameter cgRndTex;

	CGparameter lightTex;
	CGparameter lightNoiseTex;
	CGparameter lightLerp;

	CGparameter lightTex2;
	CGparameter lightNoiseTex2;
	CGparameter lightLerp2;

	float mPriLerp;
	bool mPriUpDir;

	int vp[4];
	GLdouble wp[3];

	double c;

	RsMathTools mPriMath;

	RsObjModel* model;

	RsVboV4T2* mPriFsQuad;
	RsFbo* mPriFBO1;
	RsFbo* mPriFBO2;
	int mPriWidth;
	int mPriHeight;


	CGparameter glowTex1;
	CGparameter glowTex2;
	CGparameter glowTex3;

	CGprogram drawNormalLinesShader;
	CGprogram paintNormalTexShader;

	CGprogram coloredDissolveShader;
	CGprogram lightAndDissolveShader;

	CGprogram glowPass1Shader;
	CGprogram glowPass2Shader;
	CGprogram glowPass3Shader;

	unsigned vboIdx;
	bool renderFull;
	bool drawNormals;

	boost::filesystem::path meshFile;
};

#endif /* RSDISSOLVERENDERER_H_ */
