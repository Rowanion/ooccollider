/*
 * KeyPressedEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "KeyPressedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* KeyPressedEvent::mClassId = new oocframework::ClassId("KeyPressedEvent");
unsigned KeyPressedEvent::mProByteSize = sizeof(int);

KeyPressedEvent::KeyPressedEvent() {
	mProData = new char[KeyPressedEvent::mProByteSize];
	((int*)mProData)[0] = 0;
	init();
}

KeyPressedEvent::KeyPressedEvent(int key) {
	mProData = new char[KeyPressedEvent::mProByteSize];
	((int*)mProData)[0] = key;
}

KeyPressedEvent::~KeyPressedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* KeyPressedEvent::classid()
{
	return mClassId;
}

void KeyPressedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("KeyPressedEvent");
	}
}

bool
KeyPressedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
