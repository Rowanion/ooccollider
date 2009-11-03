/**
 * @file	RenderMasterCore.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderMasterCore class declaration.
 */

#ifndef RENDERMASTERCORE_H_
#define RENDERMASTERCORE_H_

#include <queue>
#include <string>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "mpi.h"

#include "declarations.h"
#include "AbstractEventHandler.h"
#include "OctreeHandler.h"
#include "LooseOctree.h"
#include "AbstractEventListener.h"
#include "MouseDraggedEvent.h"
#include "Fbo.h"
#include "Splittree.h"
#include "NodeRequestEvent.h"
#include "MersenneTwister.h"
#include "CCollisionProtocol.h"


class RenderMasterCore: oocframework::AbstractEventListener {
public:
	RenderMasterCore(unsigned _width = 640, unsigned _height = 480);
	virtual ~RenderMasterCore();
	virtual void Event(int event, bool state);
	virtual void notify(oocframework::IEvent& event);

private:
	OOCWindow* mWindow;
	bool mRunning;
	bool mTerminateApplication;
	bool mPriCamHasMoved;
	unsigned mPriFrameCount;
	unsigned mPriRenderTimeCount;
	std::queue<char*> mInQueue;
	std::queue<char*> mOutQueue;
	GLFWthread mSendThread;
	GLFWthread mReceiveThread;
	oocformats::OctreeHandler mPriOh;
	oocformats::LooseOctree* mPriLo;
	Splittree* mPriSTree;
	vector<double> mPriRenderTimes;
	oocframework::MpiControl* mPriMpiCon;
	Tile mPriRootTile;
	SimpleGlFrame* mPriGlFrame;
	std::map<int, unsigned> mPriDataLoad;

	unsigned mPriRendererDoneCount;
	unsigned mPriTileCount;
	oocframework::EventManager* mPriEventMan;
	std::set<ooctools::Quintuple> mPriQuintSet;
	std::map<int, std::set<Quintuple> > mPriNodeReqMap;
	ooctools::MTRand mPriMTwister;
	oocframework::CCollisionProtocol mPriCCol;


	std::map<int, Tile> mPriTileMap;
	int mPriWindowWidth;
	int mPriWindowHeight;

	static RenderMasterCore* instance;

	MPI::Request sendQueue(int dest);
	void pollSpaceNav();
	void handleMsg(oocframework::Message* msg);
	void manageCCollision();
};

#endif /* RENDERMASTERCORE_H_ */
