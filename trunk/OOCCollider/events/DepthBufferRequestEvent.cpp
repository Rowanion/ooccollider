/*
 * DepthBufferRequestEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "DepthBufferRequestEvent.h"

#include <iostream>

oocframework::ClassId* DepthBufferRequestEvent::mClassId = new oocframework::ClassId("DepthBufferRequestEvent");

DepthBufferRequestEvent::DepthBufferRequestEvent(const float* _matrix) {
	mProByteSize = sizeof(float)*16;
	mProData = new char[mProByteSize];
	memcpy(mProData, _matrix, sizeof(float)*16);
	init();
}

DepthBufferRequestEvent::DepthBufferRequestEvent(const oocframework::Message* msg)
{
	mProByteSize = sizeof(float)*16;
	mProData = new char[mProByteSize];
	memcpy(mProData, msg->getData(),msg->getLength());
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
