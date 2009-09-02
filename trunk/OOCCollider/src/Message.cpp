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

Message::Message() :
	mType(0), mLength(0), mDst(0), mData(0) {
	// TODO Auto-generated constructor stub
}

Message::Message(int _type, unsigned int _length, int _dst, const char* _data) :
	mType(_type), mLength(_length), mDst(_dst), mData(new char[_length]) {
	memcpy(mData, _data, _length);
	mSrc = MpiControl::getSingleton()->getRank();
}
Message::Message(oocframework::IEvent& event, int _dst) :
	mType(event.getClassId()->getShortId()), mLength(event.getByteSize()), mDst(_dst), mData(0)
{
	mData = new char[event.getByteSize()];
	memcpy(mData, event.getData(), mLength);
	mSrc = MpiControl::getSingleton()->getRank();
}
Message::~Message()
{
	delete[] mData;
	// TODO Auto-generated destructor stub
}
