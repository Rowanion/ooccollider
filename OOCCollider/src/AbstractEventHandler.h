/**
 * @file	AbstractEventHandler.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 03.05.2009
 * @brief 	AbstractEventHandler class declaration.
 */


#ifndef ABSTRACTEVENTHANDLER_H_
#define ABSTRACTEVENTHANDLER_H_

#include "declarations.h"

// Events can be GLFW key constants, or one of the following. The
// state argument to the Event function has a straightforward meaning
// - for keys and mouse buttons it indicates whether the button was
// pushed or released, for EVENT_WINDOW_ACTIVE it indicates whether
// the window was activated or deactivated. For EVENT_MOUSE_MOVED it
// has no meaning. For EVENT_MOUSEWHEEL_MOVED true means UP and false
// means DOWN.
#define EVENT_MOUSE_LEFT GLFW_KEY_LAST + 1
#define EVENT_MOUSE_RIGHT GLFW_KEY_LAST + 2
#define EVENT_MOUSE_MIDDLE GLFW_KEY_LAST + 3
#define EVENT_WINDOW_ACTIVE GLFW_KEY_LAST + 5
#define EVENT_WINDOW_CLOSED GLFW_KEY_LAST + 6
#define EVENT_MOUSE_MOVED GLFW_KEY_LAST + 7
#define EVENT_MOUSEWHEEL_MOVED GLFW_KEY_LAST + 8
#define EVENT_WINDOW_RESIZED GLFW_KEY_LAST + 9

/**
 * @class Abstract_Event_Handler
 * @brief A base class for objects that handle window events.
 */
class AbstractEventHandler {
public:
	// Always give classes with virtual functions a virtual constructor.
	virtual ~AbstractEventHandler() {
	}
	virtual void Event(int event, bool state) = 0;
};

#endif /* ABSTRACTEVENTHANDLER_H_ */
