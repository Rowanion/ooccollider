/*
 * BoundingBox.cpp
 *
 *  Created on: 22.02.2009
 *      Author: ava
 */

#include "BoundingBox.h"

#include "V3f.h"
#include <cstddef>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <sstream>
#include <string>
#include <limits>
#include <iostream>
#include <cmath>

using namespace std;

namespace ooctools {

extern "C" int planeBoxOverlap(float normal[3],float d, float maxbox[3]);
extern "C" int triBoxOverlap(const float boxcenter[3],const float boxhalfsize[3],const float triverts[3][3]);

bool
BoundingBox::hasSharedComponent(const BoundingBox &_bb1, const BoundingBox &_bb2)
{
	return _bb1.hasSharedComponent(_bb2);
}

BoundingBox::BoundingBox() :
	mPrivMin((float)numeric_limits<float>::max()), mPrivMax((float)-numeric_limits<float>::max()),
//	mPrivMin(3.0f), mPrivMax(3.0f),
	mPrivEdgeSizes(0.0f), mPrivCenter(0.0f)
{
	// note the minus sign on the following line
	// ::min() would return the positive value of the smallest magnitude,
	// not the negative value of the largest magnitude
}

BoundingBox::BoundingBox(float _value) : mPrivMin(_value), mPrivMax(_value),
mPrivEdgeSizes(0.0f), mPrivCenter(0.0f)
{
}

BoundingBox::BoundingBox(float _valA, float _valB) :
		mPrivMin(numeric_limits<float>::max()), mPrivMax(-numeric_limits<float>::max()),
//		mPrivMin(2.0f), mPrivMax(2.0f),
		mPrivEdgeSizes(0.0f), mPrivCenter(0.0f)
{
	if (_valA<_valB) {
		mPrivMin = V3f(_valA);
		mPrivMax = V3f(_valB);
	}
	else {
		mPrivMin = V3f(_valB);
		mPrivMax = V3f(_valA);
	}
}

BoundingBox::BoundingBox(const V3f& _vA, const V3f& _vB) :
//	mPrivMin(1.0f), mPrivMax(1.0f),
	mPrivMin(numeric_limits<float>::max()), mPrivMax(-numeric_limits<float>::max()),
	mPrivEdgeSizes(0.0f), mPrivCenter(0.0f)
{
	if (_vA < _vB) {
		mPrivMin = V3f(_vA);
		mPrivMax = V3f(_vB);
	}
	else {
		mPrivMin = V3f(_vB);
		mPrivMax = V3f(_vA);
	}
}

BoundingBox::BoundingBox(const V4f& _vA, const V4f& _vB) :
//	mPrivMin(1.0f), mPrivMax(1.0f),
	mPrivMin(numeric_limits<float>::max()), mPrivMax(-numeric_limits<float>::max()),
	mPrivEdgeSizes(0.0f), mPrivCenter(0.0f)
{
	V3f vA (_vA);
	V3f vB (_vB);
	if (vA < vB) {
		mPrivMin = vA;
		mPrivMax = vB;
	}
	else {
		mPrivMin = vB;
		mPrivMax = vA;
	}
}

BoundingBox::~BoundingBox()
{
//	cout << "BoundingBox..." << endl;
}

void
BoundingBox::draw()
{
	glColor3ub(255,0,0);
	drawLineStrip();
}
void
BoundingBox::draw(float _r, float _g, float _b)
{
	glColor3f(_r,_g,_b);
	drawLineStrip();
}
void
BoundingBox::draw(int _r, int _g, int _b)
{
	glColor3ub(_r,_g,_b);
	drawLineStrip();
}

void
BoundingBox::drawImmediate()
{
	glBegin(GL_LINES);
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());
	glEnd();
}

void
BoundingBox::drawLineStrip()
{
	glBegin(GL_LINE_LOOP);
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());

		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
	glEnd();
}

void
BoundingBox::expand(const V3f &_v)
{
	mPrivMax.maxMe(_v);
	mPrivMin.minMe(_v);
}

void
BoundingBox::expand(const V4f &_v)
{
	V3f v(_v);
	mPrivMax.maxMe(v);
	mPrivMin.minMe(v);
}

void
BoundingBox::expand(const float *_v)
{
	expand(_v[0], _v[1], _v[2]);
}
void
BoundingBox::expand(float _x, float _y, float _z)
{
	mPrivMax.maxMe(_x, _y, _z);
	mPrivMin.minMe(_x, _y, _z);
}

void
BoundingBox::expand(const BoundingBox &_bb)
{
	expand(_bb.getMin());
	expand(_bb.getMax());
}

bool
BoundingBox::hasSharedComponent(const BoundingBox &_bb) const
{
	// TODO WTF? which returns waht now?
	return _bb.getMax().hasSharedComponent(getMax());
	return _bb.getMin().hasSharedComponent(getMin());
	return false;
}

std::string
BoundingBox::toString()
{
	std::string s;
	std::stringstream ss;
	ss << mPrivMin.toString() << " - " << mPrivMax.toString();
	return ss.str();
}

void
BoundingBox::drawSolid()
{
	glBegin(GL_QUADS);
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());

		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());

		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMax.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());

		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMin.getZ());
		glVertex3f(mPrivMax.getX(), mPrivMin.getY(), mPrivMax.getZ());
		glVertex3f(mPrivMin.getX(), mPrivMin.getY(), mPrivMax.getZ());



	glEnd();
}

/**
 *  @brief Computes center of BB.
 *  @return   a pointer to a new V3f Reference containing the center coordinates of BB.
 *
 *  This is not an ordinary getter. This calculates the geometrical center of the BoundingBox
 *  and returns a pointer to it.
 *  Attention: the pointer is NOT kept by BB. The caller of the function has to make sure to
 *  clean it up after use!
 */
void
BoundingBox::computeCenter(V3f &_center) const
{
	_center = ((mPrivMax+mPrivMin)*0.5);
}

bool
BoundingBox::intersects(const V3f &_a, const V3f &_b, const V3f &_c) const
{
	V3f center;
	computeCenter(center);
	V3f edges;
	computeEdgeSizes(edges);
	edges *= 0.5f;
	float triverts[3][3];
	for (int i=0; i<3; ++i){
		triverts[0][i] = ((float*)_a.getData())[i];
		triverts[1][i] = ((float*)_b.getData())[i];
		triverts[2][i] = ((float*)_c.getData())[i];
	}
	return triBoxOverlap((float*)center.getData(),(float*)edges.getData(), triverts);
}

bool
BoundingBox::intersects(const V4f &_a, const V4f &_b, const V4f &_c) const
{
	V3f a(_a);
	V3f b(_b);
	V3f c(_c);
	V3f center;
	computeCenter(center);
	V3f edges;
	computeEdgeSizes(edges);
	edges *= 0.5f;
	float triverts[3][3];
	for (int i=0; i<3; ++i){
		triverts[0][i] = ((float*)a.getData())[i];
		triverts[1][i] = ((float*)b.getData())[i];
		triverts[2][i] = ((float*)c.getData())[i];
	}
	return triBoxOverlap((float*)center.getData(),(float*)edges.getData(), triverts);
}

bool
BoundingBox::intersects(const BoundingBox &_b) const
{
	V3f d;
	computeCenter(d);
	V3f bCenter;
	_b.computeCenter(bCenter);
	d-= bCenter;
	V3f s1, s2;
	computeEdgeSizes(s1);
	_b.computeEdgeSizes(s2);

	for (int i=0; i<3; ++i) {
		if (fabs(d.getData()[i]) < 0.5*(s1.getData()[i] + s2.getData()[i])) return true;
	}
	return false;
}

// vertex
bool
BoundingBox::isInside3(const float *_v) const
{
	bool result = isInside(V3f(_v[0], _v[1], _v[2]));
//	delete _temp;
	return result;
}

// dreieck
bool
BoundingBox::isInside9(const float *_v) const
{
	return (isInside3(_v) && isInside3(_v+3) && isInside3(_v+6));

}

// vertex
bool
BoundingBox::isInside(float _f1, float _f2, float _f3) const
{
	bool result = isInside(V3f(_f1, _f2, _f3));
//	delete _temp;
	return result;
}

// dreieck
bool
BoundingBox::isInside(const V3f& _v1, const V3f& _v2, const V3f& _v3) const
{
	if (isInside(_v1) && isInside(_v2) && isInside(_v3)) return true;
	else return false;
}

bool
BoundingBox::isInside(const V4f& _v1, const V4f& _v2, const V4f& _v3) const
{
	V3f v1(_v1);
	V3f v2(_v2);
	V3f v3(_v3);
	if (isInside(v1) && isInside(v2) && isInside(v3)) return true;
	else return false;
}

/**
 *  @brief Tests if given V3f is inside this one.
 *  @param A pointer to a V3f.
 *  @return   true if the given V3f-Ptr is completely inside of this BB. false else.
 *
 *  Performs a test, if the given V3f is completely inside this one.
 */
bool
BoundingBox::isInside(const V3f& _v) const
{
	if (_v > mPrivMin && _v < mPrivMax) return true;
	else return false;
}

bool
BoundingBox::isInside(const V4f& _v) const
{
	V3f temp(_v);
	if (temp > mPrivMin && temp < mPrivMax) return true;
	else return false;
}

/**
 *  @brief Tests if given BB is inside this one.
 *  @param A pointer to a BoundingBox.
 *  @return   true if the given BB-Ptr is completely inside of this BB. false else.
 *
 *  Performs a test, if the given BB is completely inside this one.
 */
bool
BoundingBox::isInside(const BoundingBox& _b) const
{
	if (isInside(_b.getMin()) && isInside(_b.getMax()))
		return true;
	else return false;
}

/**
 *  @brief Tests if given V3f is outside this BB.
 *  @param A pointer to a V3f Vertex.
 *  @return   true if the given V3f-Ptr is completely inside of this BB. False else.
 *
 *  Performs a test, if the given BB is completely outside this one.
 *  It's just an inversion of isInside(V3f*);
 */
bool
BoundingBox::isOutside(const V3f& _v) const
{
	return !isInside(_v);
}

bool
BoundingBox::isOutside(const V4f& _v) const
{
	V3f temp (_v);
	return !isInside(temp);
}

/**
 *  @brief Tests if given BB is outside this one.
 *  @param A pointer to a BoundingBox.
 *  @return   true if the given BB-Ptr is completely outside of this BB. false else.
 *
 *  Performs a test, if the given BB is completely outside this one.
 *  This does not include the intersecting case. For this please use intersects()
 */
bool
BoundingBox::isOutside(const BoundingBox &_b) const
{
	if (!_b.isInside(*this) && isOutside(_b.getMax()) && isOutside(_b.getMin()))
		return true;
	else return false;
}

void
BoundingBox::computeEdgeSizes(V3f &_sizes) const
{
	_sizes = (mPrivMax-mPrivMin);
}

float
BoundingBox::computeDiameter() const
{
	return (mPrivMax-mPrivMin).getAbs().calculateLength();
}

} // end of Namespace OOCTools
