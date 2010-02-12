/**
 * @file	InfoRequestEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	InfoRequestEvent class declaration.
 */

#ifndef INFOREQUESTEVENT_H_
#define INFOREQUESTEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class InfoRequestEvent
 *
 * @brief This event is thrown whenever the 'i' is pressed. Each recipient decides what
 * information should be printed to the console.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class InfoRequestEvent : public oocframework::IEvent{
public:
	InfoRequestEvent();
	virtual ~InfoRequestEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* INFOREQUESTEVENT_H_ */
