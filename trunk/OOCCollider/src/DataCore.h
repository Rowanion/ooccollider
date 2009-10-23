/**
 * @file	DataCore.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	DataCore class declaration.
 */

#ifndef DATACORE_H_
#define DATACORE_H_

#include <queue>
#include <string>
#include <map>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "declarations.h"
#include "AbstractEventHandler.h"
#include "OctreeHandler.h"
#include "LooseOctree.h"
#include "MpiControl.h"

class DataCore {
public:
	DataCore(unsigned _winWidth, unsigned _winHeight, unsigned _targetWidth, unsigned _targetHeight);
	virtual ~DataCore();
	void renderOneFrame();
	void waitForDepthBuffers();

private:
	OOCWindow* mWindow;
	oocframework::MpiControl* mPriMpiCon;
	bool mRunning;
	std::queue<char*> mInQueue;
	std::queue<char*> mOutQueue;
	DataCoreGlFrame* mGlFrame;
	unsigned mPriDepthBufferCount;

	static DataCore* instance;

	void sendMethod(int dest);
	void handleMsg(oocframework::Message* msg);
};

#endif /* DATACORE_H_ */
