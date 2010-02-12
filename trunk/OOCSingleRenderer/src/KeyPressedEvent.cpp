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

KeyPressedEvent::KeyPressedEvent()
{
	mProByteSize = sizeof(int);
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = 0;
	init();
}

KeyPressedEvent::KeyPressedEvent(int key, char _lShift, char _rShift, char _lCtrl, char _rCtrl, char _lAlt, char _rAlt)
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

KeyPressedEvent::KeyPressedEvent(const oocframework::Message* msg){
	mProByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());

}

KeyPressedEvent::~KeyPressedEvent() {
	delete[] mProData;
}

bool
KeyPressedEvent::isLShift()
{
	return (bool)((char*)(mProData + sizeof(int)))[0];
}

bool KeyPressedEvent::isRShift()
{
	return (bool)((char*)(mProData + sizeof(int)))[1];
}

bool KeyPressedEvent::isShift()
{
	return (bool)((char*)(mProData + sizeof(int)))[0] || (bool)((char*)(mProData + sizeof(int)))[1];
}

bool KeyPressedEvent::isLCtrl()
{
	return (bool)((char*)(mProData + sizeof(int)))[2];
}

bool KeyPressedEvent::isRCtrl()
{
	return (bool)((char*)(mProData + sizeof(int)))[3];
}

bool KeyPressedEvent::isCtrl()
{
	return (bool)((char*)(mProData + sizeof(int)))[2] || (bool)((char*)(mProData + sizeof(int)))[3];
}

bool KeyPressedEvent::isLAlt()
{
	return (bool)((char*)(mProData + sizeof(int)))[4];
}

bool KeyPressedEvent::isRAlt()
{
	return (bool)((char*)(mProData + sizeof(int)))[5];
}

bool KeyPressedEvent::isAlt()
{
	return (bool)((char*)(mProData + sizeof(int)))[4] || (bool)((char*)(mProData + sizeof(int)))[5];
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
