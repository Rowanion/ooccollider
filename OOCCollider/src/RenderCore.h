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
#include "MpiControl.h"
#include "AbstractEventListener.h"


class RenderCore : public oocframework::AbstractEventListener{
public:
	RenderCore(unsigned _winWidth, unsigned _winHeight, unsigned _targetWidth, unsigned _targetHeight);
	virtual ~RenderCore();
	virtual void notify(oocframework::IEvent& event);

private:
	OOCWindow* mWindow;
	oocframework::MpiControl* mPriMpiCon;
	bool mRunning;
	bool mPriGotMatrix;
	std::queue<char*> mInQueue;
	std::queue<char*> mOutQueue;
	RenderCoreGlFrame* mPriGlFrame;

	static RenderCore* instance;

	bool mPriReloadTest;
	double mPriReloadStart, mPriReloadStop;

	void sendMethod(int dest);
	void receiveMethod(int source);
	bool ireceiveMethod(int source);
	void handleMsg(oocframework::Message* msg);
};

#endif /* RENDERCORE_H_ */
