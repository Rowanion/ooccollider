/**
 * @file	AccumulatedRendertimeEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	AccumulatedRendertimeEvent class declaration.
 */

#ifndef ACCUMULATEDRENDERTIMEEVENT_H_
#define ACCUMULATEDRENDERTIMEEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class AccumulatedRendertimeEvent
 *
 * @brief This event is thrown whenever the Master has requested a DepthBufferRequestEvent.
 * The Render-Nodes send their summed-up render-times to the master, so that new tile-dimensions
 * can be calculated.
 *
 * This event originates from the Render-Nodes and will be submitted to the master-node.
 *
 */
class AccumulatedRendertimeEvent : public oocframework::IEvent{
public:
	AccumulatedRendertimeEvent();
	AccumulatedRendertimeEvent(double renderTime);
	virtual ~AccumulatedRendertimeEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	virtual unsigned getByteSize(){return AccumulatedRendertimeEvent::mProByteSize;};

	double getRenderTime(){return ((double*)mProData)[0];};

protected:
	static oocframework::ClassId* mClassId;
	static unsigned mProByteSize;
	virtual void init();
private:
};

#endif /* ACCUMULATEDRENDERTIMEEVENT_H_ */
