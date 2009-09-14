/**
 * @file	ColorBufferEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ColorBufferEvent class declaration.
 */

#ifndef COLORBUFFEREVENT_H_
#define COLORBUFFEREVENT_H_

#include <GL/gl.h>

#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"

/**
 * @class ColorBufferEvent
 *
 * @brief This event serves as container-event for a colorbuffer from a Render-Node.
 *
 * This event originates from a Render-Node and is sent to the Master-Node.
 *
 */
class ColorBufferEvent : public oocframework::IEvent{
public:
	ColorBufferEvent();
	ColorBufferEvent(int xPos, int yPos, int width, int height, double renderTime, const GLubyte* pixel);
	ColorBufferEvent(const Message* msg);
	virtual ~ColorBufferEvent();
	static const oocframework::ClassId* classid();
	inline virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Switching to member for byteSize because the size may differ per event.
	 * The framebuffer-section resolution is the key to size determination.
	 * @return the size in bytes stored in member mPriData
	 */
	inline virtual unsigned getByteSize(){return mPriByteSize;};

	/**
	 * @brief Returns the x-rasterposition of the framebuffer-section.
	 */
	inline int getX(){return ((int*)mProData)[0];};

	/**
	 * @brief Returns the y-rasterposition of the framebuffer-section.
	 */
	inline int getY(){return ((int*)mProData)[1];};

	/**
	 * @brief Returns the width of the framebuffer-section.
	 */
	inline int getWidth(){return ((int*)mProData)[2];};

	/**
	 * @brief Returns the heigh of the framebuffer-section.
	 */
	inline int getHeight(){return ((int*)mProData)[3];};

	/**
	 * @brief Returns the time in seconds it took to render this framebuffer segment.
	 */
	inline double getRenderTime(){
		return ((double*)(mProData+4*sizeof(int)))[0];
	};

	/**
	 * @brief Returns a const pointer to the pixel-data.
	 * A pixel consists of 4 GLubytes in the order BGRA.
	 */
	inline const GLubyte* getPixel(){
		return (GLubyte*)(mProData+sizeof(int)*4+sizeof(double));
	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
	unsigned mPriByteSize;
};

#endif /* COLORBUFFEREVENT_H_ */
