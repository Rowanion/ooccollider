/*
 * ResultsEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "ResultsEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"
#include "Message.h"

oocframework::ClassId* ResultsEvent::mClassId = new oocframework::ClassId("ResultsEvent");

ResultsEvent::ResultsEvent(unsigned _count, double* _array)
{
	mProByteSize = sizeof(unsigned) + (sizeof(double)*_count);
	mProData = new char[mProByteSize];
	((unsigned*)mProData)[0] = _count;
	memcpy(((double*)(mProData+sizeof(unsigned))), _array, sizeof(double)*_count);
	init();
}

ResultsEvent::ResultsEvent(const oocframework::Message* msg)
{
	mProByteSize = msg->getLength();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());
}

ResultsEvent::~ResultsEvent() {
	delete[] mProData;
}

const oocframework::ClassId* ResultsEvent::classid()
{
	return mClassId;
}

void ResultsEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ResultsEvent");
	}
}

bool
ResultsEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
