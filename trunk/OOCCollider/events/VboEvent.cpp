/**
 * @file	VboEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	VboEvent class definition.
 */

#include "VboEvent.h"

#include <iostream>
#include <cstring>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "ClassId.h"
#include "IEvent.h"

using namespace std;

oocframework::ClassId* VboEvent::mClassId = new oocframework::ClassId("VboEvent");

VboEvent::VboEvent() {
	mPriByteSize = sizeof(unsigned);
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = 0;
	init();
}

VboEvent::VboEvent(const ooctools::IndexedVbo* vbo, uint64_t nodeId)
{
	unsigned iCount = vbo->getIndexCount();
	unsigned vCount = vbo->getVertexCount();
	mPriByteSize = sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount + sizeof(float)*vCount*4 + sizeof(char)*vCount*4;
	mProData = new char[mPriByteSize];

	((uint64_t*)mProData)[0] = nodeId;
	((unsigned*)(mProData+sizeof(uint64_t)))[0] = iCount;
	((unsigned*)(mProData+sizeof(uint64_t)))[1] = vCount;
	memcpy((mProData+sizeof(uint64_t)+sizeof(unsigned)*2), vbo->getIndexData(), sizeof(unsigned)*iCount);
	memcpy((mProData+sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount), vbo->getVertexData(), sizeof(float)*vCount*4 + sizeof(char)*vCount*4);
}

VboEvent::VboEvent(std::string path, uint64_t nodeId)
{
	unsigned iCount = 0;
	unsigned vCount = 0;
	fs::ifstream idxFile;
	fs::ifstream vertexFile;
	idxFile.open(fs::path(path + ".idx"), ios::binary);
	idxFile.seekg(0, ios::beg);
	idxFile.read((char*)&iCount, sizeof(unsigned));
	idxFile.seekg(sizeof(unsigned), ios::beg);

	vertexFile.open(fs::path(path + ".data"), ios::binary);
	vertexFile.seekg(0, ios::beg);
	vertexFile.read((char*)&vCount, sizeof(unsigned));
	vertexFile.seekg(sizeof(unsigned), ios::beg);

	mPriByteSize = sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount + sizeof(float)*vCount*4 + sizeof(char)*vCount*4;
	mProData = new char[mPriByteSize];
	((uint64_t*)mProData)[0] = nodeId;
	((unsigned*)(mProData+sizeof(uint64_t)))[0] = iCount;
	((unsigned*)(mProData+sizeof(uint64_t)))[1] = vCount;
	idxFile.read((mProData+sizeof(uint64_t)+sizeof(unsigned)*2), sizeof(unsigned)*iCount);
	vertexFile.read((mProData+sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount), sizeof(float)*vCount*4 + sizeof(char)*vCount*4);

	idxFile.close();
	vertexFile.close();
}

VboEvent::VboEvent(const Message* msg){
	mPriByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mPriByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());

}

VboEvent::~VboEvent() {
	delete[] mProData;
}

ooctools::IndexedVbo* VboEvent::createIVbo() const
{
	return 0;
}

const oocframework::ClassId* VboEvent::classid()
{
	return mClassId;
}

void VboEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("VboEvent");
	}
}

bool
VboEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}