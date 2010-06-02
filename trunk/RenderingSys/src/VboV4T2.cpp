/**
 * @file	VboV4T2.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV4T2 class declaration.
 */

#include "VboV4T2.h"

VboV4T2::VboV4T2(unsigned _indexCount, const unsigned* _indices, const std::map<RsV4T2, unsigned>* _data) : mProData(0), mProIndices(0){
	mProDataCount = _data->size();
	mProData = new RsV4T2[mProDataCount];
	mProIndices = new unsigned[_indexCount];
}

VboV4T2::VboV4T2(unsigned _indexCount, const unsigned* _indices, unsigned _dataCount, const RsV4T2* _data, GLenum _type)
{

}

VboV4T2::~VboV4T2() {
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;
}

unsigned VboV4T2::getIndexCount() const
{
	//TODO
	return 0;
}

unsigned VboV4T2::getVertexCount() const
{
	//TODO
	return 0;
}

uint64_t VboV4T2::getComponentBytes() const
{
	return sizeof(RsV4T2);
}

void VboV4T2::draw()
{

}
