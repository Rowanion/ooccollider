/**
 * @file	RsVboV4.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsVboV4 class definition.
 */

#include "RsVboV4.h"

RsVboV4::RsVboV4() : mProData(0), mProFaces(0){
	// TODO Auto-generated constructor stub

}

RsVboV4::RsVboV4(unsigned _faceCount, unsigned _vertexCount) : mProData(0), mProFaces(0){
	mProData = new RsV4[_vertexCount];
	mProFaces = new unsigned[_faceCount];
}

RsVboV4::~RsVboV4() {
	delete[] mProData;
	mProData = 0;
	delete[] mProFaces;
	mProFaces = 0;
}

unsigned RsVboV4::getIndexCount() const
{
	//TODO
	return 0;
}

unsigned RsVboV4::getVertexCount() const
{
	//TODO
	return 0;
}

uint64_t RsVboV4::getComponentBytes() const
{
	return sizeof(RsV4);
}

void RsVboV4::draw()
{

}
