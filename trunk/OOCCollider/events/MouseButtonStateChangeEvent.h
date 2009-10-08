/**
 * @file	MouseButtonStateChangeEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	MouseButtonStateChangeEvent class declaration.
 */

#ifndef MOUSEBUTTONSTATECHANGEEVENT_H_
#define MOUSEBUTTONSTATECHANGEEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class MouseButtonStateChangeEvent
 *
 * @brief This event is thrown whenever the window class registers that a mouse button changes its
 * state. Eg. from pressed to released.
 *
 * This event originates from the Master-Node.
 *
 */
class MouseButtonStateChangeEvent : public oocframework::IEvent{
public:
	MouseButtonStateChangeEvent();
	MouseButtonStateChangeEvent(int button, int oldState, int newState);
	virtual ~MouseButtonStateChangeEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	virtual unsigned getByteSize(){return MouseButtonStateChangeEvent::mProByteSize;};

	/**
	 * @brief Returns the button which changed its state.
	 */
	int getButton() const {return ((int*)mProData)[0];};

	/**
	 * @brief Returns the new state of the button.
	 */
	int getNewState() const {return ((int*)mProData)[1];};

	/**
	 * @brief Returns the old state of the button, before the change.
	 */
	int getOldState() const {return ((int*)mProData)[2];};

protected:
	static oocframework::ClassId* mClassId;
	static unsigned mProByteSize;
	virtual void init();
private:
};

#endif /* MOUSEBUTTONSTATECHANGEEVENT_H_ */
