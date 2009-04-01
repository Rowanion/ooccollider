/*
 * V3b.cpp
 *
 *  Created on: 19.02.2009
 *      Author: ava
 */

#include "V3b.h"

#include <string>
#include <sstream>
#include <iostream>

namespace ooctools {

V3b::V3b() : x(0), y(0), z(0)
{
	mPriData = new char[3];
	x = mPriData;
	y = mPriData+1;
	z = mPriData+2;
	mPriCount = 0;

}

V3b::V3b(char _x, char _y, char _z) : x(0), y(0), z(0)
{
	mPriData = new char[3];
	x = mPriData;
	y = mPriData+1;
	z = mPriData+2;
	mPriData[0] = _x;
	mPriData[1] = _y;
	mPriData[2] = _z;
	mPriCount = 3;
}

V3b::V3b(char _x) : x(0), y(0), z(0)
{
	mPriData = new char[3];
	x = mPriData;
	y = mPriData+1;
	z = mPriData+2;
	mPriData[0] = _x;
	mPriData[1] = _x;
	mPriData[2] = _x;
	mPriCount = 3;
}

V3b::V3b(char* _data) : x(0), y(0), z(0)
{
	mPriData = new char[3];
	x = mPriData;
	y = mPriData+1;
	z = mPriData+2;
	mPriData[0] = _data[0];
	mPriData[1] = _data[1];
	mPriData[2] = _data[2];
	mPriCount = 3;
}

V3b::~V3b()
{
	delete[] mPriData;
	mPriData = 0;
//	std::cout << "deleting V3b..." << std::endl;
}

const char*
V3b::getData()
{
	return (const char*) mPriData;
}

std::string
V3b::toString()
{
	std::stringstream st;
	st << "(" << (int)mPriData[0] << ", " << (int)mPriData[1] << ", " << (int)mPriData[2] << ")";
	return st.str();
}

} // Namespace OOCTools
