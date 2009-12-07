/*
 * ClearCacheEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "ClearCacheEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* ClearCacheEvent::mClassId = new oocframework::ClassId("ClearCacheEvent");

ClearCacheEvent::ClearCacheEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

ClearCacheEvent::~ClearCacheEvent() {
	delete[] mProData;
}

const oocframework::ClassId* ClearCacheEvent::classid()
{
	return mClassId;
}

void ClearCacheEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ClearCacheEvent");
	}
}

bool
ClearCacheEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
