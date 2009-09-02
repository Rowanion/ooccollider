/*
 * WindowClosedEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "WindowClosedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* WindowClosedEvent::mClassId = new oocframework::ClassId("WindowClosedEvent");
unsigned WindowClosedEvent::mProByteSize = 0;

WindowClosedEvent::WindowClosedEvent() {
	mProData = 0;
	init();
}

WindowClosedEvent::~WindowClosedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* WindowClosedEvent::classid()
{
	return mClassId;
}

void WindowClosedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ResizeWindowEvent");
	}
}

bool
WindowClosedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
