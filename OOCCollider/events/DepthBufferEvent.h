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
#include "Structs.h"

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
	DepthBufferEvent(Tile _tileDim, int xPos, int yPos, int width, int height, int mpiRank, const GLfloat* depth);
	DepthBufferEvent(const oocframework::Message* msg);
	virtual ~DepthBufferEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Returns the original tile-dimensions.
	 * This is needed to resize the frustum accordingly. Because all the depth-rendering happens at
	 * extended frustum and the depthbuffer-size changes, this needed.
	 */
	Tile getTile(){return ((Tile*)mProData)[0];};

	/**
	 * @brief Returns the x-Part of the RasterPosition.
	 * This is needed to restore later the depth-value to correct origin in the framebuffer.
	 */
	int getX(){return ((int*)(mProData+sizeof(Tile)))[0];};

	/**
	 * @brief Returns the y-Part of the RasterPosition.
	 * This is needed to restore later the depth-value to correct origin in the framebuffer.
	 */
	int getY(){return ((int*)(mProData+sizeof(Tile)))[1];};

	/**
	 * @brief Returns the width of the framebuffer section.
	 */
	int getWidth(){return ((int*)(mProData+sizeof(Tile)))[2];};
	/**
	 * @brief Returns the height of the framebuffer section.
	 */
	int getHeight(){return ((int*)(mProData+sizeof(Tile)))[3];};

	/**
	 * @brief Returns the mpi-rank of the originating mpi-node.
	 */
	int getMpiRank(){return ((int*)(mProData+sizeof(Tile)))[4];};

	/**
	 * @brief Returns a const pointer to the depth-data.
	 */
	const GLfloat* getDepth(){
		return (GLfloat*)(mProData+sizeof(Tile)+sizeof(int)*4);
	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* DEPTHBUFFEREVENT_H_ */
