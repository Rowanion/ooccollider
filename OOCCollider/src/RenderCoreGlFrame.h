/**
 * @file	RenderCoreGlFrame.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderCoreGlFrame class declaration.
 */

#ifndef RENDERCOREGLFRAME_H_
#define RENDERCOREGLFRAME_H_

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
#include "LooseOctree.h"
#include "OctreeHandler.h"
#include "V3f.h"


/**
 * @class RenderCoreGlFrame
 * @brief GlFrame implementation for the RenderingNode.
 */
class RenderCoreGlFrame : public AbstractGlFrame, oocframework::AbstractEventListener {
public:
	RenderCoreGlFrame();
	virtual ~RenderCoreGlFrame();
	virtual void init();
	virtual void setupCg();
	virtual void display();
	virtual void reshape(int width, int height);
	virtual void setVbo(ooctools::IndexedVbo* iVbo);
	virtual void resizeWindow();
	virtual void resizeWindow(unsigned _height, unsigned _width);
	virtual void resizeWindow(unsigned topLine, unsigned tilesheight,
			unsigned leftLine, unsigned tileswidth);
	float getFrames() const {
		return avgFps;
	}
	;
	void debug();
	void notify(oocframework::IEvent& event);
	void getFrustum();

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
	float* mPriModelViewProjMatrix;
	float mPriEyePosition[3];
	ooctools::V3f eyePosition;
	bool mPriCamHasMoved;

	float myGlobalAmbient[3]; /* Dim */
	float myLightColor[3];  /* White */

	ooctools::IndexedVbo* mPriIVbo;
	ooctools::Camera camObj;


	ooctools::Fbo* mPriFbo;
	int mPriWindowWidth;
	int mPriWindowHeight;

	int mPriButtonActions[3];

	GLubyte* mPriPixelBuffer;
	GLfloat* mPriDepthBuffer;

	unsigned mPriTriCount;

	// Frustum-Culling member
	float priProj[16];
	float priModl[16];
	float priClip[16];
	float** priFrustum;
	float priFrustTemp;

	std::map<uint64_t, std::string> mPriIdPathMap;
	std::map<uint64_t, oocformats::LooseOctree*> mPriIdLoMap;
	std::set<uint64_t> mPriIdsInFrustum;
	std::map<uint64_t, ooctools::IndexedVbo*> mPriVbosInFrustum;
	std::map<uint64_t, ooctools::IndexedVbo*> mPriVbosInFrustum2;
	std::map<uint64_t, ooctools::IndexedVbo*> mPriOfflineVbosInFrustum;

	bool mPriUseWireFrame;

	std::set<uint64_t> mPriRequestedVboList;

	float mPriFarClippingPlane;

	ooctools::Quaternion localQuat;
	ooctools::Quaternion totalQuat;

	oocformats::OctreeHandler mPriOh;
	oocformats::LooseOctree* mPriLo;
	ooctools::BoundingBox mPriSceneBB;
	ooctools::V3f mPriSceneCenter;


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

	typedef std::map<uint64_t, ooctools::IndexedVbo*>::iterator VboMapIter;

	void calcFPS();
	void requestMissingVbos(std::set<uint64_t>* idList, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap);
	void loadMissingVbosFromDisk(std::set<uint64_t>* idList, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap);
	void compareVbos(std::map<uint64_t, ooctools::IndexedVbo*>* vboMap, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap2);
};

#endif /* RENDERCOREGLFRAME_H_ */
