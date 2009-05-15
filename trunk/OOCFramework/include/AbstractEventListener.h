/**
 * @file	AbstractEventListener.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2009
 * @brief 	AbstractEventListener class declaration.
 */

#ifndef ABSTRACTEVENTLISTENER_H_
#define ABSTRACTEVENTLISTENER_H_

#include "declarations.h"

namespace oocframework {

/**
 * @class AbstractEventListener
 * @brief As the name suggests, this class needs to be inherited if
 * you wish to receive any events.
 */
class AbstractEventListener {
public:
	AbstractEventListener(){};
	virtual ~AbstractEventListener(){};

	/**
	 * @brief The fundamental element of event-reception.
	 * In this implementation you have to check for the correct instance
	 * of the given event. For example:
	 * @code
	 * if (event.instanceOf(ResizeWindowEvent::classid()))
	 * {
	 * 	ResizeWindowEvent& rwe = (ResizeWindowEvent&) event;
	 * 	...
	 * }
	 * @endcode
	 * @param event
	 */
	virtual void notify(IEvent& event) = 0;
};

}

#endif /* ABSTRACTEVENTLISTENER_H_ */
