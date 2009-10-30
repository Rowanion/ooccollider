/**
 * @file	DepthBufferEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	DepthBufferEvent class definition.
 */

#include "DepthBufferEvent.h"

#include <iostream>
#include <cstring>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* DepthBufferEvent::mClassId = new oocframework::ClassId("DepthBufferEvent");

DepthBufferEvent::DepthBufferEvent() {
	mProData = 0;
	init();
}

DepthBufferEvent::DepthBufferEvent(Tile _tileDim, int xPos, int yPos, int width, int height, int mpiRank, const GLfloat* pixel)
{
	mProByteSize = sizeof(Tile) + sizeof(int)*5 + sizeof(GLfloat)*width*height;
	mProData = new char[mProByteSize];
	((Tile*)mProData)[0] = _tileDim;
	((int*)(mProData+sizeof(Tile)))[0] = xPos;
	((int*)(mProData+sizeof(Tile)))[1] = yPos;
	((int*)(mProData+sizeof(Tile)))[2] = width;
	((int*)(mProData+sizeof(Tile)))[3] = height;
	((int*)(mProData+sizeof(Tile)))[4] = mpiRank;
	memcpy((mProData+sizeof(Tile)+sizeof(int)*5), pixel, sizeof(GLfloat)*width*height);
//	std::cout << "Texture: " << std::endl;
//	for (unsigned i =0; i < sizeof(GLfloat)*width*height*4; i+=4){
//		if (*(((GLfloat*)(mProData+sizeof(int)*4))+i) != 250){
//			std::cout << "B: " << (int) *(((GLfloat*)(mProData+sizeof(int)*4))+i);
//			std::cout << ", R: " << (int) *(((GLfloat*)(mProData+sizeof(int)*4))+i+1);
//			std::cout << ", G: " << (int) *(((GLfloat*)(mProData+sizeof(int)*4))+i+2);
//			std::cout << ", A: " << (int) *(((GLfloat*)(mProData+sizeof(int)*4))+i+3) << std::endl;
//		}
//	}
}

DepthBufferEvent::DepthBufferEvent(const oocframework::Message* msg)
{
	mProByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mProByteSize];
	memcpy(mProData, msg->getData(),msg->getLength());
//
//
//	std::cout << "w: " << ((int*)dat)[0] << std::endl;
//	std::cout << "h: " << ((int*)dat)[1] << std::endl;
//	std::cout << "x: " << ((int*)dat)[2] << std::endl;
//	std::cout << "y: " << ((int*)dat)[3] << std::endl;
//	std::cout << (int)((unsigned char*)(dat+4*sizeof(int)))[0] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[1] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[2] << std::endl;

}

DepthBufferEvent::~DepthBufferEvent() {
	delete[] mProData;
}

const oocframework::ClassId* DepthBufferEvent::classid()
{
	return mClassId;
}

void DepthBufferEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ResizeWindowEvent");
	}
}

bool
DepthBufferEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
