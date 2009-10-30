/**
 * @file	JobDoneEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	JobDoneEvent class definition.
 */


#include "JobDoneEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* JobDoneEvent::mClassId = new oocframework::ClassId("JobDoneEvent");

JobDoneEvent::JobDoneEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

JobDoneEvent::~JobDoneEvent() {
	delete[] mProData;
}

const oocframework::ClassId* JobDoneEvent::classid()
{
	return mClassId;
}

void JobDoneEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("JobDoneEvent");
	}
}

bool
JobDoneEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
