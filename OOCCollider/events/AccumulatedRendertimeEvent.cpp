/*
 * AccumulatedRendertimeEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "AccumulatedRendertimeEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* AccumulatedRendertimeEvent::mClassId = new oocframework::ClassId("AccumulatedRendertimeEvent");

AccumulatedRendertimeEvent::AccumulatedRendertimeEvent() {
	mProByteSize = sizeof(double);
	mProData = new char[mProByteSize];
	init();
}

AccumulatedRendertimeEvent::AccumulatedRendertimeEvent(double renderTime)
{
	mProByteSize = sizeof(double);
	mProData = new char[mProByteSize];
	init();

	((double*)mProData)[0] = renderTime;
}

AccumulatedRendertimeEvent::~AccumulatedRendertimeEvent() {
	delete[] mProData;
}

const oocframework::ClassId* AccumulatedRendertimeEvent::classid()
{
	return mClassId;
}

void AccumulatedRendertimeEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("AccumulatedRendertimeEvent");
	}
}

bool
AccumulatedRendertimeEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
