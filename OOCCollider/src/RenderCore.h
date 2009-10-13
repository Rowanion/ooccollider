/**
 * @file	RenderCore.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	RenderCore class declaration.
 */

#ifndef RENDERCORE_H_
#define RENDERCORE_H_

#include <queue>
#include <string>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "declarations.h"
#include "AbstractEventHandler.h"
#include "OctreeHandler.h"
#include "LooseOctree.h"


class RenderCore {
public:
	RenderCore(unsigned _width=640, unsigned _height=480, unsigned _finalWidth=640, unsigned _finalHeight=480);
	virtual ~RenderCore();

private:
	OOCWindow* mWindow;
	MpiControl* mPriMpiCon;
	bool mRunning;
	bool mPriGotMatrix;
	std::queue<char*> mInQueue;
	std::queue<char*> mOutQueue;
	RenderCoreGlFrame* mPriGlFrame;

	static RenderCore* instance;

	void sendMethod(int dest);
	void receiveMethod(int source);
	bool ireceiveMethod(int source);
	void handleMsg(Message* msg);
};

#endif /* RENDERCORE_H_ */
