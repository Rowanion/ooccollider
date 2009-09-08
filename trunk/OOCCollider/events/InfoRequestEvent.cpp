/*
 * InfoRequestEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "InfoRequestEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* InfoRequestEvent::mClassId = new oocframework::ClassId("InfoRequestEvent");
unsigned InfoRequestEvent::mProByteSize = 1;

InfoRequestEvent::InfoRequestEvent() {
	mProData = new char[1];
	init();
}

InfoRequestEvent::~InfoRequestEvent() {
	delete[] mProData;
}

const oocframework::ClassId* InfoRequestEvent::classid()
{
	return mClassId;
}

void InfoRequestEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("InfoRequestEvent");
	}
}

bool
InfoRequestEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
