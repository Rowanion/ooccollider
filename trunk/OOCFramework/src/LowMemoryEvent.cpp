/*
 * LowMemoryEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "LowMemoryEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

namespace oocframework{

oocframework::ClassId* LowMemoryEvent::mClassId = new oocframework::ClassId("LowMemoryEvent");

LowMemoryEvent::LowMemoryEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

LowMemoryEvent::~LowMemoryEvent() {
	delete[] mProData;
}

const oocframework::ClassId* LowMemoryEvent::classid()
{
	return mClassId;
}

void LowMemoryEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("LowMemoryEvent");
	}
}

bool
LowMemoryEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}

}
