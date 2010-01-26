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

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "declarations.h"
#include "AbstractEventHandler.h"
#include "OctreeHandler.h"
#include "LooseRenderOctree.h"
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
	oocformats::LooseRenderOctree* mPriLo;
	Splittree* mPriSTree;
	vector<double> mPriRenderTimes;
	oocframework::MpiControl* mPriMpiCon;
	Tile mPriRootTile;
	SimpleGlFrame* mPriGlFrame;
	std::map<int, unsigned> mPriDataLoad;
	unsigned mPriFrameTick;
	double mPriCCollisionTime;

	unsigned mPriRendererDoneCount;
	unsigned mPriTileCount;
	oocframework::EventManager* mPriEventMan;
	std::vector<ooctools::Quintuple> mPriQuintVec;
	std::vector<ooctools::Quintuple> mPriOcclusionVec;
	std::map<int, std::set<ooctools::Quintuple> > mPriNodeReqMap;
	std::map<int, std::set<ooctools::Quintuple> > mPriOcclusionReqMap;
	ooctools::MTRand mPriMTwister;
	oocframework::CCollisionProtocol mPriCCol;


	std::map<int, Tile> mPriTileMap;
	int mPriWindowWidth;
	int mPriWindowHeight;

	std::map<int, double> mPriSceneCompletion;
	unsigned mPriSceneCompletionCount;
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

	list<int> mPriRendererComplete;
	int mPriPosition;
	bool mPriRequestSave;
	boost::filesystem::ofstream* mPriRequestOutFile;

	oocframework::Log* mPriLog;

	static RenderMasterCore* instance;

	MPI::Request sendQueue(int dest);
	void pollSpaceNav();
	void handleMsg(oocframework::Message* msg, bool debug = false);
	void manageCCollision();
	void checkForActiveMovement();

	/**
	 * @brief using mmap (memory-mapped io) to acquire a valid address to the file and returning it.
	 */
	char* mapFile(fs::path _path, unsigned _fileSize, int& _fHandle);

	/**
	 * @brief using munmap (memory-mapped io) to release a previously acquired address for a file.
	 */
	void umapFile(char* _map, unsigned _fileSize, int& _fHandle);

	void transmitAssignments();
};

#endif /* RENDERMASTERCORE_H_ */
