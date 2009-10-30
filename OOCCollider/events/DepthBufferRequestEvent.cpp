/*
 * DepthBufferRequestEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "DepthBufferRequestEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* DepthBufferRequestEvent::mClassId = new oocframework::ClassId("DepthBufferRequestEvent");

DepthBufferRequestEvent::DepthBufferRequestEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

DepthBufferRequestEvent::~DepthBufferRequestEvent() {
	delete[] mProData;
}

const oocframework::ClassId* DepthBufferRequestEvent::classid()
{
	return mClassId;
}

void DepthBufferRequestEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("DepthBufferRequestEvent");
	}
}

bool
DepthBufferRequestEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
