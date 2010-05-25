/**
 * @file	VboV3N4T2.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV3N4T2 class declaration.
 */

#include "VboV3N4T2.h"

#include <cstring>
#include <iostream>
#include <cstdlib>

VboV3N4T2::VboV3N4T2() : mProData(0), mProIndices(0){
	// TODO Auto-generated constructor stub

}

VboV3N4T2::VboV3N4T2(unsigned _indexCount, unsigned* _indices, std::map<RsV3N4T2, unsigned>* _data) : mProDataCount(0), mProIndexCount(_indexCount)
{
	std::map<RsV3N4T2, unsigned>::iterator it;
	unsigned entryCount = _data->size();
	mProData = new RsV3N4T2[entryCount];
	mProIndices = new unsigned[_indexCount];
	memcpy(mProIndices, _indices, sizeof(unsigned)*_indexCount);

	for (it = _data->begin(); it != _data->end(); it++){
		mProData[it->second] = it->first;
		mProDataCount++;
	}
}

VboV3N4T2::~VboV3N4T2() {
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;
}

unsigned VboV3N4T2::getIndexCount() const
{
	//TODO
	return 0;
}

unsigned VboV3N4T2::getVertexCount() const
{
	//TODO
	return 0;
}

uint64_t VboV3N4T2::getComponentBytes() const
{
	return sizeof(RsV3N4T2);
}

void VboV3N4T2::draw()
{
	//TODO
}

void VboV3N4T2::debug()
{
	// check closer  - some numbers seem very small...
	for (unsigned i = 0; i< mProIndexCount; ++i){
		std::cerr << mProData[i].v.x << ", " << mProData[i].v.y << ", " << mProData[i].v.z << std::endl;
	}
}
