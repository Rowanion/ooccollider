/*
 * MouseDraggedEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "MouseDraggedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* MouseDraggedEvent::mClassId = new oocframework::ClassId("MouseDraggedEvent");
unsigned MouseDraggedEvent::mProByteSize = sizeof(int)*2;

MouseDraggedEvent::MouseDraggedEvent() {
	mProData = new char[MouseDraggedEvent::mProByteSize];
	((int*)mProData)[0] = 0;
	((int*)mProData)[1] = 0;
	init();
}

MouseDraggedEvent::MouseDraggedEvent(int x, int y){
	mProData = new char[sizeof(int)];
	((int*)mProData)[0] = x;
	((int*)mProData)[1] = y;
	init();
}

MouseDraggedEvent::~MouseDraggedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* MouseDraggedEvent::classid()
{
	return mClassId;
}

void MouseDraggedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("MouseDraggedEvent");
	}
}

bool
MouseDraggedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
