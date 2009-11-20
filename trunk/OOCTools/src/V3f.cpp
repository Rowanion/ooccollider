/**
 * @file	V3f.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 * @brief   Class definition of V3f
 */

#include "V3f.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

#include "StructDefs.h"
#include "Face.h"
#include "V4f.h"

using namespace std;
namespace ooctools {

V3f::V3f() :
	x(mPriData), y(mPriData + 1), z(mPriData + 2), mPriFaceList(0)
{
	mPriData[0] = 1.0f;
	mPriData[1] = 0.0f;
	mPriData[2] = 0.0f;
	mPriCount = 3;
}

V3f::V3f(float _x, float _y, float _z) :
	x(mPriData), y(mPriData + 1), z(mPriData + 2), mPriFaceList(0)
{
	mPriData[0] = _x;
	mPriData[1] = _y;
	mPriData[2] = _z;
	mPriCount = 3;
}

V3f::V3f(const float* _data) :
	x(mPriData), y(mPriData + 1), z(mPriData + 2), mPriFaceList(0)
{
	mPriData[0] = _data[0];
	mPriData[1] = _data[1];
	mPriData[2] = _data[2];
}

V3f::V3f(float _x) :
	x(mPriData), y(mPriData + 1), z(mPriData + 2), mPriFaceList(0)
{
	mPriData[0] = _x;
	mPriData[1] = _x;
	mPriData[2] = _x;
	mPriCount = 3;
}

// copy constructor
V3f::V3f(const V3f& _v) :
	x(mPriData), y(mPriData + 1), z(mPriData + 2), mPriFaceList(0)
{
	mPriData[0] = _v.getX();
	mPriData[1] = _v.getY();
	mPriData[2] = _v.getZ();
	mPriCount = 3;
}

V3f::V3f(const V4f& _v) :
	x(mPriData), y(mPriData + 1), z(mPriData + 2), mPriFaceList(0)
{
	mPriData[0] = _v.getX();
	mPriData[1] = _v.getY();
	mPriData[2] = _v.getZ();
	mPriCount = 3;
}

//V3f::V3f(const V3f* _v) : mPriFaceList(0)
//{
//	x = mPriData;
//	y = mPriData+1;
//	z = mPriData+2;
//	mPriData[0] = _v->getX();
//	mPriData[1] = _v->getY();
//	mPriData[2] = _v->getZ();
//	mPriCount = 3;
//}

V3f::~V3f()
{
//	cout << "V3f...." << endl;
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
}

const V3f
V3f::max(const V3f& _a, const V3f& _b)
{
	return V3f(fmax(_a.getX(), _b.getX()), fmax(_a.getY(), _b.getY()), fmax(_a.getZ(), _b.getZ()));
}

const V3f
V3f::min(const V3f& _a, const V3f& _b)
{
	return V3f(fmin(_a.getX(), _b.getX()), fmin(_a.getY(), _b.getY()), fmin(_a.getZ(), _b.getZ()));
}

void
V3f::minMe(const V3f& _a)
{
	minMe(_a.getX(), _a.getY(), _a.getZ());
}

void
V3f::maxMe(const V3f& _a)
{
	maxMe(_a.getX(), _a.getY(), _a.getZ());
}

void
V3f::minMe(float _x, float _y, float _z)
{
	mPriData[0] = fmin(mPriData[0], _x);
	mPriData[1] = fmin(mPriData[1], _y);
	mPriData[2] = fmin(mPriData[2], _z);
}

void
V3f::maxMe(float _x, float _y, float _z){
	mPriData[0] = fmax(mPriData[0], _x);
	mPriData[1] = fmax(mPriData[1], _y);
	mPriData[2] = fmax(mPriData[2], _z);
}

bool
V3f::hasSharedComponent(const V3f& _v) const
{
	if(_v.getX() == getX()) return true;
	if(_v.getY() == getY()) return true;
	if(_v.getZ() == getZ()) return true;
	return false;
}

V3f&
V3f::normalize()
{





   	float reciSqrt = 1.0f/calculateMagnitude();

   	// ExtremeOperatorOverloadingPower! Schakkalakka!
   	(*this) *= reciSqrt;
   	return (*this);
//   	mPriData[0]*=reciSqrt;
//   	mPriData[1]*=reciSqrt;
//   	mPriData[2]*=reciSqrt;
}

bool
V3f::hasSharedComponent(const V3f& _v1, const V3f& _v2)
{
	return _v1.hasSharedComponent(_v2);
}

bool
V3f::operator==(const V3f& a)
{
	if (mPriData[0]==a.getX() && mPriData[1]==a.getY() && mPriData[2]==a.getZ())
		return true;
	else return false;
}

//bool
//V3f::operator==(V3f* a)
//{
//	if (mPriData[0]==a->getX() && mPriData[1]==a->getY() && mPriData[2]==a->getZ())
//		return true;
//	else return false;
//}

bool
V3f::operator!=(V3f& a)
{
	if (mPriData[0]!=a.getX() || mPriData[1]!=a.getY() || mPriData[2]!=a.getZ())
		return true;
	else return false;
}

bool
V3f::operator>(const V3f& a) const
{
	if (mPriData[0] > a.getX() && mPriData[1] > a.getY() && mPriData[2] > a.getZ())
		return true;
	else return false;
}

//bool
//V3f::operator>(V3f* a)
//{
//	if (mPriData[0] > a->getX() && mPriData[1] > a->getY() && mPriData[2] > a->getZ())
//		return true;
//	else return false;
//}

bool
V3f::operator>=(const V3f& a) const
{
	if (mPriData[0] >= a.getX() && mPriData[1] >= a.getY() && mPriData[2] >= a.getZ())
		return true;
	else return false;
}

//bool
//V3f::operator>=(V3f* a)
//{
//	if (mPriData[0] >= a->getX() && mPriData[1] >= a->getY() && mPriData[2] >= a->getZ())
//		return true;
//	else return false;
//}

bool
V3f::operator<(const V3f& a) const
{
	if (mPriData[0] < a.getX() && mPriData[1] < a.getY() && mPriData[2] < a.getZ())
		return true;
	else return false;
}

//bool
//V3f::operator<(const V3f* a)
//{
//	if (mPriData[0] < a->getX() && mPriData[1] < a->getY() && mPriData[2] < a->getZ())
//		return true;
//	else return false;
//}

bool
V3f::operator<=(const V3f& a) const
{
	if (mPriData[0] <= a.getX() && mPriData[1] <= a.getY() && mPriData[2] <= a.getZ())
		return true;
	else return false;
}

//bool
//V3f::operator<=(V3f* a)
//{
//	if (mPriData[0] <= a->getX() && mPriData[1] <= a->getY() && mPriData[2] <= a->getZ())
//		return true;
//	else return false;
//}

V3f&
V3f::operator=(const V3f& a)
{
	if (this != &a){
//		V3f copy(a);
		mPriData[0] = a.getX();
		mPriData[1] = a.getY();
		mPriData[2] = a.getZ();
	}
	return *this;
}

V3f&
V3f::operator=(const V4f& a){
	mPriData[0] = a.getX();
	mPriData[1] = a.getY();
	mPriData[2] = a.getZ();
	return *this;
}

V3f&
V3f::operator/=(const float f)
{
	mPriData[0]/=f;
	mPriData[1]/=f;
	mPriData[2]/=f;
	return *this;
}

V3f&
V3f::operator+=(const V3f& a)
{
//	V3f copy(a);
	mPriData[0]+=a.getX();
	mPriData[1]+=a.getY();
	mPriData[2]+=a.getZ();
	return (*this);
}

V3f&
V3f::operator+=(const float f)
{
	mPriData[0]+=f;
	mPriData[1]+=f;
	mPriData[2]+=f;
	return (*this);
}

V3f&
V3f::operator*=(float f)
{
   	mPriData[0]*=f;
   	mPriData[1]*=f;
   	mPriData[2]*=f;
   	return *this;
}

V3f&
V3f::operator*=(const V3f& v)
{
	mPriData[0] *= v.getX();
	mPriData[1] *= v.getY();
	mPriData[2] *= v.getZ();
	return *this;
}

V3f&
V3f::operator-=(const V3f& a)
{
   	mPriData[0]-=a.getX();
   	mPriData[1]-=a.getY();
   	mPriData[2]-=a.getZ();
   	return *this;
}

V3f
V3f::operator/(float f) const
{
	V3f copy = *this; // using the copy constructor - yay
	copy /= f;
	return copy;
}
V3f
V3f::operator+(const V3f& a) const
{
	V3f copy = *this;
	copy += a;
	return copy;
}

V3f
V3f::operator-(const V3f& a) const
{
	V3f copy = *this;
	copy -= a;
	return copy;
}

V3f
V3f::operator*(float f) const
{
	V3f copy = *this;
	copy *= f;
	return copy;
}

//V3f *V3f::cross(V3f &a, V3f &b){
//	return new V3f(a.getY()*b.getZ()-a.getZ()*b.getY(),
//			a.getZ()*b.getX()-a.getX()*b.getZ(),
//			a.getX()*b.getY()-a.getY()*b.getX());
//}

V3f
V3f::cross(const V3f& a, const V3f& b)
{
	return V3f(a.getY() * b.getZ() - a.getZ() * b.getY(),
			a.getZ() * b.getX() - a.getX() * b.getZ(),
			a.getX() * b.getY() - a.getY() * b.getX());
}

void
V3f::calcFNormal(Face* face)
{
	V3f v1 = (*face->vertexPtrList.at(1)) - (*face->vertexPtrList.at(0));
	V3f v2 = (*face->vertexPtrList.at(2)) - (*face->vertexPtrList.at(0));
	V3f* normal = new V3f (V3f::cross(v1, v2));

	normal->normalize();
//	if (face->fNormal!=0){
//		delete face->fNormal;
//	}
	face->fNormal = normal;
}

void
V3f::addFaceRef(Face* face)
{
	if (mPriFaceList==0)
		mPriFaceList = new vector<Face*>;
	mPriFaceList->push_back(face);
}

V3f
V3f::getAbs() const
{
	return V3f(fabs(getX()), fabs(getY()), fabs(getZ()));
}

float
V3f::calculateMagnitude(){
	return sqrt(pow(mPriData[0],2)+pow(mPriData[1],2)+pow(mPriData[2],2));
}

string
V3f::toString(const float* _array)
{
	stringstream st;
	st << "(" << _array[0] << ", " << _array[1] << ", " << _array[2] << ")";
	return (const std::string) st.str();
}

const string
V3f::toString() const
{
		stringstream st;
		st << "(" << mPriData[0] << ", " << mPriData[1] << ", " << mPriData[2] << ")";
		return (const std::string) st.str();
}

float
V3f::getMaxComponent()const
{
	float val = mPriData[0];
	(val < mPriData[1]) ? val= mPriData[1] : val;
	(val < mPriData[2]) ? val= mPriData[2] : val;
	return val;
}

float
V3f::getMinComponent()const
{
	float val = mPriData[0];
	(val > mPriData[1]) ? val= mPriData[1] : val;
	(val > mPriData[2]) ? val= mPriData[2] : val;
	return val;
}

float
V3f::dot(const V3f& rhs)const
{
	return (mPriData[0]*rhs.getX() + mPriData[1]*rhs.getY() + mPriData[2]*rhs.getZ());
}

float
V3f::calcDistance(const V3f& point) const
{
	return sqrt(pow((getX() - point.getX()), 2.0) + pow((getY() - point.getY()), 2.0) + pow((getZ() - point.getZ()), 2.0));
}

float
V3f::calcSimpleDistance(const V3f& point) const
{
	V3f aVec = *this-point;
	return (aVec.getX()*aVec.getX()) + (aVec.getY()*aVec.getY()) + (aVec.getZ()*aVec.getZ());
//	return pow((*x - point.getX()), 2.0) + pow((*y - point.getY()), 2.0) + pow((*z - point.getZ()), 2.0);
}

float
V3f::calcSimpleDistance(const V3f& p1, const V3f& p2)
{
	return pow((p2.getX() - p1.getX()), 2.0) + pow((p2.getY() - p1.getY()), 2.0) + pow((p2.getZ() - p1.getZ()), 2.0);
}

V3f
V3f::lerp(V3f& v2, float factor)
{
	V3f temp = V3f(v2.getX()-*x, v2.getY()-*y, v2.getZ()-*z);
	V3f temp2 = V3f();
	temp*=factor;

	temp += *this;
//	reurn temp;
//	V3f_class *temp = V3f_class::mul(this, 1.0f-factor);
//	temp->add(V3f_class::mul(v2, factor));
	return temp;
}

void V3f::debug()
{
	cout << toString() << endl;
}
} // Namespace OOCTools
