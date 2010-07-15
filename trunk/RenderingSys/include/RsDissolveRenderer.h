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
 * @class RsRendererImpl
 * @brief: This is an exemplary implementation of a renderer.
 * This implementation simply loads a mesh-model from an OVJ-file, and uses some cg-shaders to slowly
 * dissolve the model, using perlin noise and glowing.
 */
class RsDissolveRenderer: public RsAbstractRenderer
{
public:
	RsDissolveRenderer();
	virtual ~RsDissolveRenderer();
	virtual void init();
	virtual void display();
	virtual void reshape(int _w, int _h);
	virtual void keyboard(unsigned char _key, int _x, int _y, int* _present = 0);
	virtual void specialKeys(int _key, int _x, int _y, int* _present = 0);
	virtual void processMouse(int _button, int _state, int _x, int _y, int* _present = 0);
	virtual void processMouseActiveMotion(int _x, int _y, int* _present = 0);
	virtual void animator(int* _present = 0);
	virtual void debug();
	void applyKeyEvents();
private:
	void setupTextures();
	void setupShaders();
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

	CGcontext context;
	CGprofile fprof;
	CGprofile gprof;
	CGprofile vprof;
	CGprogram fshader;
	CGprogram vshader;
	CGprogram shader;
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

	void drawFSQuad();

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
};

#endif /* RSDISSOLVERENDERER_H_ */
