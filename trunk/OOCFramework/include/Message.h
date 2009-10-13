/**
 * @file	Message.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 07.05.2009
 * @brief 	Message class declaration.
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "declarations.h"
#include "MpiControl.h"
#include "IEvent.h"

namespace oocframework{

class Message {

friend class MpiControl;

public:
	Message();
	Message(int _type, unsigned int _length, int dst, const char* _data, MpiControl::Group _group = MpiControl::DEFAULT);
	Message(oocframework::IEvent& event, int _dst, MpiControl::Group _group = MpiControl::DEFAULT);
	Message(const Message& _msg);
	virtual ~Message();
	int getType() const {return mType;};
	unsigned getUType() const {return *((unsigned*)&mType);};
	void setType(int _type) {mType = _type;};
	unsigned int getLength() const {return mLength;};
	void setLength(unsigned int _length) {mLength = _length;};
	int getDst() const {return mDst;};
	void setDst(int _dst) {mDst = _dst;};
	const char* getData() const {return mData;};
	void setData(char* _data) {mData = _data;};
	void setSrc(int src) {mSrc = src;};
	int getSrc() const {return mSrc;};
	void setGroup(MpiControl::Group _group) {mGroup = _group;};
	MpiControl::Group getGroup() const {return mGroup;};

private:
	int mType;
	unsigned int mLength;
	int mDst;
	char* mData;
	int mSrc;
	MPI::Request request;
	MpiControl::Group mGroup;

	inline char* getDataHandle() {return mData;};
};

} // oocframework
#endif /* MESSAGE_H_ */
