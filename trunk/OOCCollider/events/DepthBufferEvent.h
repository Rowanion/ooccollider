/**
 * @file	DepthBufferEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	DepthBufferEvent class declaration.
 */

#ifndef DEPTHBUFFEREVENT_H_
#define DEPTHBUFFEREVENT_H_

#include <GL/gl.h>

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"

/**
 * @class DepthBufferEvent
 *
 * @brief This event is actually a container event for packaging, shipping and receiving the z-buffer-data
 * over the network.
 *
 * This event originates from Render-Nodes and will be sent to Data-Nodes.
 *
 */
class DepthBufferEvent : public oocframework::IEvent{
public:
	DepthBufferEvent();
	DepthBufferEvent(int xPos, int yPos, int width, int height, int mpiRank, const GLfloat* depth);
	DepthBufferEvent(const oocframework::Message* msg);
	virtual ~DepthBufferEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Switching to member for byteSize because the size may differ per event.
	 * The framebuffer-resolution is the key to size-determination.
	 * @return the size in bytes stored in member mPriData
	 */
	virtual unsigned getByteSize(){return mPriByteSize;};

	/**
	 * @brief Returns the x-Part of the RasterPosition.
	 * This is needed to restore later the depth-value to correct origin in the framebuffer.
	 */
	int getX(){return ((int*)mProData)[0];};

	/**
	 * @brief Returns the y-Part of the RasterPosition.
	 * This is needed to restore later the depth-value to correct origin in the framebuffer.
	 */
	int getY(){return ((int*)mProData)[1];};

	/**
	 * @brief Returns the width of the framebuffer section.
	 */
	int getWidth(){return ((int*)mProData)[2];};
	/**
	 * @brief Returns the height of the framebuffer section.
	 */
	int getHeight(){return ((int*)mProData)[3];};

	/**
	 * @brief Returns the mpi-rank of the originating mpi-node.
	 */
	int getMpiRank(){return ((int*)mProData)[4];};

	/**
	 * @brief Returns a const pointer to the depth-data.
	 */
	const GLfloat* getDepth(){
		return (GLfloat*)(mProData+sizeof(int)*4);
	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
	unsigned mPriByteSize;
};

#endif /* DEPTHBUFFEREVENT_H_ */
