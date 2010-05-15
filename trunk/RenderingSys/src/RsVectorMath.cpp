/**
 * @file	RsVectorMath.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2010
 * @brief 	RsVectorMath class declaration.
 */

#include "RsVectorMath.h"

V2f::V2f() {
	// TODO Auto-generated constructor stub

}
V2f::V2f(float _x, float _y) {
	this->data.x = _x;
	this->data.y = _y;
}

V2f::~V2f() {
	// TODO Auto-generated destructor stub
}

V2f V2f::operator+(const V2f& _rhs)
{
	V2f result = V2f();
	result.data = this->data + _rhs.data;
	return result;
}

V2f V2f::operator-(const V2f& _rhs)
{
	V2f result = V2f();
	result.data = this->data - _rhs.data;
	return result;
}

V2f V2f::operator*(float _rhs)
{
	return V2f(this->data.x*_rhs, this->data.y*_rhs);
}

// ------------------------------------------------------

V3f::V3f() {
	// TODO Auto-generated constructor stub

}

V3f::~V3f() {
	// TODO Auto-generated destructor stub
}

V3f::V3f(float _x, float _y, float _z) {
	this->data.x = _x;
	this->data.y = _y;
	this->data.z = _z;
}

V3f V3f::operator+(const V3f& _rhs)
{
	this->data = this->data + _rhs.data;
	return *this;
}

V3f V3f::operator-(const V3f& _rhs)
{
	this->data = this->data - _rhs.data;
	return *this;
}

V3f V3f::operator*(float _rhs)
{
	return V3f(this->data.x*_rhs, this->data.y*_rhs, this->data.z*_rhs);
}

// -------------------------------------------------------------

Tuplef operator+(const Tuplef& _lhs, const Tuplef& _rhs)
{
	Tuplef result = Tuplef();
	result.x = _lhs.x + _rhs.x;
	result.y = _lhs.y + _rhs.y;
	return result;
}

Tuplef operator-(const Tuplef& _lhs, const Tuplef& _rhs)
{
	Tuplef result = Tuplef();
	result.x = _lhs.x - _rhs.x;
	result.y = _lhs.y - _rhs.y;
	return result;
}

Triplef operator+(const Triplef& _lhs, const Triplef& _rhs)
{
	Triplef result = Triplef();
	result.x = _lhs.x + _rhs.x;
	result.y = _lhs.y + _rhs.y;
	result.z = _lhs.z + _rhs.z;
	return result;
}

Triplef operator-(const Triplef& _lhs, const Triplef& _rhs)
{
	Triplef result = Triplef();
	result.x = _lhs.x - _rhs.x;
	result.y = _lhs.y - _rhs.y;
	result.z = _lhs.z - _rhs.z;
	return result;
}

