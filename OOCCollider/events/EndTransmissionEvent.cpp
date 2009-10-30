/*
 * EndTransmissionEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "EndTransmissionEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* EndTransmissionEvent::mClassId = new oocframework::ClassId("EndTransmissionEvent");

EndTransmissionEvent::EndTransmissionEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

EndTransmissionEvent::~EndTransmissionEvent() {
	delete[] mProData;
}

const oocframework::ClassId* EndTransmissionEvent::classid()
{
	return mClassId;
}

void EndTransmissionEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("EndTransmissionEvent");
	}
}

bool
EndTransmissionEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
