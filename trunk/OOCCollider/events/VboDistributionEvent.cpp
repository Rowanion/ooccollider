/**
 * @file	VboDistributionEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	VboDistributionEvent class definition.
 */

#include "VboDistributionEvent.h"

#include <iostream>
#include <cstring>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* VboDistributionEvent::mClassId = new oocframework::ClassId("VboDistributionEvent");

VboDistributionEvent::VboDistributionEvent() {
	mProData = 0;
	init();
}

VboDistributionEvent::VboDistributionEvent(const std::set<uint64_t>& _idSet)
{
	unsigned setSize = _idSet.size();
	mProByteSize = sizeof(uint64_t) + sizeof(uint64_t)*setSize;
	mProData = new char[mProByteSize];
	((uint64_t*)mProData)[0] = setSize;
	unsigned count = 0;
	std::set<uint64_t>::iterator it = _idSet.begin();
	for (; it != _idSet.end(); ++it, ++count){
		((uint64_t*)(mProData+sizeof(uint64_t)))[count] = *it;
	}
}

VboDistributionEvent::VboDistributionEvent(const oocframework::Message* msg)
{
	mProByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());



//	std::cout << "w: " << ((int*)dat)[0] << std::endl;
//	std::cout << "h: " << ((int*)dat)[1] << std::endl;
//	std::cout << "x: " << ((int*)dat)[2] << std::endl;
//	std::cout << "y: " << ((int*)dat)[3] << std::endl;
//	std::cout << (int)((unsigned char*)(dat+4*sizeof(int)))[0] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[1] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[2] << std::endl;

}

VboDistributionEvent::~VboDistributionEvent() {
	delete[] mProData;
}

const oocframework::ClassId* VboDistributionEvent::classid()
{
	return mClassId;
}

void VboDistributionEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ResizeWindowEvent");
	}
}

bool
VboDistributionEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
