/**
 * @file	Message.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 07.05.2009
 * @brief 	Message class definition.
 */

#include "Message.h"
#include "IEvent.h"

#include <iostream>
#include <cstring>

namespace oocframework{

Message::Message() :
	mType(0), mLength(0), mDst(0), mData(0), mGroup(MpiControl::DEFAULT) {
	// TODO Auto-generated constructor stub
}

Message::Message(int _type, unsigned int _length, int _dst, const char* _data, MpiControl::Group _group) :
	mType(_type), mLength(_length), mDst(_dst), mData(new char[_length]), mGroup(_group) {
	memcpy(mData, _data, _length);
	mSrc = MpiControl::getSingleton()->getRank();

}
Message::Message(oocframework::IEvent& event, int _dst, MpiControl::Group _group) :
	mType(event.getClassId()->getShortId()), mLength(event.getByteSize()), mDst(_dst), mData(0), mGroup(_group)
{
	mData = new char[mLength];
	memcpy(mData, event.getData(), mLength);
	mSrc = MpiControl::getSingleton()->getRank();
}

Message::Message(const Message& _msg)
{
	mLength = _msg.getLength();
	mData = new char[mLength];
	memcpy(mData, _msg.getData(), mLength);
	mSrc = _msg.getSrc();
	mDst = _msg.getDst();
	mGroup = _msg.getGroup();
	mType = _msg.getType();
}

Message::~Message()
{
	delete[] mData;
	// TODO Auto-generated destructor stub
}
} // oocframework
