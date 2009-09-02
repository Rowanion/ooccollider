/**
 * @file	Triangle.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 16.05.2009
 * @brief 	Triangle class definition.
 */

#include "Triangle.h"

#include <cstring>
#include <string>
#include <sstream>
#include <cmath>
#include <utility>

#include "BoundingBox.h"

using namespace std::rel_ops;

namespace ooctools {

Triangle::Triangle() : mVertices(0), mNormals(0), mArea(0.0), mBb()
{

}

Triangle::Triangle(const float* vertices, const char* normals) : mVertices(0), mNormals(0), mArea(0.0)
{
	mVertices = new float[3*4];
	memcpy(mVertices, vertices, sizeof(float) * 3 * 4);
	mNormals = new char[3*4];
	memcpy(mNormals, normals, sizeof(char) * 3 * 4);
	mArea = calcArea();
	calcBb();
}

Triangle::Triangle(const V4N4& a, const V4N4& b, const V4N4& c) : mVertices(0), mNormals(0), mArea(0.0)
{
	mVertices = new float[3*4];
	memcpy(mVertices, a.v, sizeof(float) * 4);
	memcpy(mVertices+4, b.v, sizeof(float) * 4);
	memcpy(mVertices+8, c.v, sizeof(float) * 4);
	mNormals = new char[3*4];
	memcpy(mNormals, a.n, sizeof(char) * 4);
	memcpy(mNormals+4, b.n, sizeof(char) * 4);
	memcpy(mNormals+8, c.n, sizeof(char) * 4);
	mArea = calcArea();
	calcBb();
}

Triangle::Triangle(const Triangle& rhs) : mVertices(0), mNormals(0), mArea(0.0)
{
	float* vTemp = new float[4*3];
	char* nTemp = new char[4*3];
	memcpy(vTemp, rhs.getVertices(),4*3*sizeof(float));
	memcpy(nTemp, rhs.getNormals(),4*3*sizeof(char));

	delete[] mNormals;
	mNormals = nTemp;
	delete[] mVertices;
	mVertices = vTemp;
	mArea = calcArea();
	calcBb();
}

Triangle::~Triangle() {
	delete[] mVertices;
	mVertices = 0;
	delete[] mNormals;
	mNormals = 0;
}

const float* Triangle::getVertices() const
{
	return mVertices;
}

const char* Triangle::getNormals() const
{
	return mNormals;
}

void Triangle::setData(const float* vertices, const char* normals)
{
	if (mVertices!=0){
		delete[] mVertices;
		mVertices = 0;
	}
	if (mNormals!=0){
		delete[] mNormals;
		mNormals = 0;
	}
	mVertices = new float[3*4];
	memcpy(mVertices, vertices, sizeof(float) * 3 * 4);
	mNormals = new char[3*4];
	memcpy(mNormals, normals, sizeof(char) * 3 * 4);
	mArea = calcArea();
	mBb = BoundingBox();
	calcBb();
}

float Triangle::calcArea()
{

	V3f v = V3f(mVertices+4)-V3f(mVertices);
	V3f w = V3f(mVertices+8)-V3f(mVertices);
	V3f cross = V3f::cross(v, w);

	float area = cross.calculateMagnitude()/2.0f;

	return area;
}

float Triangle::calcDistance(const float* a, const float* b)
{
	long double bax = b[0]-a[0];
	long double bay = b[1]-a[1];
	long double baz = b[2]-a[2];
	return sqrt((bax*bax) + (bay*bay) + (baz*baz));
}

void Triangle::calcBb()
{
	mBb = BoundingBox();
	mBb.expand(mVertices);
	mBb.expand(mVertices+4);
	mBb.expand(mVertices+8);
}

const BoundingBox& Triangle::getBb() const
{
	return mBb;
}

Triangle& Triangle::operator=(const Triangle& rhs)
{
	float* vTemp = new float[4*3];
	char* nTemp = new char[4*3];
	memcpy(vTemp, rhs.getVertices(),4*3*sizeof(float));
	memcpy(nTemp, rhs.getNormals(),4*3*sizeof(char));

	delete[] mNormals;
	mNormals = nTemp;
	delete[] mVertices;
	mVertices = vTemp;
	mArea = calcArea();
	calcBb();
	return *this;
}

void
Triangle::setVertices(const float* vertices)
{
	if (mVertices!=0){
		delete[] mVertices;
		mVertices = 0;
	}
	mVertices = new float[3*4];
	memcpy(mVertices, vertices, sizeof(float) * 3 * 4);
	mArea = calcArea();
	mBb = BoundingBox();
	calcBb();
}

void
Triangle::setNormals(const char* normals)
{
	if (mNormals!=0){
		delete[] mNormals;
		mNormals = 0;
	}
	mNormals = new char[3*4];
	memcpy(mNormals, normals, sizeof(char) * 3 * 4);
}

const std::string
Triangle::toString() const
{
	std::stringstream ss;
	for (unsigned i=0; i< 3; ++i){
		ss << " v" << i+1 << "(";
		for (unsigned j=0; j< 4; ++j){
			ss << mVertices[(i*4)+j] << ", ";
		}
		ss << "), ";
	}
	ss << std::endl;
	for (unsigned i=0; i< 3; ++i){
		ss << " n" << i+1 << "(";
		for (unsigned j=0; j< 4; ++j){
			ss << (int)mNormals[(i*4)+j] << ", ";
		}
		ss << "), ";
	}
	ss << std::endl;

	return ss.str();
}

void
Triangle::sort()
{
	unsigned min = 0;

	/*
	 * 112 121 211 123 321
	 */

	if (mVertices[0] > mVertices[4])
		min = 4;
	if(min ==4 && mVertices[min] > mVertices[8])
		min = 8;
	else if(min ==0 && mVertices[min] >= mVertices[8])
		min = 8;

	if (min > 0){
		float vertex[4];
		char normal[4];
		memcpy(vertex, mVertices+min, sizeof(float)*4);
		memcpy(normal, mNormals+min, sizeof(char)*4);
		if (min == 4){
			memcpy(mVertices+min, mVertices+8, sizeof(float)*4);
			memcpy(mVertices+8, mVertices, sizeof(float)*4);
			memcpy(mVertices, vertex, sizeof(float)*4);
			memcpy(mNormals+min, mNormals+8, sizeof(char)*4);
			memcpy(mNormals+8, mNormals, sizeof(char)*4);
			memcpy(mNormals, normal, sizeof(char)*4);
		}
		else if (min == 8){
			memcpy(mVertices+min, mVertices+4, sizeof(float)*4);
			memcpy(mVertices+4, mVertices, sizeof(float)*4);
			memcpy(mVertices, vertex, sizeof(float)*4);
			memcpy(mNormals+min, mNormals+4, sizeof(char)*4);
			memcpy(mNormals+4, mNormals, sizeof(char)*4);
			memcpy(mNormals, normal, sizeof(char)*4);
		}
	}
}

bool
Triangle::operator==(const Triangle& rhs) const
{
	const char* normals = rhs.getNormals();
	const float* vertices= rhs.getVertices();
	for (unsigned i=0; i<12; ++i){
		if (mVertices[i]!=vertices[i])
			return false;
		// omit comparison of 4th normal component because it just
		// useless filling.
		if ((i+1)%4!=0 && mNormals[i]!=normals[i])
			return false;
	}
	return true;
}

bool
Triangle::operator!=(const Triangle& rhs) const
{
	return !(*this == rhs);
}

bool
Triangle::operator<(const Triangle& rhs) const
{
	const float* v = rhs.getVertices();
	return mVertices[0] < v[0];
}

} // ooctools
