/*
 * V3ub.cpp
 *
 *  Created on: 19.02.2009
 *      Author: ava
 */

#include "V3ub.h"
#include <string>
#include <sstream>
#include <iostream>

#include "V3f.h"

namespace ooctools {

V3ub::V3ub() : x(mPriData), y(mPriData+1), z(mPriData+2)
{
//	mPriData = new unsigned char[3];
	mPriCount = 0;

}
V3ub::V3ub(unsigned char _x, unsigned char _y, unsigned char _z) : x(mPriData), y(mPriData+1), z(mPriData+2)
{
//	mPriData = new unsigned char[3];
	mPriData[0] = _x;
	mPriData[1] = _y;
	mPriData[2] = _z;
	mPriCount = 3;
}

V3ub::V3ub(float _x, float _y, float _z) : x(mPriData), y(mPriData+1), z(mPriData+2)
{
//	mPriData = new unsigned char[3];
	mPriData[0] = static_cast<unsigned char>(255.0*_x);
	mPriData[1] = static_cast<unsigned char>(255.0*_y);
	mPriData[2] = static_cast<unsigned char>(255.0*_z);
	mPriCount = 3;
}

V3ub::V3ub(unsigned char _x) : x(mPriData), y(mPriData+1), z(mPriData+2)
{
//	mPriData = new unsigned char[3];
	mPriData[0] = _x;
	mPriData[1] = _x;
	mPriData[2] = _x;
	mPriCount = 3;
}

V3ub::V3ub(const unsigned char* _data) : x(mPriData), y(mPriData+1), z(mPriData+2)
{
//	mPriData = new unsigned char[3];
	mPriData[0] = _data[0];
	mPriData[1] = _data[1];
	mPriData[2] = _data[2];
	mPriCount = 3;
}

V3ub::V3ub(const V3f& _v) : x(mPriData), y(mPriData+1), z(mPriData+2)
{
//	mPriData = new unsigned char[3];
	mPriData[0] = static_cast<unsigned char>(255.0*_v.getX());
	mPriData[1] = static_cast<unsigned char>(255.0*_v.getY());
	mPriData[2] = static_cast<unsigned char>(255.0*_v.getZ());
	mPriCount = 3;
}

// copy constructor
V3ub::V3ub(const V3ub& _v) :
	x(mPriData), y(mPriData + 1), z(mPriData + 2)
{
//	mPriData = new unsigned char[3];
	mPriData[0] = _v.getX();
	mPriData[1] = _v.getY();
	mPriData[2] = _v.getZ();
	mPriCount = 3;
}

V3ub::~V3ub()
{
	x = 0;
	y = 0;
	z = 0;
//	std::cout << "V3ub..." << std::endl;
}

const unsigned char*
V3ub::getData()
{
	return (const unsigned char*) mPriData;
}

bool
V3ub::operator==(V3ub& a)
{
	if (mPriData[0]==a.getX() && mPriData[1]==a.getY() && mPriData[2]==a.getZ())
		return true;
	else return false;
}

bool
V3ub::operator==(V3ub* a)
{
	if (mPriData[0]==a->getX() && mPriData[1]==a->getY() && mPriData[2]==a->getZ())
		return true;
	else return false;
}

bool
V3ub::operator!=(V3ub& a)
{
	if (mPriData[0]!=a.getX() || mPriData[1]!=a.getY() || mPriData[2]!=a.getZ())
		return true;
	else return false;
}

bool
V3ub::operator!=(V3ub* a)
{
	if (mPriData[0]!=a->getX() || mPriData[1]!=a->getY() || mPriData[2]!=a->getZ())
		return true;
	else return false;
}

std::string
V3ub::toString() const
{
	std::stringstream st;
	st << "(" << (int)mPriData[0] << ", " << (int)mPriData[1] << ", " << (int)mPriData[2] << ")";
	return st.str();
}

V3ub&
V3ub::operator=(const V3ub& a){
	mPriData[0] = a.getX();
	mPriData[1] = a.getY();
	mPriData[2] = a.getZ();
	return *this;
}

} // Namespace OOCTools
