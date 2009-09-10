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
using namespace ooctools;

oocframework::ClassId* VboEvent::mClassId = new oocframework::ClassId("VboEvent");

VboEvent::VboEvent() {
	mPriByteSize = sizeof(unsigned);
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = 0;
	init();
}

VboEvent::VboEvent(const ooctools::IndexedVbo* vbo) // single vbo
{
	unsigned bytePrefixSums[2];
	bytePrefixSums[0] = 0;
	bytePrefixSums[1] = sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*vbo->getIndexCount() + sizeof(V4N4)*vbo->getVertexCount();
	mPriByteSize = sizeof(unsigned) + sizeof(unsigned)*(2) + bytePrefixSums[1];
	mProData = new char[mPriByteSize];

	((unsigned*)mProData)[0] = 1;
	memcpy(((mProData + sizeof(unsigned))), bytePrefixSums, sizeof(unsigned)*(2));

	char* tempPt =  (mProData + sizeof(unsigned) + sizeof(unsigned)*(2));
	unsigned iCount = vbo->getIndexCount();
	unsigned vCount = vbo->getVertexCount();

	((uint64_t*)tempPt)[0] = vbo->getId();
	((unsigned*)(tempPt + sizeof(uint64_t)))[0] = iCount;
	((unsigned*)(tempPt + sizeof(uint64_t)))[1] = vCount;
	memcpy((tempPt + sizeof(uint64_t) + sizeof(unsigned)*2), vbo->getIndexData(), sizeof(unsigned)*iCount);
	memcpy((tempPt + sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount), vbo->getVertexData(), sizeof(float)*vCount*4 + sizeof(char)*vCount*4);
}

VboEvent::VboEvent(const std::vector<ooctools::IndexedVbo*>& vboVec, const std::vector<float>& distVec) // multiple vbos
{
	unsigned vboCount = vboVec.size();
	unsigned bytePrefixSums[vboCount+1];
	bytePrefixSums[0] = 0;
	for (unsigned i=1; i <= vboCount; ++i){
		bytePrefixSums[i] = bytePrefixSums[i-1] + sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(float) + sizeof(unsigned)*vboVec[i-1]->getIndexCount() + sizeof(V4N4)*vboVec[i-1]->getVertexCount();
	}
	// vbocount + bytePrefixSums+1 + sum of bytes
	mPriByteSize = sizeof(unsigned) + sizeof(unsigned)*(vboCount+1) + bytePrefixSums[vboCount];
	mProData = new char[mPriByteSize];

	((unsigned*)mProData)[0] = vboCount;
	memcpy(((mProData + sizeof(unsigned))), bytePrefixSums, sizeof(unsigned)*(vboCount+1));

	for (unsigned i=0; i < vboCount; ++i){
		char* tempPt =  (mProData + sizeof(unsigned) + sizeof(unsigned)*(vboCount+1) + bytePrefixSums[i]);
		unsigned iCount = vboVec[i]->getIndexCount();
		unsigned vCount = vboVec[i]->getVertexCount();

		((uint64_t*)tempPt)[0] = vboVec[i]->getId();
		((unsigned*)(tempPt + sizeof(uint64_t)))[0] = iCount;
		((unsigned*)(tempPt + sizeof(uint64_t)))[1] = vCount;
		((unsigned*)(tempPt + sizeof(uint64_t) + sizeof(unsigned)*2))[0] = distVec[i];
		memcpy((tempPt + sizeof(uint64_t) + sizeof(unsigned)*2) + sizeof(float), vboVec[i]->getIndexData(), sizeof(unsigned)*iCount);
		memcpy((tempPt + sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(float) + sizeof(unsigned)*iCount), vboVec[i]->getVertexData(), sizeof(float)*vCount*4 + sizeof(char)*vCount*4);
	}
}

const ooctools::V4N4* VboEvent::getVertexArray(unsigned idx) const
{
	return ((ooctools::V4N4*)(mProData + sizeof(unsigned) + sizeof(unsigned)*(getVboCount()+1) + getBytePrefixSum(idx) + sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(float) + sizeof(unsigned)*getIndexCount(idx)));
};
const unsigned* VboEvent::getIndexArray(unsigned idx) const
{
	return ((unsigned*)(mProData + sizeof(unsigned) + sizeof(unsigned)*(getVboCount()+1) + getBytePrefixSum(idx) + sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(float)));
};
float VboEvent::getDist(unsigned idx) const
{
	return ((float*)(mProData + sizeof(unsigned) + sizeof(unsigned)*(getVboCount()+1) + getBytePrefixSum(idx) + sizeof(uint64_t) + sizeof(unsigned)*2))[0];
}
unsigned VboEvent::getVertexCount(unsigned idx) const
{
	return ((unsigned*)(mProData + sizeof(unsigned) + sizeof(unsigned)*(getVboCount()+1) + getBytePrefixSum(idx) + sizeof(uint64_t)))[1];
}
unsigned VboEvent::getIndexCount(unsigned idx) const
{
	return ((unsigned*)(mProData + sizeof(unsigned) + sizeof(unsigned)*(getVboCount()+1) + getBytePrefixSum(idx) + sizeof(uint64_t)))[0];
}
uint64_t VboEvent::getNodeId(unsigned idx) const
{
	return ((uint64_t*)(mProData + sizeof(unsigned) + sizeof(unsigned)*(getVboCount()+1) + getBytePrefixSum(idx)))[0];
}
unsigned VboEvent::getVboCount() const
{
	return ((unsigned*)mProData)[0];
}
unsigned VboEvent::getBytePrefixSum(unsigned idx) const
{
	return ((unsigned*)(mProData + sizeof(unsigned)))[idx];
}
VboEvent::VboEvent(std::string path, uint64_t nodeId) // single vbo
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

	unsigned bytePrefixSums[2];
	bytePrefixSums[0] = 0;
	bytePrefixSums[1] = sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount + sizeof(V4N4)*vCount;
	mPriByteSize = sizeof(unsigned) + sizeof(unsigned)*(2) + bytePrefixSums[1];
	mProData = new char[mPriByteSize];
	((unsigned*)mProData)[0] = 1;
	memcpy(((mProData + sizeof(unsigned))), bytePrefixSums, sizeof(unsigned)*(2));
	char* tempPt =  (mProData + sizeof(unsigned) + sizeof(unsigned)*(2));

	((uint64_t*)tempPt)[0] = nodeId;
	((unsigned*)(tempPt + sizeof(uint64_t)))[0] = iCount;
	((unsigned*)(tempPt + sizeof(uint64_t)))[1] = vCount;
	idxFile.read((tempPt + sizeof(uint64_t) + sizeof(unsigned)*2), sizeof(unsigned)*iCount);
	vertexFile.read((tempPt + sizeof(uint64_t) + sizeof(unsigned)*2 + sizeof(unsigned)*iCount), sizeof(float)*vCount*4 + sizeof(char)*vCount*4);

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
