/**
 * @file	V3f.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 *
 */

#ifndef V3F_H_
#define V3F_H_
#include <string>
#include <vector>

#include "declarations.h"

namespace ooctools {

/**
 * @class C
 * @brief B
 * L
 */
class V3f
{
public:
	V3f();
	V3f(float, float, float);
	V3f(const float* _data);
	V3f(float);
	V3f(const V3f &_v);
	V3f(const V4f &_v);
//	V3f(const V3f *_v);
	virtual ~V3f();
	float *x;
	float *y;
	float *z;
	static const V3f max(const V3f &_a, const V3f &_b);
	static const V3f min(const V3f &_a, const V3f &_b);
	void minMe(const V3f &_a);
	void maxMe(const V3f &_a);
	void minMe(float _x, float _y, float _z);
	void maxMe(float _x, float _y, float _z);
	const float getX() const {return mPriData[0];};
	const float getY() const {return mPriData[1];};
	const float getZ() const {return mPriData[2];};
	const float getR() const {return mPriData[0];};
	const float getG() const {return mPriData[1];};
	const float getB() const {return mPriData[2];};
	const float* getData() const {return (const float*) mPriData;};
	void setX(float f){mPriData[0]=f;};
	void setY(float f){mPriData[1]=f;};
	void setZ(float f){mPriData[2]=f;};
	const std::string toString() const;
	bool hasSharedComponent(const V3f &_v) const;
	V3f &normalize();
	void addFaceRef(Face *face);
	V3f getAbs() const;
	float calculateLength();

	bool operator>(const V3f &a) const;
//	bool operator>(V3f *a);
	bool operator>=(V3f &a);
	bool operator>=(V3f *a);
	bool operator<(const V3f &a) const;
//	bool operator<(const V3f *a);
	bool operator<=(V3f &a);
	bool operator<=(V3f *a);
	bool operator==(V3f &a);
	bool operator==(V3f *a);
	bool operator!=(V3f &a);
	V3f &operator=(const V3f &a);
	V3f &operator=(const V4f &a);
	V3f &operator/=(const float f);
	V3f &operator+=(const V3f &a);
	V3f &operator*=(float f);
	V3f &operator-=(const V3f &a);
	V3f operator/(float f) const;
	V3f operator+(const V3f &a) const;
	V3f operator-(const V3f &a) const;
	V3f operator*(float f) const;

	static V3f cross(V3f &a, V3f &b);
	static void calcFNormal(Face *face);
	static bool hasSharedComponent(const V3f& _v1, const V3f& _v2);

	// back-reference for vertex-normal-calculation. DO NOT DELETE, DUH!
	std::vector<Face*> *mPriFaceList;

private:
	float mPriData[3];
	int mPriCount;
};

} // Namespace ModelConverter

#endif /* V3F_H_ */
