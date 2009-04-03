/**
 * @file	V4f.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 *
 */

#ifndef V4F_H_
#define V4F_H_
#include <string>
#include <vector>

#include "declarations.h"

namespace ooctools {

/**
 * @class C
 * @brief B
 * L
 */
class V4f
{
public:
	V4f();
	V4f(float, float, float, float);
	V4f(const float *_data);
	V4f(float);
	V4f(const V4f& _v);
	V4f(const V3f& _v);
//	V4f(const V4f *_v);
	virtual ~V4f();
	float *x;
	float *y;
	float *z;
	float *w;
	static const V4f max(const V4f &_a, const V4f &_b);
	static const V4f min(const V4f &_a, const V4f &_b);
	void minMe(const V4f &_a);
	void maxMe(const V4f &_a);
	void minMe(float _x, float _y, float _z, float _w);
	void maxMe(float _x, float _y, float _z, float _w);
	const float getX() const {return mPriData[0];};
	const float getY() const {return mPriData[1];};
	const float getZ() const {return mPriData[2];};
	const float getW() const {return mPriData[3];};
	const float getR() const {return mPriData[0];};
	const float getG() const {return mPriData[1];};
	const float getB() const {return mPriData[2];};
	const float getA() const {return mPriData[3];};
	const float* getData() const {return (const float*) mPriData;};
	void setX(float f){mPriData[0]=f;};
	void setY(float f){mPriData[1]=f;};
	void setZ(float f){mPriData[2]=f;};
	void setW(float f){mPriData[3]=f;};
	const std::string toString() const;
	bool hasSharedComponent(const V4f &_v) const;
	V4f &normalize();
	void addFaceRef(Face *face);
	V4f getAbs() const;

	bool operator>(const V4f &a) const;
//	bool operator>(V4f *a);
	bool operator>=(V4f &a);
	bool operator>=(V4f *a);
	bool operator<(const V4f &a) const;
//	bool operator<(const V4f *a);
	bool operator<=(V4f &a);
//	bool operator<=(V4f *a);
	bool operator==(V4f &a);
//	bool operator==(V4f *a);
	bool operator!=(V4f &a);
	V4f& operator=(const V4f &a);
	V4f& operator=(const V3f& a);
	V4f& operator/=(const float f);
	V4f& operator+=(const V4f &a);
	V4f& operator*=(float f);
	V4f& operator-=(const V4f &a);
	V4f operator/(float f) const;
	V4f operator+(const V4f &a) const;
	V4f operator-(const V4f &a) const;
	V4f operator*(float f) const;

	static V4f cross(V4f &a, V4f &b);
	static void calcFNormal(Face *face);
	static bool hasSharedComponent(const V4f& _v1, const V4f& _v2);

	// back-reference for vertex-normal-calculation. DO NOT DELETE, DUH!
	std::vector<Face*> *mPriFaceList;

private:
	float mPriData[4];
	int mPriCount;
};

} // Namespace ooctools

#endif /* V4F_H_ */
