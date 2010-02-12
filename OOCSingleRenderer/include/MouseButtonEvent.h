/**
 * @file	MouseButtonEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	MouseButtonEvent class declaration.
 */

#ifndef MOUSEBUTTONEVENT_H_
#define MOUSEBUTTONEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class MouseButtonEvent
 *
 * @brief This event is thrown whenever the window class registers that a mouse button has been
 * pressed.
 *
 * This event originates from the Master-Node.
 *
 */
class MouseButtonEvent : public oocframework::IEvent{
public:
	MouseButtonEvent();
	MouseButtonEvent(int x, int y, int state, int button);
	virtual ~MouseButtonEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Returns the x-position of the mouse-pointer.
	 */
	int getX() const {
		return ((int*)mProData)[0];
	};

	/**
	 * @brief Returns the y-position of the mouse-pointer.
	 */
	int getY() const {
		return ((int*)mProData)[1];
	};

	/**
	 * @brief Returns the state of the mouse-button.
	 */
	int getState() const {
		return ((int*)mProData)[2];
	};

	/**
	 * @brief Returns the mouse-button which triggered the event.
	 */
	int getButton() const {
		return ((int*)mProData)[3];
	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* MOUSEBUTTONEVENT_H_ */
