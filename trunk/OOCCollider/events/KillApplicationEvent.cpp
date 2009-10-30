/*
 * KillApplicationEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "KillApplicationEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* KillApplicationEvent::mClassId = new oocframework::ClassId("KillApplicationEvent");

KillApplicationEvent::KillApplicationEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

KillApplicationEvent::~KillApplicationEvent() {
	delete[] mProData;
}

const oocframework::ClassId* KillApplicationEvent::classid()
{
	return mClassId;
}

void KillApplicationEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("KillApplicationEvent");
	}
}

bool
KillApplicationEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
