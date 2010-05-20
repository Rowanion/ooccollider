/**
 * @file	VboV4.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV4 class declaration.
 */

#include "VboV4.h"

VboV4::VboV4() : mProData(0), mProFaces(0){
	// TODO Auto-generated constructor stub

}

VboV4::VboV4(unsigned _faceCount, unsigned _vertexCount) : mProData(0), mProFaces(0){
	mProData = new RsV4[_vertexCount];
	mProFaces = new unsigned[_faceCount];
}

VboV4::~VboV4() {
	delete[] mProData;
	mProData = 0;
	delete[] mProFaces;
	mProFaces = 0;
}

unsigned VboV4::getIndexCount() const
{
	//TODO
	return 0;
}

unsigned VboV4::getVertexCount() const
{
	//TODO
	return 0;
}

uint64_t VboV4::getComponentBytes() const
{
	return sizeof(RsV4);
}

void VboV4::draw()
{

}
