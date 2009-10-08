/**
 * @file	ColorBufferEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ColorBufferEvent class definition.
 */

#include "ColorBufferEvent.h"

#include <iostream>
#include <cstring>

#include "ClassId.h"
#include "IEvent.h"

oocframework::ClassId* ColorBufferEvent::mClassId = new oocframework::ClassId("ColorBufferEvent");

ColorBufferEvent::ColorBufferEvent() {
	mProData = 0;
	init();
}

ColorBufferEvent::ColorBufferEvent(int xPos, int yPos, int width, int height, double renderTime, const GLubyte* pixel)
{
	mPriByteSize = sizeof(int)*5 + sizeof(double) + sizeof(GLubyte)*width*height*4;
	mProData = new char[mPriByteSize];
	((int*)mProData)[0] = xPos;
	((int*)mProData)[1] = yPos;
	((int*)mProData)[2] = width;
	((int*)mProData)[3] = height;
	((int*)mProData)[4] = MpiControl::getSingleton()->getRank();
	((double*)(mProData+5*sizeof(int)))[0] = renderTime;
	memcpy((mProData+sizeof(int)*5 + sizeof(double)), pixel, sizeof(GLubyte)*width*height*4);
//	std::cout << "Texture: " << std::endl;
//	for (unsigned i =0; i < sizeof(GLubyte)*width*height*4; i+=4){
//		if (*(((GLubyte*)(mProData+sizeof(int)*4))+i) != 250){
//			std::cout << "B: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i);
//			std::cout << ", R: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i+1);
//			std::cout << ", G: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i+2);
//			std::cout << ", A: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i+3) << std::endl;
//		}
//	}
}

ColorBufferEvent::ColorBufferEvent(const Message* msg)
{
	mPriByteSize = msg->getLength();
//	const char* dat = msg->getData();
	mProData = new char[mPriByteSize];

	memcpy(mProData, msg->getData(),msg->getLength());



//	std::cout << "w: " << ((int*)dat)[0] << std::endl;
//	std::cout << "h: " << ((int*)dat)[1] << std::endl;
//	std::cout << "x: " << ((int*)dat)[2] << std::endl;
//	std::cout << "y: " << ((int*)dat)[3] << std::endl;
//	std::cout << (int)((unsigned char*)(dat+4*sizeof(int)))[0] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[1] << ", " << (int)((unsigned char*)(dat+4*sizeof(int)))[2] << std::endl;

}

ColorBufferEvent::~ColorBufferEvent() {
	delete[] mProData;
}

void ColorBufferEvent::set(int xPos, int yPos, int width, int height, double renderTime, const GLubyte* pixel)
{
	unsigned newSize = sizeof(int)*5 + sizeof(double) + sizeof(GLubyte)*width*height*4;
	if (newSize != mPriByteSize){
		delete[] mProData;
		mPriByteSize = newSize;
	}
	mProData = new char[mPriByteSize];
	((int*)mProData)[0] = xPos;
	((int*)mProData)[1] = yPos;
	((int*)mProData)[2] = width;
	((int*)mProData)[3] = height;
	((int*)mProData)[4] = MpiControl::getSingleton()->getRank();
	((double*)(mProData+5*sizeof(int)))[0] = renderTime;
	memcpy((mProData+sizeof(int)*5 + sizeof(double)), pixel, sizeof(GLubyte)*width*height*4);
//	std::cout << "Texture: " << std::endl;
//	for (unsigned i =0; i < sizeof(GLubyte)*width*height*4; i+=4){
//		if (*(((GLubyte*)(mProData+sizeof(int)*4))+i) != 250){
//			std::cout << "B: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i);
//			std::cout << ", R: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i+1);
//			std::cout << ", G: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i+2);
//			std::cout << ", A: " << (int) *(((GLubyte*)(mProData+sizeof(int)*4))+i+3) << std::endl;
//		}
//	}

}

const oocframework::ClassId* ColorBufferEvent::classid()
{
	return mClassId;
}

void ColorBufferEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("ResizeWindowEvent");
	}
}

bool
ColorBufferEvent::instanceOf(const oocframework::ClassId* cId) const
{
	if (mClassId->getId() == cId->getId() ||
			oocframework::IEvent::classid()->getId() == cId->getId()){
		return true;
	}
	else
		return false;
}
