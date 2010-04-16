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
#include "OOCCamera.h"
#include "LooseRenderOctree.h"
#include "OctreeHandler.h"


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
	float getFrames() const { return avgFps;};
	const float* getMvMatrix() const {return mPriModelViewMatrix;};
	const float* createMvMatrix();
	void notify(oocframework::IEvent& event);


protected:

	unsigned width, height;
	float nearPlane, farPlane;

private:
	float scale;
	float fps[10];
	float avgFps;
	double time;
	unsigned frame;
	ooctools::VboManager* mPriVboMan;
	ooctools::CgToolkit* mPriCgt;
	oocformats::LooseRenderOctree* mPriLo;

	float mPriModelViewMatrix[16];
	float mPriSavedPositions[10][16];

	float myGlobalAmbient[3]; /* Dim */
	float myLightColor[3];  /* White */

	ooctools::IndexedVbo* mPriIVbo;
	ooctools::Camera camObj;

	int oldPosX;
	int oldPosY;

	OOCCamera mPriCamera;
	float walkingSpeed;

	int mPriButtonActions[3];

	unsigned mPriMinLvl;
	ooctools::Fbo* mPriFboWire;
	ooctools::Fbo* mPriFboSolid;

	GLubyte *mPriColorBuffer;

	int mPriCBufWidth;
	int mPriCBufHeight;
	int mPriCBufX;
	int mPriCBufY;

	bool mPriUseSpaceNav;
	bool mPrilockTrans;
	bool mPrilockRot;

	bool mPriStepLeft;
	bool mPriStepRight;
	bool mPriStepForward;
	bool mPriStepBackward;
	bool mPriRollLeft;
	bool mPriRollRight;
	bool mPriTiltUp;
	bool mPriTiltDown;
	bool mPriTurnLeft;
	bool mPriTurnRight;
	bool mPriTurnUp;
	bool mPriTurnDown;

	int mPriBBMode;
	ooctools::ColorTable mPriColorTable;

	CGprogram vpProxy;
	CGprogram gpProxy;
	CGprogram fpProxy;

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
	CGparameter cgFragLUT;

	CGprogram cgVertNoLight;
	CGprogram cgFragNoLight;
	CGparameter cgNoLightLUT;

	CGprogram cgVertDepthTex;
	CGprogram cgFragDepthTex;
	CGparameter cgDepthTex;
	oocformats::OctreeHandler mPriOh;
	bool mPriRenderFrame;
	float lightPos[3];
	float mPriNearPlane;
	float mPriFarPlane;

	unsigned mPriBoxCount[15];

	void setupCg();

	void calcFPS();

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
	void applyKeyEvents();
	void renderBBs(oocformats::LooseRenderOctree* lro);
	void renderBBsSolid(oocformats::LooseRenderOctree* lro);
	void renderBBLines(oocformats::LooseRenderOctree* lro);
	void renderFrame(unsigned _maxFiles = 100);
	char* mapFile(fs::path _path, unsigned _fileSize, int& _fHandle);
	void umapFile(char* _map, unsigned _fileSize, int& _fHandle);
	void countBBs(oocformats::LooseRenderOctree* lro);
};

#endif /* SIMPLEGLFRAME_H_ */
