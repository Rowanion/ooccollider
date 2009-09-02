/*
 * MouseButtonEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "MouseButtonEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* MouseButtonEvent::mClassId = new oocframework::ClassId("MouseButtonEvent");
unsigned MouseButtonEvent::mProByteSize = sizeof(int)*4;

MouseButtonEvent::MouseButtonEvent() {
	mProData = new char[MouseButtonEvent::mProByteSize];
	((int*)mProData)[0] = 0;
	((int*)mProData)[1] = 0;
	((int*)mProData)[2] = 0;
	((int*)mProData)[3] = 0;
	init();
}

MouseButtonEvent::MouseButtonEvent(int x, int y, int state, int button) {
	mProData = new char[MouseButtonEvent::mProByteSize];
	((int*)mProData)[0] = x;
	((int*)mProData)[1] = y;
	((int*)mProData)[2] = state;
	((int*)mProData)[3] = button;
	init();
}

MouseButtonEvent::~MouseButtonEvent() {
	delete[] mProData;
}

const oocframework::ClassId* MouseButtonEvent::classid()
{
	return mClassId;
}

void MouseButtonEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("MouseButtonEvent");
	}
}

bool
MouseButtonEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
