/**
 * @file	TestEventHandler.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 03.05.2009
 * @brief 	TestEventHandler class declaration.
 */

#ifndef TESTEVENTHANDLER_H_
#define TESTEVENTHANDLER_H_

#include "declarations.h"
#include "AbstractEventHandler.h"

class TestEventHandler: public AbstractEventHandler {
public:
	TestEventHandler();
	virtual ~TestEventHandler();
	virtual void Event(int event, bool state);
};

#endif /* TESTEVENTHANDLER_H_ */
