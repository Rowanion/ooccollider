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
#include <cmath>

#include "ClassId.h"
#include "IEvent.h"

using namespace std;

oocframework::ClassId* NodeRequestEvent::mClassId = new oocframework::ClassId("NodeRequestEvent");

NodeRequestEvent::NodeRequestEvent() {
	mPriByteSize = sizeof(unsigned);
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = 0;
	init();
}

NodeRequestEvent::NodeRequestEvent(const std::set<ooctools::Quintuple>& quintSet, int recipient, bool isExtendedFrustum)
{
	unsigned setSize = quintSet.size();
	unsigned minSize = setSize;
	std::set<ooctools::Quintuple>::const_iterator quintIt;

	// #of nodes, recipientId, isExtFrus, distance of each node, id of each node
	mPriByteSize = sizeof(unsigned) + sizeof(int) + sizeof(bool) + minSize*(sizeof(ooctools::Quintuple));
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = minSize;
	((int*)(mProData+sizeof(unsigned)))[0] = recipient;
	((bool*)(mProData+sizeof(unsigned)+sizeof(int)))[0] = isExtendedFrustum;
	unsigned elementCount = 0;
//	std::cout << "list of node-requests inside the Event: " << std::endl;
	for (quintIt = quintSet.begin(); quintIt!= quintSet.end(); ++quintIt){
//		std::cout << mapIt->second << std::endl;
		((ooctools::Quintuple*)(mProData+sizeof(unsigned)+sizeof(int)+sizeof(bool)))[elementCount] = *quintIt;
//		std::cout << ((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)))[elementCount] << std::endl;

		elementCount++;
	}
	init();
}

NodeRequestEvent::NodeRequestEvent(const oocframework::Message* msg){

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
