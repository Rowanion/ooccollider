/**
 * @file	CameraMovedEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	CameraMovedEvent class definition.
 */

#include "CameraMovedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* CameraMovedEvent::mClassId = new oocframework::ClassId("CameraMovedEvent");

CameraMovedEvent::CameraMovedEvent() {
	mProByteSize = 1;
	mProData = new char[mProByteSize];
	init();
}

CameraMovedEvent::~CameraMovedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* CameraMovedEvent::classid()
{
	return mClassId;
}

void CameraMovedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("CameraMovedEvent");
	}
}

bool
CameraMovedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
