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


class RenderMasterCore: oocframework::AbstractEventListener {
public:
	RenderMasterCore();
	RenderMasterCore(unsigned _width = 640, unsigned _height = 480);
	virtual ~RenderMasterCore();
	virtual void Event(int event, bool state);
	virtual void notify(oocframework::IEvent& event);


private:
	OOCWindow* mWindow;
	bool mRunning;
	bool mTerminateApplication;
	std::queue<char*> mInQueue;
	std::queue<char*> mOutQueue;
	GLFWthread mSendThread;
	GLFWthread mReceiveThread;
	oocformats::OctreeHandler mPriOh;
	oocformats::LooseOctree* mPriLo;

	oocframework::EventManager* mPriEventMan;

	static RenderMasterCore* instance;

	MPI::Request sendQueue(int dest);
	void receiveQueue(int source);
	void pollSpaceNav();
};

#endif /* RENDERMASTERCORE_H_ */
