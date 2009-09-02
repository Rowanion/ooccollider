/**
 * @file	EndTransmissionEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	EndTransmissionEvent class declaration.
 */

#ifndef ENDTRANSMISSIONEVENT_H_
#define ENDTRANSMISSIONEVENT_H_

#include "IEvent.h"
#include "ClassId.h"

/**
 * @class EndTransmissionEvent
 *
 * @brief This event is thrown if all visible VBOs have been sent by a data-node to determine
 * the end of the VBO-Transmission.
 *
 * This event originates from a Data-Node and will be submitted to a Render-Node.
 *
 */
class EndTransmissionEvent : public oocframework::IEvent{
public:
	EndTransmissionEvent();
	virtual ~EndTransmissionEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	virtual unsigned getByteSize(){return EndTransmissionEvent::mProByteSize;};

protected:
	static oocframework::ClassId* mClassId;
	static unsigned mProByteSize;
	virtual void init();
private:
};

#endif /* ENDTRANSMISSIONEVENT_H_ */
