/*
 * EndOfFrameEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "EndOfFrameEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* EndOfFrameEvent::mClassId = new oocframework::ClassId("EndOfFrameEvent");

EndOfFrameEvent::EndOfFrameEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

EndOfFrameEvent::~EndOfFrameEvent() {
	delete[] mProData;
}

const oocframework::ClassId* EndOfFrameEvent::classid()
{
	return mClassId;
}

void EndOfFrameEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("EndOfFrameEvent");
	}
}

bool
EndOfFrameEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
