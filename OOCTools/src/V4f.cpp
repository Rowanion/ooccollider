/*
 * V4f.cpp
 *
 *  Created on: 19.02.2009
 *      Author: ava
 */

#include "V4f.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

#include "StructDefs.h"
#include "Face.h"

using namespace std;
namespace ooctools {

V4f::V4f() : x(mPriData), y(mPriData+1), z(mPriData+2), w(mPriData+3), mPriFaceList(0)
{
	mPriData[0] = 0.0f;
	mPriData[1] = 0.0f;
	mPriData[2] = 0.0f;
	mPriData[3] = 0.0f;
	mPriCount = 4;
}

V4f::V4f(float _x, float _y, float _z, float _w) : x(mPriData), y(mPriData+1), z(mPriData+2), w(mPriData+3), mPriFaceList(0)
{
	mPriData[0] = _x;
	mPriData[1] = _y;
	mPriData[2] = _z;
	mPriData[3] = _z;
	mPriCount = 4;
}

V4f::V4f(const float* _data) : x(mPriData), y(mPriData+1), z(mPriData+2), w(mPriData+3), mPriFaceList(0)
{
	mPriData[0] = _data[0];
	mPriData[1] = _data[1];
	mPriData[2] = _data[2];
	mPriData[3] = _data[3];
	mPriCount = 4;
}

V4f::V4f(float _x) : x(mPriData), y(mPriData+1), z(mPriData+2), w(mPriData+3), mPriFaceList(0)
{
	mPriData[0] = _x;
	mPriData[1] = _x;
	mPriData[2] = _x;
	mPriData[3] = _x;
	mPriCount = 4;
}

// copy constructor
V4f::V4f(const V4f& _v) : x(mPriData), y(mPriData+1), z(mPriData+2), w(mPriData+3), mPriFaceList(0)
{
	mPriData[0] = _v.getX();
	mPriData[1] = _v.getY();
	mPriData[2] = _v.getZ();
	mPriData[3] = _v.getW();
	mPriCount = 4;

}

V4f::V4f(const V3f& _v) : x(mPriData), y(mPriData+1), z(mPriData+2), w(mPriData+3), mPriFaceList(0)
{
	mPriData[0] = _v.getX();
	mPriData[1] = _v.getY();
	mPriData[2] = _v.getZ();
	mPriData[3] = 1.0f;
	mPriCount = 4;

}

//V4f::V4f(const V4f* _v) : mPriFaceList(0)
//{
//	x = mPriData;
//	y = mPriData+1;
//	z = mPriData+2;
//	mPriData[0] = _v->getX();
//	mPriData[1] = _v->getY();
//	mPriData[2] = _v->getZ();
//	mPriCount = 3;
//}

V4f::~V4f()
{
//	cout << "V4f...." << endl;
	if (mPriFaceList!=0) {
//		std::vector<Face*>::iterator it;
//		for(it=mPriFaceList->begin(); it!=mPriFaceList->end(); ++it){
//			delete (*it);
//			(*it) = 0;
//		}
		mPriFaceList->clear();
		delete mPriFaceList;
		mPriFaceList = 0;
	}
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

const V4f
V4f::max(const V4f& _a, const V4f& _b)
{
	return V4f(fmax(_a.getX(), _b.getX()), fmax(_a.getY(), _b.getY()), fmax(
			_a.getZ(), _b.getZ()), fmax(_a.getW(), _b.getW()));
}

const V4f
V4f::min(const V4f& _a, const V4f& _b)
{
	return V4f(fmin(_a.getX(), _b.getX()), fmin(_a.getY(), _b.getY()), fmin(
			_a.getZ(), _b.getZ()), fmin(_a.getW(), _b.getW()));
}

void
V4f::minMe(const V4f& _a)
{
	minMe(_a.getX(), _a.getY(), _a.getZ(), _a.getW());
}

void
V4f::maxMe(const V4f& _a)
{
	maxMe(_a.getX(), _a.getY(), _a.getZ(), _a.getW());
}

void
V4f::minMe(float _x, float _y, float _z, float _w)
{
	mPriData[0] = fmin(mPriData[0], _x);
	mPriData[1] = fmin(mPriData[1], _y);
	mPriData[2] = fmin(mPriData[2], _z);
	mPriData[3] = fmin(mPriData[3], _w);
}

void
V4f::maxMe(float _x, float _y, float _z, float _w){
	mPriData[0] = fmax(mPriData[0], _x);
	mPriData[1] = fmax(mPriData[1], _y);
	mPriData[2] = fmax(mPriData[2], _z);
	mPriData[3] = fmax(mPriData[3], _w);
}

bool
V4f::hasSharedComponent(const V4f& _v) const
{
	if(_v.getX() == getX()) return true;
	if(_v.getY() == getY()) return true;
	if(_v.getZ() == getZ()) return true;
	if(_v.getW() == getW()) return true;
	return false;
}

V4f&
V4f::normalize()
{
	float reciSqrt = 1.0f / sqrt(pow(mPriData[0], 2) + pow(mPriData[1], 2)
			+ pow(mPriData[2], 2) + pow(mPriData[3], 2));

	// ExtremeOperatorOverloadingPower! Schakkalakka!
	(*this) *= reciSqrt;
   	return (*this);
//   	mPriData[0]*=reciSqrt;
//   	mPriData[1]*=reciSqrt;
//   	mPriData[2]*=reciSqrt;
}

bool
V4f::hasSharedComponent(const V4f& _v1, const V4f& _v2)
{
	return _v1.hasSharedComponent(_v2);
}

bool
V4f::operator==(V4f& a)
{
	if (mPriData[0] == a.getX() && mPriData[1] == a.getY() && mPriData[2]
			== a.getZ() && mPriData[3] == a.getW())
		return true;
	else return false;
}

//bool
//V4f::operator==(V4f* a)
//{
//	if (mPriData[0]==a->getX() && mPriData[1]==a->getY() && mPriData[2]==a->getZ())
//		return true;
//	else return false;
//}

bool
V4f::operator!=(V4f& a)
{
	if (mPriData[0] != a.getX() || mPriData[1] != a.getY() || mPriData[2]
			!= a.getZ() || mPriData[3] != a.getW())
		return true;
	else return false;
}

bool
V4f::operator>(const V4f& a) const
{
	if (mPriData[0] > a.getX() && mPriData[1] > a.getY() && mPriData[2]
			> a.getZ() && mPriData[3] > a.getW())
		return true;
	else
		return false;
}

//bool
//V4f::operator>(V4f* a)
//{
//	if (mPriData[0] > a->getX() && mPriData[1] > a->getY() && mPriData[2] > a->getZ())
//		return true;
//	else return false;
//}

bool
V4f::operator>=(V4f& a)
{
	if (mPriData[0] >= a.getX() && mPriData[1] >= a.getY() && mPriData[2]
			>= a.getZ() && mPriData[3] >= a.getW())
		return true;
	else return false;
}

bool
V4f::operator>=(V4f* a)
{
	if (mPriData[0] >= a->getX() && mPriData[1] >= a->getY() && mPriData[2]
			>= a->getZ() && mPriData[3] >= a->getW())
		return true;
	else return false;
}

bool
V4f::operator<(const V4f& a) const
{
	if (mPriData[0] < a.getX() && mPriData[1] < a.getY() && mPriData[2]
			< a.getZ() && mPriData[3] < a.getW())
		return true;
	else return false;
}

//bool
//V4f::operator<(const V4f* a)
//{
//	if (mPriData[0] < a->getX() && mPriData[1] < a->getY() && mPriData[2] < a->getZ())
//		return true;
//	else return false;
//}

bool
V4f::operator<=(V4f& a)
{
	if (mPriData[0] <= a.getX() && mPriData[1] <= a.getY() && mPriData[2]
			<= a.getZ() && mPriData[3] <= a.getW())
		return true;
	else return false;
}

V4f&
V4f::operator=(const V4f& a)
{
	if (this != &a){
//		V4f copy(a);
		mPriData[0] = a.getX();
		mPriData[1] = a.getY();
		mPriData[2] = a.getZ();
		mPriData[3] = a.getW();
	}
	return *this;
}

V4f&
V4f::operator=(const V3f& a)
{
	mPriData[0] = a.getX();
	mPriData[1] = a.getY();
	mPriData[2] = a.getZ();
	return *this;
}

V4f&
V4f::operator/=(const float f)
{
	mPriData[0]/=f;
	mPriData[1]/=f;
	mPriData[2]/=f;
	mPriData[3]/=f;
	return *this;
}

V4f&
V4f::operator+=(const V4f& a)
{
//	V4f copy(a);
	mPriData[0]+=a.getX();
	mPriData[1]+=a.getY();
	mPriData[2]+=a.getZ();
	mPriData[3]+=a.getW();
	return (*this);
}

V4f&
V4f::operator+=(const float f)
{
	mPriData[0]+=f;
	mPriData[1]+=f;
	mPriData[2]+=f;
	mPriData[3]+=f;
	return (*this);
}

V4f&
V4f::operator*=(float f)
{
   	mPriData[0]*=f;
   	mPriData[1]*=f;
   	mPriData[2]*=f;
   	mPriData[3]*=f;
   	return *this;
}

V4f&
V4f::operator-=(const V4f& a)
{
   	mPriData[0]-=a.getX();
   	mPriData[1]-=a.getY();
   	mPriData[2]-=a.getZ();
   	mPriData[3]-=a.getW();
   	return *this;
}

V4f
V4f::operator/(float f) const
{
	V4f copy = *this; // using the copy constructor - yay
	copy /= f;
	return copy;
}
V4f
V4f::operator+(const V4f& a) const
{
	V4f copy = *this;
	copy += a;
	return copy;
}

V4f
V4f::operator-(const V4f& a) const
{
	V4f copy = *this;
	copy -= a;
	return copy;
}

V4f
V4f::operator*(float f) const
{
	V4f copy = *this;
	copy *= f;
	return copy;
}

void
V4f::addFaceRef(Face* face)
{
	if (mPriFaceList==0)
		mPriFaceList = new vector<Face*>;
	mPriFaceList->push_back(face);
}

V4f
V4f::getAbs() const
{
	return V4f(fabs(getX()), fabs(getY()), fabs(getZ()), fabs(getW()));
}

string
V4f::toString(const float* _array)
{
	stringstream st;
	st << "(" << _array[0] << ", " << _array[1] << ", " << _array[2] << ", " << _array[3] << ")";
	return (const std::string) st.str();
}

const string
V4f::toString() const
{
		stringstream st;
		st << "(" << mPriData[0] << ", " << mPriData[1] << ", " << mPriData[2] << ", " << mPriData[3] << ")";
		return (const std::string) st.str();
}

} // Namespace OOCTools
