/**
 * @file	RsVectorMath.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2010
 * @brief 	RsVectorMath class declaration.
 */

#ifndef RSVECTORMATH_H_
#define RSVECTORMATH_H_

#include "RsStructs.h"

class V2f{
public:
	V2f();
	V2f(float _x, float _y);
	virtual ~V2f();
	V2f operator+(const V2f& _rhs);
	V2f operator-(const V2f& _rhs);
	V2f operator*(float _rhs);

	Tuplef data;
};

class V3f{
public:
	V3f();
	V3f(float _x, float _y, float _z);
	virtual ~V3f();
	V3f operator+(const V3f& _rhs);
	V3f operator-(const V3f& _rhs);
	V3f operator*(float _rhs);

	Triplef data;
};


Tuplef operator+(const Tuplef& _lhs, const Tuplef& _rhs);
Tuplef operator-(const Tuplef& _lhs, const Tuplef& _rhs);
Triplef operator+(const Triplef& _lhs, const Triplef& _rhs);
Triplef operator-(const Triplef& _lhs, const Triplef& _rhs);

#endif /* RSVECTORMATH_H_ */
