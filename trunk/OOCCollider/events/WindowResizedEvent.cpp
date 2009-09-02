/*
 * ResizeWindowEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "WindowResizedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* WindowResizedEvent::mClassId = new oocframework::ClassId("WindowResizedEvent");
unsigned WindowResizedEvent::mProByteSize = sizeof(int)*2;

WindowResizedEvent::WindowResizedEvent() {
	mProData = new char[WindowResizedEvent::mProByteSize];
	((int*)mProData)[0] = 0;
	((int*)mProData)[1] = 0;
	init();
}

WindowResizedEvent::WindowResizedEvent(int width, int height) {
	mProData = new char[WindowResizedEvent::mProByteSize];
	((int*)mProData)[0] = width;
	((int*)mProData)[1] = height;
	init();
}

WindowResizedEvent::~WindowResizedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* WindowResizedEvent::classid()
{
	return mClassId;
}

void WindowResizedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ResizeWindowEvent");
	}
}

bool
WindowResizedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
