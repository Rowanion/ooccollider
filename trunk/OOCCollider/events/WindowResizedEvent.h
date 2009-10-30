/**
 * @file	WindowResizedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	WindowResizedEvent class declaration.
 */

#ifndef WINDOWRESIZEDEVENT_H_
#define WINDOWRESIZEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class WindowResizedEvent
 *
 * @brief This event is being thrown whenever the window-manager class realizes that the
 * window size has been changed.
 *
 * This event will only be thrown by the Master-Node.
 *
 * @todo Real usefulness of this event has still to be proven. It was the first test-case with events.
 */
class WindowResizedEvent : public oocframework::IEvent{
public:
	WindowResizedEvent();
	WindowResizedEvent(int width, int height);
	virtual ~WindowResizedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief returns the new window-width.
	 */
	int getWidth() const {return ((int*)mProData)[0];};

	/**
	 * @brief returns the new window-height.
	 */
	int getHeight() const {return ((int*)mProData)[1];};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* WINDOWRESIZEDEVENT_H_ */
