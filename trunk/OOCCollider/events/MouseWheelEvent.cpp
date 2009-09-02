/*
 * MouseWheelEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "MouseWheelEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* MouseWheelEvent::mClassId = new oocframework::ClassId("MouseWheelEvent");
unsigned MouseWheelEvent::mProByteSize = sizeof(int);

MouseWheelEvent::MouseWheelEvent() {
	mProData = new char[MouseWheelEvent::mProByteSize];
	((Direction*)mProData)[0] = STILL;
	init();
}

MouseWheelEvent::MouseWheelEvent(Direction _direction) {
	mProData = new char[sizeof(int)];
	((Direction*)mProData)[0] = _direction;
	init();
}

MouseWheelEvent::~MouseWheelEvent() {
	delete[] mProData;
}

const oocframework::ClassId* MouseWheelEvent::classid()
{
	return mClassId;
}

void MouseWheelEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("MouseWheelEvent");
	}
}

bool
MouseWheelEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
