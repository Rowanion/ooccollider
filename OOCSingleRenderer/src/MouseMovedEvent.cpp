/*
 * MouseDraggedEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "MouseMovedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* MouseMovedEvent::mClassId = new oocframework::ClassId("MouseMovedEvent");

MouseMovedEvent::MouseMovedEvent() {
	mProByteSize = sizeof(int)*2;
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = 0;
	((int*)mProData)[1] = 0;
	init();
}

MouseMovedEvent::MouseMovedEvent(int x, int y) {
	mProByteSize = sizeof(int)*2;
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = x;
	((int*)mProData)[1] = y;
	init();
}

MouseMovedEvent::~MouseMovedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* MouseMovedEvent::classid()
{
	return mClassId;
}

void MouseMovedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("MouseMovedEvent");
	}
}

bool
MouseMovedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
