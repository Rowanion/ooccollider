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
#include "LooseRenderOctree.h"
#include "OctreeHandler.h"
#include "V3f.h"
#include "OOCCamera.h"
#include "ColorBufferEvent.h"


/**
 * @class RenderCoreGlFrame
 * @brief GlFrame implementation for the RenderingNode.
 */
class RenderCoreGlFrame : public AbstractGlFrame, oocframework::AbstractEventListener {
public:
	RenderCoreGlFrame(int winWidth, int winHeight, int targetWinWidth, int targetWinHeight);
	virtual ~RenderCoreGlFrame();
	virtual void init();
	virtual void setupCg();
	virtual void display();
	virtual void reshape(int width, int height);
	virtual void reshape(int width, int height, float farPlane);
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
	ColorBufferEvent& getColorBufferEvent() {return mPriColorBufferEvent;};
	void cullFrustum();
	void manageCaching();
	void clearRequests();
//	void initTiles();


protected:

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
	ooctools::V3f mPriEyePosition;
	ooctools::V3f mPriViewVector;
	bool mPriCamHasMoved;
	unsigned mPriBBMode;

	float mPriAspectRatio;

	float* mPriMaxDistPerLevel;

	float myGlobalAmbient[3]; /* Dim */
	float myLightColor[3];  /* White */

	ooctools::ColorTable mPriColorTable;

	ooctools::Fbo* mPriFbo;
	GLfloat ratio;
	unsigned mPriTileYPos, mPriTileXPos, mPriTileWidth, mPriTileHeight;

	int mPriButtonActions[3];

	GLubyte* mPriPixelBuffer;
	GLfloat* mPriDepthBuffer;
	GLfloat* mPriDepthBufferD;
	GLfloat* mPriDepthTexture;
	GLuint mPriDepthTexId;

	unsigned mPriTriCount;
	ColorBufferEvent mPriColorBufferEvent;

	// Frustum-Culling member
	float priProj[16];
	float priModl[16];
	float priClip[16];
	float** priFrustum;
	float priFrustTemp;

	std::map<uint64_t, std::string> mPriIdPathMap;
	std::map<uint64_t, oocformats::LooseRenderOctree*> mPriIdLoMap;
	std::set<uint64_t> mPriIdsInFrustum;
	std::map<uint64_t, int> mPriIdLvlInFrustum;
	std::set<uint64_t> mPriIdsInExtFrustum;
	std::map<uint64_t, ooctools::IndexedVbo*> mPriVbosInFrustum;
	std::map<uint64_t, ooctools::IndexedVbo*> mPriOfflineVbosInFrustum;
	std::set<uint64_t> mPriMissingIdsInFrustum;
	std::list< std::map<uint64_t, ooctools::IndexedVbo*>::iterator > mPriObsoleteVbos;

	std::list<oocformats::WrappedOcNode*> mPriWrapperInFrustum;
	std::set<ooctools::Quintuple> mPriRequests;
	unsigned int mPriL1Cache;
	unsigned int mPriL2Cache;

	bool mPriUseWireFrame;

	std::set<uint64_t> mPriRequestedVboList;

	OOCCamera mPriCamera;


	oocformats::OctreeHandler mPriOh;
	oocformats::LooseRenderOctree* mPriLo;
	ooctools::BoundingBox mPriSceneBB;
	ooctools::V3f mPriSceneCenter;

	double mPriRenderTimeSum;

	bool mPriShowOffset;

	unsigned mPriFrameTick;
	double mPriDisplayTime;
	double mPriFrustumCullingTime;
	double mPriRequestDataTime;

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
	typedef std::list<uint64_t>::iterator IdListIter;
	typedef std::list<uint64_t>::const_iterator CIdListIter;
	typedef std::list<uint64_t>::reverse_iterator RIdListIter;
	typedef std::set<ooctools::Triple>::iterator TripleSetIter;
	typedef std::set<ooctools::Triple>::const_iterator CTripleSetIter;
	typedef std::set<ooctools::Triple>::reverse_iterator RTripleSetIter;
	typedef std::map<uint64_t, int>::iterator IdLvlMapIter;
	typedef std::map<uint64_t, int>::const_iterator CIdLvlSetIter;
	typedef std::map<uint64_t, int>::reverse_iterator RIdLvlSetIter;
	typedef std::list<oocformats::WrappedOcNode*>::iterator WrapperListIter;
	typedef std::list<oocformats::WrappedOcNode*>::const_iterator CWrapperListIter;
	typedef std::list<oocformats::WrappedOcNode*>::reverse_iterator RWrapperListIter;
	typedef std::set<ooctools::Quintuple>::iterator QuintSetIter;
	typedef std::set<ooctools::Quintuple>::const_iterator CQuintSetIter;
	typedef std::set<ooctools::Quintuple>::reverse_iterator RQuintSetIter;

	void calcFPS();
	void requestMissingVbos();
	void compareVbos(std::map<uint64_t, ooctools::IndexedVbo*>* vboMap, std::map<uint64_t, ooctools::IndexedVbo*>* vboMap2);

	/**
	 * @brief Divides the set of nodes in frustum into a set of obsoleteVbos (ie. not anymore in frustum) and newVbos (ie. not
	 * currently online). The obsolete VBOs are in form of iterators to these elements.
	 */
	void divideIdList();

	/**
	 * @brief Ensures that the list with OfflineVBOS a.k.a. cache is not larger than MAX_OFFLINE_VBOS.
	 * @todo Make this method more generic so that different displacement policies (ie. LRU or GreatestDistanceToEye)can be utilized.
	 */
	void trimCacheMap();

	void setupTexture();
	void drawDepthTex();

	/**
	 * @brief Functions takes two sets with IDs and a reference to a third set which will contain all elements
	 * which are only present in one of the sets. It's a kind of set-XOR
	 */
	void uniqueElements(const std::set<uint64_t>& leftSet, const std::set<uint64_t>& rightSet, std::set<uint64_t>& uniqueSet);

	/**
	 * @brief Functions takes two maps with IDs and a reference to a third mapt which will contain all elements
	 * which are only present in one of the maps. It's a kind of map-XOR
	 */
	void uniqueElements(const std::map<uint64_t, ooctools::IndexedVbo*>& leftMap, const std::set<uint64_t>& rightSet, std::set<uint64_t>& uniqueSet);
	void stripDoublesFromRight(const std::set<uint64_t>& leftSet, std::set<uint64_t>& rightSet);
	void stripDoublesFromRight(const std::map<uint64_t, ooctools::IndexedVbo*>& leftMap, std::set<uint64_t>& rightSet);
	void generateMaxDistPerLevel(unsigned _maxLevel, float _originalSize);
};

#endif /* RENDERCOREGLFRAME_H_ */
