/**
 * @file	MouseWheelEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	MouseWheelEvent class declaration.
 */

#ifndef MOUSEWHEELEVENT_H_
#define MOUSEWHEELEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class MouseWheelEvent
 *
 * @brief This event is being thrown whenever the window-manager class realizes that the
 * mouse wheel has been moved.
 *
 * This event will only be thrown by the Master-Node.
 */
class MouseWheelEvent : public oocframework::IEvent{
public:
	enum Direction{
		STILL = 0,
		UP = 1,
		DOWN = 2
	};
	MouseWheelEvent();
	MouseWheelEvent(Direction _direction);
	virtual ~MouseWheelEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief returns the direction in which the mousewheel was moved
	 */
	Direction getDirection() const {return ((Direction*)mProData)[0];};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* MOUSEWHEELEVENT_H_ */
