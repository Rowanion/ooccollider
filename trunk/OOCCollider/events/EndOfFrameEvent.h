/**
 * @file	EndOfFrameEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	EndOfFrameEvent class declaration.
 */

#ifndef ENDOFFRAMEEVENT_H_
#define ENDOFFRAMEEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class EndOfFrameEvent
 *
 * @brief This event is thrown if all visible VBOs have been sent by a data-node to determine
 * the end of the VBO-Transmission.
 *
 * This event originates from a Data-Node and will be submitted to a Render-Node.
 *
 */
class EndOfFrameEvent : public oocframework::IEvent{
public:
	EndOfFrameEvent();
	virtual ~EndOfFrameEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* ENDOFFRAMEEVENT_H_ */
