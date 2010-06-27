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

/**
 * @class RsV2f
 * @brief A 2-component float vector with basic operations.
 */
class RsV2f{
public:
	RsV2f();
	RsV2f(float _x, float _y);
	virtual ~RsV2f();
	RsV2f operator+(const RsV2f& _rhs);
	RsV2f operator-(const RsV2f& _rhs);
	RsV2f operator*(float _rhs);
	RsV2f operator*(const RsV2f& _rhs);

	RsTuplef data;
};

/**
 * @class RsV3f
 * @brief A 3-component float vector with basic operations.
 */
class RsV3f{
public:
	RsV3f();
	RsV3f(float _x, float _y, float _z);
	virtual ~RsV3f();
	RsV3f operator+(const RsV3f& _rhs);
	RsV3f operator-(const RsV3f& _rhs);
	RsV3f operator*(float _rhs);
	RsV3f operator*(const RsV3f& _rhs);

	RsTriplef data;
};


RsTuplef operator+(const RsTuplef& _lhs, const RsTuplef& _rhs);
RsTuplef operator-(const RsTuplef& _lhs, const RsTuplef& _rhs);
RsTriplef operator+(const RsTriplef& _lhs, const RsTriplef& _rhs);
RsTriplef operator-(const RsTriplef& _lhs, const RsTriplef& _rhs);

/**
 * @brief As the function name-suggests, this function normalizes the given vector-data.
 * @param _data the basal vector data.
 */
void normalize(RsTriplec* _data);

#endif /* RSVECTORMATH_H_ */
