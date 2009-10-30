/*
 * MouseButtonStateChangeEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "MouseButtonStateChangeEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* MouseButtonStateChangeEvent::mClassId = new oocframework::ClassId("MouseButtonStateChangeEvent");

MouseButtonStateChangeEvent::MouseButtonStateChangeEvent() {
	mProByteSize = sizeof(int)*3;
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = 0;
	((int*)mProData)[1] = 0;
	((int*)mProData)[2] = 0;
	init();
}

MouseButtonStateChangeEvent::MouseButtonStateChangeEvent(int button, int oState, int nState) {
	mProByteSize = sizeof(int)*3;
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = button;
	((int*)mProData)[1] = oState;
	((int*)mProData)[2] = nState;
	init();
}

MouseButtonStateChangeEvent::~MouseButtonStateChangeEvent() {
	delete[] mProData;
}

const oocframework::ClassId* MouseButtonStateChangeEvent::classid()
{
	return mClassId;
}

void MouseButtonStateChangeEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("MouseButtonStateChangeEvent");
	}
}

bool
MouseButtonStateChangeEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
