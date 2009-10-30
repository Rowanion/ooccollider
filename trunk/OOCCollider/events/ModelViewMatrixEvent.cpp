/*
 * ModelViewMatrixEvent.cpp
 *
 *  Created on: 12.05.2009
 *      Author: ava
 */

#include "ModelViewMatrixEvent.h"

#include <iostream>
#include <cstring>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* ModelViewMatrixEvent::mClassId = new oocframework::ClassId("ModelViewMatrixEvent");

ModelViewMatrixEvent::ModelViewMatrixEvent(){
	mProByteSize = sizeof(float)*16;
	mProData = new char[mProByteSize];
	((float*)mProData)[0] = 1;
	((float*)mProData)[1] = 0;
	((float*)mProData)[2] = 0;
	((float*)mProData)[3] = 0;
	((float*)mProData)[4] = 0;
	((float*)mProData)[5] = 1;
	((float*)mProData)[6] = 0;
	((float*)mProData)[7] = 0;
	((float*)mProData)[8] = 0;
	((float*)mProData)[9] = 0;
	((float*)mProData)[10] = 1;
	((float*)mProData)[11] = 0;
	((float*)mProData)[12] = 0;
	((float*)mProData)[13] = 0;
	((float*)mProData)[14] = 0;
	((float*)mProData)[15] = 1;
	init();
}

ModelViewMatrixEvent::ModelViewMatrixEvent(const float* matrix){
	mProByteSize = sizeof(float)*16;
	mProData = new char[mProByteSize];
	memcpy(mProData, matrix, mProByteSize);
	init();
}

ModelViewMatrixEvent::ModelViewMatrixEvent(const oocframework::Message* msg)
{
	mProByteSize = sizeof(float)*16;
	mProData = new char[mProByteSize];
	memcpy(mProData, msg->getData(),msg->getLength());
}

ModelViewMatrixEvent::~ModelViewMatrixEvent() {
	delete[] mProData;
}

const oocframework::ClassId* ModelViewMatrixEvent::classid()
{
	return mClassId;
}

void ModelViewMatrixEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ModelViewMatrixEvent");
	}
}

bool
ModelViewMatrixEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
