/*
 * ChangeTileDimensionsEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "ChangeTileDimensionsEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"
#include "Message.h"

oocframework::ClassId* ChangeTileDimensionsEvent::mClassId = new oocframework::ClassId("ChangeTileDimensionsEvent");
unsigned ChangeTileDimensionsEvent::mProByteSize = 4*sizeof(int);

ChangeTileDimensionsEvent::ChangeTileDimensionsEvent() {
	mProData = new char[mProByteSize];
	init();
}

ChangeTileDimensionsEvent::ChangeTileDimensionsEvent(int xPos, int yPos, int width, int height)
{
	mProData = new char[mProByteSize];
	init();

	((int*)mProData)[0] = xPos;
	((int*)mProData)[1] = yPos;
	((int*)mProData)[2] = width;
	((int*)mProData)[3] = height;
}

ChangeTileDimensionsEvent::ChangeTileDimensionsEvent(Tile& t)
{
	mProData = new char[mProByteSize];
	init();

	((int*)(mProData))[0] = t.xPos;
	((int*)(mProData))[1] = t.yPos;
	((int*)(mProData))[2] = t.width;
	((int*)(mProData))[3] = t.height;
}

ChangeTileDimensionsEvent::ChangeTileDimensionsEvent(const Message* msg)
{
	mProData = new char[mProByteSize];
	memcpy(mProData, msg->getData(),msg->getLength());
}
ChangeTileDimensionsEvent::~ChangeTileDimensionsEvent() {
	delete[] mProData;
}

void ChangeTileDimensionsEvent::setTileDimension(Tile& t)
{
	((int*)(mProData))[0] = t.xPos;
	((int*)(mProData))[1] = t.yPos;
	((int*)(mProData))[2] = t.width;
	((int*)(mProData))[3] = t.height;
}

const oocframework::ClassId* ChangeTileDimensionsEvent::classid()
{
	return mClassId;
}

void ChangeTileDimensionsEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ChangeTileDimensionsEvent");
	}
}

bool
ChangeTileDimensionsEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
