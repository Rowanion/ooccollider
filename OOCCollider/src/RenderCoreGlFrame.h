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
#include "OOCCamera.h"


/**
 * @class RenderCoreGlFrame
 * @brief GlFrame implementation for the RenderingNode.
 */
class RenderCoreGlFrame : public AbstractGlFrame, oocframework::AbstractEventListener {
public:
	RenderCoreGlFrame(int width, int height);
	virtual ~RenderCoreGlFrame();
	virtual void init();
	virtual void setupCg();
	virtual void display();
	virtual void reshape(int width, int height);
	virtual void reshape(int width, int height, float farPlane);
	virtual void resizeFrustum();
	virtual void resizeFrustum(unsigned _width, unsigned _height);
	virtual void resizeFrustum(unsigned tileXPos, unsigned tileYPos,
			unsigned tileswidth, unsigned tilesheight);
	float getFrames() const {
		return avgFps;
	}
	;
	void debug();
	void notify(oocframework::IEvent& event);
	void getFrustum();
	void setTileDimensions(int xPos, int yPos, int width, int height);
	inline double getRenderTime()const {return mPriRenderTimeSum;};
	inline void resetRenderTime() {mPriRenderTimeSum = 0.0;};
	void depthPass();

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
	float lightPos[3];
	ooctools::VboManager* mPriVboMan;
	ooctools::CgToolkit* mPriCgt;

	float mPriModelViewMatrix[16];
	float* mPriModelViewProjMatrix;
	ooctools::V3f mPriEyePosition;
	bool mPriCamHasMoved;
	unsigned mPriBBMode;

	float mPriAspectRatio;

	float myGlobalAmbient[3]; /* Dim */
	float myLightColor[3];  /* White */

	ooctools::ColorTable mPriColorTable;

	ooctools::Fbo* mPriFbo;
	int mPriWindowWidth;
	int mPriWindowHeight;
	GLfloat ratio;
	unsigned mPriTileYPos, mPriTileXPos, mPriTileWidth, mPriTileHeight;

	int mPriButtonActions[3];

	GLubyte* mPriPixelBuffer;
	GLfloat* mPriDepthBuffer;
	GLfloat* mPriDepthBufferD;
	GLfloat* mPriDepthTexture;
	GLuint mPriDepthTexId;

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
	std::map<uint64_t, ooctools::IndexedVbo*> mPriOfflineVbosInFrustum;
	std::set<uint64_t> mPriMissingIdsInFrustum;
	std::vector< std::map<uint64_t, ooctools::IndexedVbo*>::iterator > mPriObsoleteVbos;

	bool mPriUseWireFrame;

	std::set<uint64_t> mPriRequestedVboList;

	float mPriFarClippingPlane, mPriNearClippingPlane;

	float mPriFrustumExtension;

	OOCCamera mPriCamera;


	oocformats::OctreeHandler mPriOh;
	oocformats::LooseOctree* mPriLo;
	ooctools::BoundingBox mPriSceneBB;
	ooctools::V3f mPriSceneCenter;

	double mPriRenderTimeSum;

	bool mPriShowOffset;

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

	typedef std::map<uint64_t, ooctools::IndexedVbo*>::iterator IdVboMapIter;
	typedef std::map<uint64_t, ooctools::IndexedVbo*>::const_iterator CIdVboMapIter;
	typedef std::map<uint64_t, ooctools::IndexedVbo*>::reverse_iterator RIdVboMapIter;
	typedef std::multimap<float, uint64_t>::iterator FloatIdMMapIter;
	typedef std::multimap<float, uint64_t>::const_iterator CFloatIdMMapIter;
	typedef std::multimap<float, uint64_t>::reverse_iterator RFloatIdMMapIter;
	typedef std::set<uint64_t>::iterator IdSetIter;
	typedef std::set<uint64_t>::const_iterator CIdSetIter;
	typedef std::set<uint64_t>::reverse_iterator RIdSetIter;

	void calcFPS();
	void requestMissingVbos();
	void compareVbos(std::map<uint64_t, ooctools::IndexedVbo*>* vboMap, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap2);

	/**
	 * @brief Divides the set of nodes in frustum into a set of obsoleteVbos (ie. not anymore in frustum) and newVbos (ie. not
	 * currently online).
	 */
	void divideIdList();

	/**
	 * @brief Ensures that the list with OfflineVBOS a.k.a. cache is not larger than MAX_OFFLINE_VBOS.
	 * @todo Make this method more generic so that different displacement policies (ie. LRU or GreatestDistanceToEye)can be utilized.
	 */
	void trimCacheMap();

	void setupTexture();
	void drawDepthTex();
	void initTiles(bool extendFrustum);
};

#endif /* RENDERCOREGLFRAME_H_ */
