/**
 * @file	VboV4T2.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV4T2 class declaration.
 */

#include "VboV4T2.h"

VboV4T2::VboV4T2() : mProData(0), mProFaces(0){
	// TODO Auto-generated constructor stub

}

VboV4T2::VboV4T2(unsigned _faceCount, unsigned _vertexCount) : mProData(0), mProFaces(0){
	mProData = new RsV4T2[_vertexCount];
	mProFaces = new unsigned[_faceCount];
}

VboV4T2::~VboV4T2() {
	delete[] mProData;
	mProData = 0;
	delete[] mProFaces;
	mProFaces = 0;
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
