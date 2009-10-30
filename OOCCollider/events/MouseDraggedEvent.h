/**
 * @file	MouseDraggedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	MouseDraggedEvent class declaration.
 */

#ifndef MOUSEDRAGGEDEVENT_H_
#define MOUSEDRAGGEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class MouseDraggedEvent
 *
 * @brief This event is thrown whenever the window class registers that the mouse is
 * moved while a button is pressed.
 *
 * This event originates from the Master-Node.
 *
 */
class MouseDraggedEvent : public oocframework::IEvent{
public:
	MouseDraggedEvent();
	MouseDraggedEvent(int x, int y);
	virtual ~MouseDraggedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Returns the x-position to where the mouse was dragged.
	 */
	int getX() const {return ((int*)mProData)[0];};

	/**
	 * @brief Returns the y-position to where the mouse was dragged.
	 */
	int getY() const {return ((int*)mProData)[1];};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* MOUSEDRAGGEDEVENT_H_ */
