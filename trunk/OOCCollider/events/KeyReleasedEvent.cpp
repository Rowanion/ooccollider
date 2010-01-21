/*
 * KeyReleasedEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "KeyReleasedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* KeyReleasedEvent::mClassId = new oocframework::ClassId("KeyReleasedEvent");

KeyReleasedEvent::KeyReleasedEvent()
{
	mProByteSize = sizeof(int);
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = 0;
	init();
}

KeyReleasedEvent::KeyReleasedEvent(int key, char _lShift, char _rShift, char _lCtrl, char _rCtrl, char _lAlt, char _rAlt)
{
	mProByteSize = sizeof(int) + sizeof(char)*6;
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = key;
	((char*)(mProData + sizeof(int)))[0] = _lShift;
	((char*)(mProData + sizeof(int)))[1] = _rShift;
	((char*)(mProData + sizeof(int)))[2] = _lCtrl;
	((char*)(mProData + sizeof(int)))[3] = _rCtrl;
	((char*)(mProData + sizeof(int)))[4] = _lAlt;
	((char*)(mProData + sizeof(int)))[5] = _rAlt;
}

KeyReleasedEvent::KeyReleasedEvent(const oocframework::Message* msg){
	mProByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());

}

KeyReleasedEvent::~KeyReleasedEvent() {
	delete[] mProData;
}

bool
KeyReleasedEvent::isLShift()
{
	return (bool)((char*)(mProData + sizeof(int)))[0];
}

bool KeyReleasedEvent::isRShift()
{
	return (bool)((char*)(mProData + sizeof(int)))[1];
}

bool KeyReleasedEvent::isShift()
{
	return (bool)((char*)(mProData + sizeof(int)))[0] || (bool)((char*)(mProData + sizeof(int)))[1];
}

bool KeyReleasedEvent::isLCtrl()
{
	return (bool)((char*)(mProData + sizeof(int)))[2];
}

bool KeyReleasedEvent::isRCtrl()
{
	return (bool)((char*)(mProData + sizeof(int)))[3];
}

bool KeyReleasedEvent::isCtrl()
{
	return (bool)((char*)(mProData + sizeof(int)))[2] || (bool)((char*)(mProData + sizeof(int)))[3];
}

bool KeyReleasedEvent::isLAlt()
{
	return (bool)((char*)(mProData + sizeof(int)))[4];
}

bool KeyReleasedEvent::isRAlt()
{
	return (bool)((char*)(mProData + sizeof(int)))[5];
}

bool KeyReleasedEvent::isAlt()
{
	return (bool)((char*)(mProData + sizeof(int)))[4] || (bool)((char*)(mProData + sizeof(int)))[5];
}

const oocframework::ClassId* KeyReleasedEvent::classid()
{
	return mClassId;
}

void KeyReleasedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("KeyReleasedEvent");
	}
}

bool
KeyReleasedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
