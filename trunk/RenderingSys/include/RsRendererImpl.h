/*
 * RsRendererImpl.h
 *
 *  Created on: 12.03.2010
 *      Author: TheAvatar
 */

#ifndef RSRENDERERIMPL_H_
#define RSRENDERERIMPL_H_

#include "RsAbstractRenderer.h"
#include "OOCCamera.h"

#include "GL/glew.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"

#include <FTGL/ftgl.h>

#include "RsMathTools.h"

#define GET_GLERROR(ret) \
{ \
         GLenum err = glGetError(); \
         if (err != GL_NO_ERROR) { \
                 fprintf(stderr, "[%s line %d] GL Error: %s\n", \
                 __FILE__, __LINE__, gluErrorString(err)); \
                 fflush(stderr); \
         } \
}

class RsRendererImpl: public RsAbstractRenderer
{
public:
	RsRendererImpl();
	virtual ~RsRendererImpl();
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
	OOCCamera mPriCam;
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
	CGprofile vprof;
	CGprofile fprof;
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

	float mPriLerp;
	bool mPriUpDir;

	FTPoint textPoint;
	FTBitmapFont* font;
	double mv[16];
	double pr[16];
	int vp[4];
	GLdouble wp[3];

	double c;

	RsMathTools mPriMath;
};

#endif /* RSRENDERERIMPL_H_ */
