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
#include "Message.h"

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
	DepthBufferRequestEvent(const float* _matrix);
	DepthBufferRequestEvent(const oocframework::Message* msg);
	virtual ~DepthBufferRequestEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	const float* getMatrix() const {return ((const float*)mProData);};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* DEPTHBUFFERREQUESTEVENT_H_ */
