/**
 * @file	DataCoreGlFrame.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	DataCoreGlFrame class declaration.
 */

#ifndef DATACOREGLFRAME_H_
#define DATACOREGLFRAME_H_

#include "GL/glew.h"
#include "GL/glfw.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <vector>
#include <cstring>

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
#include "NodeRequestEvent.h"


/**
 * @class DataCoreGlFrame
 * @brief GlFrame implementation for the RenderingNode.
 */
class DataCoreGlFrame : public AbstractGlFrame, oocframework::AbstractEventListener {
public:
	DataCoreGlFrame();
	virtual ~DataCoreGlFrame();
	virtual void init();
	virtual void display(NodeRequestEvent& nre);
	virtual void display();
	virtual void reshape(int width, int height);
	inline float getFrames() const {
		return avgFps;
	}
	;
	void setMvMatrix(const float* matrix);

	void debug();
	void notify(oocframework::IEvent& event);

protected:
private:
	float scale;
	float fps[10];
	float avgFps;
	double time;
	unsigned frame;
	ooctools::VboManager* mPriVboMan;
	ooctools::CgToolkit* mPriCgt;

	float mPriModelViewMatrix[16];
	float mPriEyePosition[3];

	float myGlobalAmbient[3]; /* Dim */
	float myLightColor[3];  /* White */

	ooctools::Camera camObj;


	ooctools::Fbo* mPriFbo;
	int mPriWindowWidth;
	int mPriWindowHeight;

	int mPriButtonActions[3];

	GLuint mPriTexId;

	GLubyte* mPriPixelBuffer;
	GLfloat* mPriDepthBuffer;
	bool mPriNewDepthBuf;

	// Frustum-Culling member
	float priProj[16];
	float priModl[16];
	float priClip[16];
	float** priFrustum;
	float priFrustTemp;

	GLuint mPriOccQueries[100];
	std::map<uint64_t, GLint> mPriOccResults;

	std::map<uint64_t, std::string> mPriIdPathMap;
	std::map<uint64_t, oocformats::LooseOctree*> mPriIdLoMap;
	std::set<uint64_t> mPriIdsInFrustum;
	std::map<uint64_t, ooctools::IndexedVbo*> mPriVboMap;
	std::vector<ooctools::IndexedVbo*> mPriVisibleVbosVec;
	std::vector<float> mPriVisibleDistVec;
	std::multimap<float, uint64_t> mPriDistanceMap;

	bool mPriUseWireFrame;

	float mPriFarClippingPlane;
	float mPriNearClippingPlane;

	ooctools::Quaternion localQuat;
	ooctools::Quaternion totalQuat;

	oocformats::OctreeHandler mPriOh;
	oocformats::LooseOctree* mPriLo;
	ooctools::BoundingBox mPriSceneBB;
	ooctools::V3f mPriSceneCenter;


	CGprogram cgVertexProg;
	CGparameter cgTexture;
//	CGparameter cgGlobalAmbient;
//	CGparameter g_cgLightColor;
//	CGparameter g_cgEyePosition;
//	CGparameter g_cgLightPosition;
//	CGparameter cgColorLut;
//	CGparameter cgNColors;

	CGprogram cgFragmentProg;
//	CGparameter g_cgShininess;
//	CGparameter g_cgKs;
//	CGparameter g_cgKd;
//	CGparameter g_cgKa;
//	CGparameter g_cgKe;
//	CGparameter g_cgModelViewInv;

	typedef std::map<uint64_t, ooctools::IndexedVbo*>::iterator VboMapIter;

	void calcFPS();
	void loadMissingVbos(std::set<uint64_t>* idList, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap);
	void setupTexture();
	void drawDepthTex();
	void setupCg();
};

#endif /* DATACOREGLFRAME_H_ */
