/**
 * @file	VboV3N4.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV3N4 class declaration.
 */

#include "VboV3N4.h"

VboV3N4::VboV3N4() : mProData(0), mProFaces(0){
	// TODO Auto-generated constructor stub

}

VboV3N4::VboV3N4(unsigned _faceCount, unsigned _vertexCount) : mProData(0), mProFaces(0){
	mProData = new RsV3N4[_vertexCount];
	mProFaces = new unsigned[_faceCount];
}

VboV3N4::~VboV3N4() {
	delete[] mProData;
	mProData = 0;
	delete[] mProFaces;
	mProFaces = 0;
}

unsigned VboV3N4::getIndexCount() const
{
	//TODO
	return 0;
}

unsigned VboV3N4::getVertexCount() const
{
	//TODO
	return 0;
}

uint64_t VboV3N4::getComponentBytes() const
{
	return sizeof(RsV3N4);
}

void VboV3N4::draw()
{

}
