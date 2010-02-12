/**
 * @file	WindowClosedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	WindowClosedEvent class declaration.
 */


#ifndef WINDOWCLOSEDEVENT_H_
#define WINDOWCLOSEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class WindowClosedEvent
 *
 * @brief This event is being thrown whenever the window-manager class realizes that the
 * window has been closed.
 *
 * This event will only be thrown by the Master-Node.
 * Due to the nature of the event, it contains no data-fields at all. Maybe we'll add
 * a closing-reason later on, if needed.
 */
class WindowClosedEvent : public oocframework::IEvent{
public:
	WindowClosedEvent();
	virtual ~WindowClosedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* WINDOWCLOSEDEVENT_H_ */
