/**
 * @file	RsVectorMath.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2010
 * @brief 	RsVectorMath class definition.
 */

#include "RsVectorMath.h"

#include <cmath>

RsV2f::RsV2f() {
	// TODO Auto-generated constructor stub

}
RsV2f::RsV2f(float _x, float _y) {
	this->data.x = _x;
	this->data.y = _y;
}

RsV2f::~RsV2f() {
	// TODO Auto-generated destructor stub
}

RsV2f RsV2f::operator+(const RsV2f& _rhs)
{
	RsV2f result = RsV2f();
	result.data = this->data + _rhs.data;
	return result;
}

RsV2f RsV2f::operator-(const RsV2f& _rhs)
{
	RsV2f result = RsV2f();
	result.data = this->data - _rhs.data;
	return result;
}

RsV2f RsV2f::operator*(float _rhs)
{
	return RsV2f(this->data.x*_rhs, this->data.y*_rhs);
}

RsV2f RsV2f::operator*(const RsV2f& _rhs)
{
	return RsV2f(this->data.x*_rhs.data.x, this->data.y*_rhs.data.y);
}
// ------------------------------------------------------

RsV3f::RsV3f() {
	// TODO Auto-generated constructor stub

}

RsV3f::~RsV3f() {
	// TODO Auto-generated destructor stub
}

RsV3f::RsV3f(float _x, float _y, float _z) {
	this->data.x = _x;
	this->data.y = _y;
	this->data.z = _z;
}

RsV3f RsV3f::operator+(const RsV3f& _rhs)
{
	this->data = this->data + _rhs.data;
	return *this;
}

RsV3f RsV3f::operator-(const RsV3f& _rhs)
{
	this->data = this->data - _rhs.data;
	return *this;
}

RsV3f RsV3f::operator*(float _rhs)
{
	return RsV3f(this->data.x*_rhs, this->data.y*_rhs, this->data.z*_rhs);
}

RsV3f RsV3f::operator*(const RsV3f& _rhs)
{
	return RsV3f(this->data.x*_rhs.data.x, this->data.y*_rhs.data.y, this->data.z*_rhs.data.z);
}

// -------------------------------------------------------------

RsTuplef operator+(const RsTuplef& _lhs, const RsTuplef& _rhs)
{
	RsTuplef result = RsTuplef();
	result.x = _lhs.x + _rhs.x;
	result.y = _lhs.y + _rhs.y;
	return result;
}

RsTuplef operator-(const RsTuplef& _lhs, const RsTuplef& _rhs)
{
	RsTuplef result = RsTuplef();
	result.x = _lhs.x - _rhs.x;
	result.y = _lhs.y - _rhs.y;
	return result;
}

RsTriplef operator+(const RsTriplef& _lhs, const RsTriplef& _rhs)
{
	RsTriplef result = RsTriplef();
	result.x = _lhs.x + _rhs.x;
	result.y = _lhs.y + _rhs.y;
	result.z = _lhs.z + _rhs.z;
	return result;
}

RsTriplef operator-(const RsTriplef& _lhs, const RsTriplef& _rhs)
{
	RsTriplef result = RsTriplef();
	result.x = _lhs.x - _rhs.x;
	result.y = _lhs.y - _rhs.y;
	result.z = _lhs.z - _rhs.z;
	return result;
}

void normalize(RsTriplec* _data)
{
	double d = 1.0 / sqrt((_data->x*_data->x) + (_data->y*_data->y) + (_data->z*_data->z));
	_data->x = (char)(d*_data->x);
	_data->y = (char)(d*_data->y);
	_data->z = (char)(d*_data->z);
}
