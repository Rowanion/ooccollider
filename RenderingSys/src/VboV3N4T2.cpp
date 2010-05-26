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
	// ----------------------------------
//	unsigned testarray[_indexCount];
//	for (unsigned i=0; i<_indexCount; i++){
//		testarray[i]= 0;
//	}
//	for (std::map<RsV3N4T2, unsigned>::iterator iter = _data->begin(); iter != _data->end(); iter++){
//		testarray[iter->second] = 1;
//	}
//	for (unsigned i=0; i<_indexCount; i++){
//		if (testarray[i]== 0){
//			std::cerr << "entry " << i << " is missing!"<< std::endl;
//		}
//	}
//	exit(0);
	// ----------------------------------
	memcpy(mProIndices, _indices, sizeof(unsigned)*_indexCount);

	for (it = _data->begin(); it != _data->end(); it++){
		mProData[it->second] = it->first;
		mProDataCount++;
	}

	// ------------------------------------------------
	// GL - VBO stuff from here
	// ------------------------------------------------
	glGenBuffers(1, &mPriDataId);
	glBindBuffer(GL_ARRAY_BUFFER, mPriDataId);

	glBufferData(GL_ARRAY_BUFFER, mProDataCount*sizeof(RsV3N4T2), mProData, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, sizeof(RsV3N4T2), bufferOffset(0));
	glNormalPointer(GL_BYTE, sizeof(RsV3N4T2), bufferOffset(sizeof(float)*3));
	glTexCoordPointer(2, GL_FLOAT, sizeof(RsV3N4T2), bufferOffset(sizeof(float)*3 + sizeof(char)*4));

	glGenBuffers(1, &mPriIndexId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIndexId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mProIndexCount*sizeof(unsigned), mProIndices, GL_STATIC_DRAW);

}

VboV3N4T2::~VboV3N4T2() {
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;
}

unsigned VboV3N4T2::getIndexCount() const
{
	return mProIndexCount;
}

unsigned VboV3N4T2::getVertexCount() const
{
	return mProDataCount;
}

uint64_t VboV3N4T2::getComponentBytes() const
{
	return sizeof(RsV3N4T2);
}

void VboV3N4T2::draw()
{
	//TODO
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, mPriDataId);
	glVertexPointer(3, GL_FLOAT, sizeof(RsV3N4T2), bufferOffset(0));
	glNormalPointer(GL_BYTE, sizeof(RsV3N4T2), bufferOffset(sizeof(float)*3));
	glTexCoordPointer(2, GL_FLOAT, sizeof(RsV3N4T2), bufferOffset(sizeof(float)*3+sizeof(char)*4));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIndexId);
	glDrawElements(GL_TRIANGLES, mProIndexCount, GL_UNSIGNED_INT, bufferOffset(0));

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void VboV3N4T2::debug()
{
	// check closer  - some numbers seem very small...
	for (unsigned i = 0; i< mProDataCount; ++i){
//		std::cerr << (int)mProData[i].n.x << ", " << (int)mProData[i].n.y << ", " << (int)mProData[i].n.z << ", " << (int)mProData[i].n.w << std::endl;
	}
}
