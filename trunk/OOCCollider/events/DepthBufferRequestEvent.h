/**
 * @file	DepthBufferRequestEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	DepthBufferRequestEvent class declaration.
 */

#ifndef DEPTHBUFFERREQUESTEVENT_H_
#define DEPTHBUFFERREQUESTEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class DepthBufferRequestEvent
 *
 * @brief This event is thrown whenever the Master determines that the depth-buffer should be updated.
 *
 * This event originates from the Master-Node and will be submitted to all render-nodes.
 *
 */
class DepthBufferRequestEvent : public oocframework::IEvent{
public:
	DepthBufferRequestEvent();
	virtual ~DepthBufferRequestEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	virtual unsigned getByteSize(){return DepthBufferRequestEvent::mProByteSize;};

protected:
	static oocframework::ClassId* mClassId;
	static unsigned mProByteSize;
	virtual void init();
private:
};

#endif /* DEPTHBUFFERREQUESTEVENT_H_ */
