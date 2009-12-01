/**OcculsionResults
 * @file	OcclusionResultsEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	OcclusionResultsEvent class definition.
 */

#include "OcclusionResultsEvent.h"

#include <iostream>
#include <cstring>
#include <cmath>
#include <utility>

#include "ClassId.h"
#include "IEvent.h"


using namespace std;

oocframework::ClassId* OcclusionResultsEvent::mClassId = new oocframework::ClassId("OcclusionResultsEvent");

//OcclusionResultsEvent::OcclusionResultsEvent() {
//	mPriByteSize = sizeof(unsigned);
//	mProData = new char[mPriByteSize];
//	((unsigned*)mProData)[0] = 0;
//	init();
//}

OcclusionResultsEvent::OcclusionResultsEvent(const OcclusionResultsEvent& _ore)
{
	mProByteSize = _ore.getByteSize();
	mProData = new char[mProByteSize];

	memcpy(mProData, _ore.getData(), mProByteSize);
}

OcclusionResultsEvent::OcclusionResultsEvent(const std::list<ooctools::Visibility>& _pairList)
{
	unsigned listSize = _pairList.size();
	mProByteSize = listSize*sizeof(uint64_t) + listSize*sizeof(char);
	mProData = new char[mProByteSize];

	((unsigned*)(mProData))[0] = listSize;
	list<ooctools::Visibility>::const_iterator vIt = _pairList.begin();
	for (unsigned i=0; vIt != _pairList.end(); vIt++, i++){
		((uint64_t*)(mProData + sizeof(unsigned) + sizeof(uint64_t)*i + sizeof(char)*i))[0] = vIt->id;
		((char*)(mProData + sizeof(unsigned) + sizeof(uint64_t)*(i+1) + sizeof(char)*i))[0] = vIt->visible;
	}
}

OcclusionResultsEvent::OcclusionResultsEvent(const oocframework::Message* msg){

	mProByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mProByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());

}

OcclusionResultsEvent::~OcclusionResultsEvent() {
	delete[] mProData;
}

const oocframework::ClassId* OcclusionResultsEvent::classid()
{
	return mClassId;
}

void OcclusionResultsEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("OcclusionResultsEvent");
	}
}

bool
OcclusionResultsEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
