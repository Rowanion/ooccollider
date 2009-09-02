/**
 * @file	NodeRequestEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	NodeRequestEvent class definition.
 */

#include "NodeRequestEvent.h"

#include <iostream>
#include <cstring>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* NodeRequestEvent::mClassId = new oocframework::ClassId("NodeRequestEvent");

NodeRequestEvent::NodeRequestEvent() {
	mPriByteSize = sizeof(unsigned);
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = 0;
	init();
}

NodeRequestEvent::NodeRequestEvent(const std::multimap<float, uint64_t>& idMap, int recipient) {

	unsigned mapSize = idMap.size();
	std::multimap<float, uint64_t>::const_iterator mapIt;



	mPriByteSize = sizeof(unsigned) + sizeof(int) + (sizeof(float)*mapSize) + (sizeof(uint64_t)*mapSize);
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = mapSize;
	((int*)(mProData+sizeof(unsigned)))[0] = recipient;
	unsigned elementCount = 0;
//	std::cout << "list of node-requests inside the Event: " << std::endl;
	for (mapIt = idMap.begin(); mapIt!= idMap.end(); ++mapIt){
//		std::cout << mapIt->second << std::endl;
		((float*)(mProData+sizeof(unsigned)+sizeof(int)))[elementCount] = mapIt->first;
		((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)+(sizeof(float)*mapSize)))[elementCount] = mapIt->second;
//		std::cout << ((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)))[elementCount] << std::endl;

		elementCount++;
	}
	init();
}

NodeRequestEvent::NodeRequestEvent(const Message* msg){
	mPriByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mPriByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());

}

NodeRequestEvent::~NodeRequestEvent() {
	delete[] mProData;
}

const oocframework::ClassId* NodeRequestEvent::classid()
{
	return mClassId;
}

void NodeRequestEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("NodeRequestEvent");
	}
}

bool
NodeRequestEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
