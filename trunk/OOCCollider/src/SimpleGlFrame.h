/**
 * @file	SimpleGlFrame.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	SimpleGlFrame class declaration.
 */

#ifndef SIMPLEGLFRAME_H_
#define SIMPLEGLFRAME_H_

#include "MpiControl.h"
#include "GL/glew.h"
#include "GL/glfw.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <vector>

#include "declarations.h"
#include "AbstractGlFrame.h"
#include "VboManager.h"
#include "IndexedVbo.h"
#include "Camera.h"
#include "Quaternion.h"
#include "IndexedVbo.h"
#include "AbstractEventListener.h"
#include "Fbo.h"


/**
 * @class SimpleGlFrame
 * @brief GlFrame implementation for the RenderingNode.
 */
class SimpleGlFrame : public AbstractGlFrame, oocframework::AbstractEventListener {
public:
	SimpleGlFrame();
	virtual ~SimpleGlFrame();
	virtual void init();
	virtual void display();
	virtual void reshape(int width, int height);
	inline float getFrames() const {
		return avgFps;
	}
	;
	virtual void initMatrices();
	virtual void calcMatrix();
	virtual void multMatrix(float *m1, float *m2, float *res);

	inline const float* getMvMatrix() const {return mPriModelViewMatrix;};
	void notify(oocframework::IEvent& event);


protected:
	void normalizeFrustum();

	unsigned width, height;
	float nearPlane, farPlane;
	GLdouble worldTopLine, worldBottomLine;
	GLdouble worldLeftLine, worldRightLine;
	GLdouble screenXMax, screenYMax, screenYMin;
	GLdouble screenXMaxH, screenYMaxH, screenYMinH;

private:
	float scale;
	float fps[10];
	float avgFps;
	double time;
	unsigned frame;
	ooctools::VboManager* mPriVboMan;
	ooctools::CgToolkit* mPriCgt;

	float mPriModelViewMatrix[16];
	float mPriSavedPositions[10][16];

	float myGlobalAmbient[3]; /* Dim */
	float myLightColor[3];  /* White */

	ooctools::IndexedVbo* mPriIVbo;
	ooctools::Camera camObj;
	float xmove, ymove, zmove;

	float myXRot;
	float myYRot;
	float myZRot;

	int oldPosX;
	int oldPosY;
	float *myTranslateMatrix;
	float *myGLRotMatrix;
	bool first;
	float *myQuatRotMat;
	float *myTempMat;

	float walkingSpeed;

	int mPriButtonActions[3];

	ooctools::Fbo* mPriFbo;

	GLubyte *mPriColorBuffer;

	int mPriCBufWidth;
	int mPriCBufHeight;
	int mPriCBufX;
	int mPriCBufY;

	bool mPriUseSpaceNav;
	bool mPrilockTrans;
	bool mPrilockRot;

	ooctools::Quaternion localQuat;
	ooctools::Quaternion totalQuat;



	CGprogram g_cgVertexProg;
	CGparameter g_cgGlobalAmbient;
	CGparameter g_cgLightColor;
	CGparameter g_cgEyePosition;
	CGparameter g_cgLightPosition;
	CGparameter cgColorLut;
	CGparameter cgNColors;

	CGprogram g_cgFragmentProg;
	CGparameter g_cgShininess;
	CGparameter g_cgKs;
	CGparameter g_cgKd;
	CGparameter g_cgKa;
	CGparameter g_cgKe;
	CGparameter g_cgModelViewInv;



	void calcFPS();
	void pollSpaceNav();

	/**
	 * @brief Loads the 10 pre-defined camera-positions from 'camerapositions.data' into mPriSavedPositions.
	 * The file consists of 16 floats per cameraposition a.k.a. the respective Modelview-
	 * Matrix for each of the keys (1,2,..,0).
	 * The positions are saved each time a new positions is safed via 'Ctrl-#'.
	 * @see writeCameraPositions()
	 */
	void loadCameraPositions();

	/**
	 * @brief Writes the 10 pre-defined camera-positions in mPriSavedPositions from 'camerapositions.data'.
	 * The file consists of 16 floats per cameraposition a.k.a. the respective Modelview-
	 * Matrix for each of the keys (1,2,..,0).
	 * The positions are saved each time a new positions is safed via 'Ctrl-#'.
	 * @see loadCameraPositions()
	 */
	void writeCameraPositions();
	void cameraInit();
};

#endif /* SIMPLEGLFRAME_H_ */
