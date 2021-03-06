/**
 * @file	OcclusionRequestEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	OcclusionRequestEvent class definition.
 */

#include "OcclusionRequestEvent.h"

#include <iostream>
#include <cstring>
#include <cmath>

#include "ClassId.h"
#include "IEvent.h"


using namespace std;

oocframework::ClassId* OcclusionRequestEvent::mClassId = new oocframework::ClassId("OcclusionRequestEvent");

//OcclusionRequestEvent::OcclusionRequestEvent() {
//	mPriByteSize = sizeof(unsigned);
//	mProData = new char[mPriByteSize];
//	((unsigned*)mProData)[0] = 0;
//	init();
//}

OcclusionRequestEvent::OcclusionRequestEvent(const OcclusionRequestEvent& _nre)
{
	mProByteSize = _nre.getByteSize();
	mProData = new char[mProByteSize];

	memcpy(mProData, _nre.getData(), mProByteSize);
}

OcclusionRequestEvent::OcclusionRequestEvent(const std::set<ooctools::Quintuple>& quintSet)
{
	unsigned setSize = quintSet.size();
	std::set<ooctools::Quintuple>::const_iterator quintIt;

	// #of nodes, recipientId, isExtFrus, distance of each node, id of each node
	mProByteSize = sizeof(unsigned) + setSize*(sizeof(ooctools::Quintuple)) + sizeof(float)*16;
	mProData = new char[mProByteSize];
	((unsigned*)mProData)[0] = setSize;
	unsigned elementCount = 0;
//	std::cout << "list of node-requests inside the Event: " << std::endl;
	for (quintIt = quintSet.begin(); quintIt!= quintSet.end(); ++quintIt){
//		std::cout << mapIt->second << std::endl;
		((ooctools::Quintuple*)(mProData+sizeof(unsigned)))[elementCount] = *quintIt;
//		std::cout << ((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)))[elementCount] << std::endl;

		elementCount++;
	}
	init();
}

OcclusionRequestEvent::OcclusionRequestEvent(const std::set<ooctools::Quintuple>& _quintSet, const float* _matrix)
{
	unsigned setSize = _quintSet.size();
	std::set<ooctools::Quintuple>::const_iterator quintIt;

	// #of nodes, recipientId, isExtFrus, distance of each node, id of each node
	mProByteSize = sizeof(unsigned) + setSize*(sizeof(ooctools::Quintuple)) + sizeof(float)*16;
	mProData = new char[mProByteSize];
	((unsigned*)mProData)[0] = setSize;
	unsigned elementCount = 0;
//	std::cout << "list of node-requests inside the Event: " << std::endl;
	for (quintIt = _quintSet.begin(); quintIt!= _quintSet.end(); ++quintIt){
//		std::cout << mapIt->second << std::endl;
		((ooctools::Quintuple*)(mProData+sizeof(unsigned)))[elementCount] = *quintIt;
//		std::cout << ((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)))[elementCount] << std::endl;

		elementCount++;
	}
	memcpy(((float*)(mProData + sizeof(unsigned) + sizeof(ooctools::Quintuple)*setSize)), _matrix, sizeof(float)*16);
	init();
}

OcclusionRequestEvent::OcclusionRequestEvent(std::set<ooctools::Quintuple>::iterator _begin, std::set<ooctools::Quintuple>::iterator _end, unsigned int _count)
{
	mProByteSize = sizeof(unsigned) + _count*(sizeof(ooctools::Quintuple) + sizeof(float)*16);
	mProData = new char[mProByteSize];

	std::set<ooctools::Quintuple>::const_iterator quintIt;

	// #of nodes, recipientId, isExtFrus, distance of each node, id of each node
	((unsigned*)mProData)[0] = _count;
	unsigned elementCount = 0;
//	std::cout << "list of node-requests inside the Event: " << std::endl;
	for (quintIt = _begin; quintIt!= _end; ++quintIt){
//		std::cout << mapIt->second << std::endl;
		((ooctools::Quintuple*)(mProData+sizeof(unsigned)))[elementCount] = *quintIt;
//		std::cout << ((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)))[elementCount] << std::endl;

		elementCount++;
	}
	init();
}

OcclusionRequestEvent::OcclusionRequestEvent(const oocframework::Message* msg){

	mProByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());

}

OcclusionRequestEvent::~OcclusionRequestEvent() {
	delete[] mProData;
}

const OcclusionRequestEvent& OcclusionRequestEvent::operator=(const OcclusionRequestEvent& _rhs)
{
	if (this == &_rhs)
		return *this;
	else{
		if (mProByteSize != 0){
			delete[] mProData;
			mProData = 0;
		}
		mProByteSize = _rhs.getLength();
		mProData = new char[mProByteSize];
		memcpy(mProData, _rhs.getData(), mProByteSize);
		return *this;
	}
}

const oocframework::ClassId* OcclusionRequestEvent::classid()
{
	return mClassId;
}

void OcclusionRequestEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("OcclusionRequestEvent");
	}
}

bool
OcclusionRequestEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
