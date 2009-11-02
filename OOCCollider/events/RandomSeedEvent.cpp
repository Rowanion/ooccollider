/**
 * @file	RandomSeedEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	RandomSeedEvent class definition.
 */


#include "RandomSeedEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"
#include "Message.h"

oocframework::ClassId* RandomSeedEvent::mClassId = new oocframework::ClassId("RandomSeedEvent");

RandomSeedEvent::RandomSeedEvent() {
	mProByteSize = sizeof(unsigned int);
	mProData = new char[mProByteSize];
	init();
}

RandomSeedEvent::RandomSeedEvent(unsigned int _seed) {
	mProByteSize = sizeof(unsigned int);
	mProData = new char[mProByteSize];
	((unsigned int*)mProData)[0] = _seed;
	init();
}

RandomSeedEvent::RandomSeedEvent(const oocframework::Message* msg)
{
	mProByteSize = msg->getLength();
	mProData = new char[mProByteSize];
	memcpy(mProData, msg->getData(),mProByteSize);
}

RandomSeedEvent::~RandomSeedEvent() {
	delete[] mProData;
}

const oocframework::ClassId* RandomSeedEvent::classid()
{
	return mClassId;
}

void RandomSeedEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("RandomSeedEvent");
	}
}

bool
RandomSeedEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
