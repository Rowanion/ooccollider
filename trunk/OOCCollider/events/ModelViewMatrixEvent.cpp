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
unsigned ModelViewMatrixEvent::mProByteSize = sizeof(float)*16 + sizeof(int)*4;

ModelViewMatrixEvent::ModelViewMatrixEvent(){
	mProData = new char[ModelViewMatrixEvent::mProByteSize];
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
	((int*)(mProData + sizeof(float)*16))[0] = 0;
	((int*)(mProData + sizeof(float)*16))[1] = 0;
	((int*)(mProData + sizeof(float)*16))[2] = 0;
	((int*)(mProData + sizeof(float)*16))[3] = 0;
	init();
}

ModelViewMatrixEvent::ModelViewMatrixEvent(const float* matrix, int xPos, int yPos, int width, int height){
	mProData = new char[ModelViewMatrixEvent::mProByteSize];
	memcpy(mProData, matrix, ModelViewMatrixEvent::mProByteSize);
	((int*)(mProData + sizeof(float)*16))[0] = xPos;
	((int*)(mProData + sizeof(float)*16))[1] = yPos;
	((int*)(mProData + sizeof(float)*16))[2] = width;
	((int*)(mProData + sizeof(float)*16))[3] = height;
	init();
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
