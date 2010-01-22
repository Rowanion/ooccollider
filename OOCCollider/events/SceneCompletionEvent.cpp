/*
 * SceneCompletionEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "SceneCompletionEvent.h"

#include <iostream>

#include "ClassId.h"
#include "IEvent.h"
#include "Message.h"

oocframework::ClassId* SceneCompletionEvent::mClassId = new oocframework::ClassId("SceneCompletionEvent");

SceneCompletionEvent::SceneCompletionEvent(int _src) {
	mProByteSize = sizeof(int);
	mProData = new char[mProByteSize];
	((int*)mProData)[0] = _src;
	init();
}

SceneCompletionEvent::SceneCompletionEvent(const oocframework::Message* msg)
{
	mProByteSize = msg->getLength();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());
}

SceneCompletionEvent::~SceneCompletionEvent() {
	delete[] mProData;
}

const oocframework::ClassId* SceneCompletionEvent::classid()
{
	return mClassId;
}

void SceneCompletionEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("SceneCompletionEvent");
	}
}

bool
SceneCompletionEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
