/**
 * @file	TestEventHandler.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 03.05.2009
 * @brief 	TestEventHandler class declaration.
 */

#include "TestEventHandler.h"

#include <iostream>

using namespace std;

TestEventHandler::TestEventHandler() {
	// TODO Auto-generated constructor stub

}

TestEventHandler::~TestEventHandler() {
	// TODO Auto-generated destructor stub
}

void TestEventHandler::Event(int event, bool state){
	cout << "Event fired: " << event << " with state: " << state << endl;
}
