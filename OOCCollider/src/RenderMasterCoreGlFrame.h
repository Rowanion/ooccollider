/**
 * @file	RenderMasterCoreGlFrame.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderMasterCoreGlFrame class declaration.
 */

#ifndef RENDERMASTERCOREGLFRAME_H_
#define RENDERMASTERCOREGLFRAME_H_

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
 * @class RenderMasterCoreGlFrame
 * @brief GlFrame implementation for the RenderingNode.
 */
class RenderMasterCoreGlFrame : public AbstractGlFrame, oocframework::AbstractEventListener {
public:
	RenderMasterCoreGlFrame();
	virtual ~RenderMasterCoreGlFrame();
	virtual void init();
	virtual void setupCg();
	virtual void display();
	virtual void initMatrices();
	virtual void calcMatrix();
	virtual void multMatrix(float *m1, float *m2, float *res);
	virtual void reshape(int width, int height);
	virtual void setVbo(ooctools::IndexedVbo* iVbo);
	virtual void setColorBuffer(int x, int y, int width, int height, GLubyte* buffer);
	virtual void resizeWindow();
	virtual void resizeWindow(unsigned _height, unsigned _width);
	virtual void resizeWindow(unsigned topLine, unsigned tilesheight,
			unsigned leftLine, unsigned tileswidth);
	float getFrames() const {
		return avgFps;
	}
	;
	void debug();
//	void setColorBuffer(const GLubyte* pixel, int x, int y, int width, int height);
	void pollSpaceNav();
	void notify(oocframework::IEvent& event);

protected:
	void normalizeFrustum();

	//frustumculling variables
	std::vector< std::vector<float> > frustum;
	std::vector<float> frustum1;
	std::vector<float> frustum2;
	std::vector<float> frustum3;
	std::vector<float> frustum4;
	std::vector<float> frustum5;
	std::vector<float> frustum6;
	float proj[16];
	float modl[16];
	float clip[16];
	float frustTemp;
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
	bool mPriCamHasMoved;

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
};

#endif /* RENDERMASTERCOREGLFRAME_H_ */
