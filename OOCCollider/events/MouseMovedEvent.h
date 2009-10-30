/**
 * @file	MouseMovedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	MouseMovedEvent class declaration.
 */

#ifndef MOUSEMOVEDEVENT_H_
#define MOUSEMOVEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class MouseMovedEvent
 *
 * @brief This event is being thrown whenever the window-manager class realizes that the
 * mouse was moved.
 *
 * This event will only be thrown by the Master-Node.
 *
 * @todo verify that this event is being in use. Looks like this was one of the first mouse events.
 * It would seem a little 'overheaddy' to transmit every single mouse movement over the network.
 */
class MouseMovedEvent : public oocframework::IEvent{
public:
	MouseMovedEvent();
	MouseMovedEvent(int x, int y);
	virtual ~MouseMovedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief return the x-position of the mouse-cursor.
	 */
	unsigned int getX() const {return ((int*)mProData)[0];};

	/**
	 * @brief return the x-position of the mouse-cursor.
	 */
	unsigned int getY() const {return ((int*)mProData)[1];};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* MOUSEMOVEDEVENT_H_ */
