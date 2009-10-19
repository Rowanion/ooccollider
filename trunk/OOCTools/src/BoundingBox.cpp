/**
 * @file	BoundingBox.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.02.2009
 * @brief   Class definition of BoundingBox
 */

#include "BoundingBox.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <limits>
#include <iostream>
#include <cmath>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include "V3f.h"
#include "V4f.h"
#include "TriBoxTest.h"

using namespace std;

namespace ooctools {

bool BoundingBox::initialized = false;

std::vector<V3f> BoundingBox::normals = std::vector<V3f>();


bool
BoundingBox::hasSharedComponent(const BoundingBox &_bb1, const BoundingBox &_bb2)
{
	return _bb1.hasSharedComponent(_bb2);
}

BoundingBox::BoundingBox() :
	mPriMin((float)numeric_limits<float>::max()), mPriMax((float)-numeric_limits<float>::max()),
//	mPrivMin(3.0f), mPrivMax(3.0f),
	mPriEdgeSizes(0.0f), mPriCenter(0.0f), mTriBoxTest()
{
	computeCenter(mPriCenter);
	if (!BoundingBox::initialized){
		init();
	}
	// note the minus sign on the following line
	// ::min() would return the positive value of the smallest magnitude,
	// not the negative value of the largest magnitude
}

BoundingBox::BoundingBox(float _value) : mPriMin(_value), mPriMax(_value),
mPriEdgeSizes(0.0f), mPriCenter(0.0f), mTriBoxTest()
{
	if (!BoundingBox::initialized){
		init();
	}
}

BoundingBox::BoundingBox(float _valA, float _valB) :
		mPriMin(numeric_limits<float>::max()), mPriMax(-numeric_limits<float>::max()),
//		mPrivMin(2.0f), mPrivMax(2.0f),
		mPriEdgeSizes(0.0f), mPriCenter(0.0f), mTriBoxTest()
{
	if (_valA<_valB) {
		mPriMin = V3f(_valA);
		mPriMax = V3f(_valB);
	}
	else {
		mPriMin = V3f(_valB);
		mPriMax = V3f(_valA);
	}
	computeCenter(mPriCenter);
	if (!BoundingBox::initialized){
		init();
	}
}

BoundingBox::BoundingBox(const V3f& _vA, const V3f& _vB) :
//	mPrivMin(1.0f), mPrivMax(1.0f),
	mPriMin(numeric_limits<float>::max()), mPriMax(-numeric_limits<float>::max()),
	mPriEdgeSizes(0.0f), mPriCenter(0.0f), mTriBoxTest()
{
	if (_vA < _vB) {
		mPriMin = V3f(_vA);
		mPriMax = V3f(_vB);
	}
	else if (_vA > _vB){
		mPriMin = V3f(_vB);
		mPriMax = V3f(_vA);
	}
	else {
		mPriMin = V3f();
		mPriMax = V3f();
		if (_vA.getX() > _vB.getX()){
			*mPriMin.x = _vB.getX();
			*mPriMax.x = _vA.getX();
		}
		else {
			*mPriMin.x = _vA.getX();
			*mPriMax.x = _vB.getX();
		}

		if (_vA.getY() > _vB.getY()){
			*mPriMin.y = _vB.getY();
			*mPriMax.y = _vA.getY();
		}
		else {
			*mPriMin.y = _vA.getY();
			*mPriMax.y = _vB.getY();
		}

		if (_vA.getZ() > _vB.getZ()){
			*mPriMin.z = _vB.getZ();
			*mPriMax.z = _vA.getZ();
		}
		else {
			*mPriMin.z = _vA.getZ();
			*mPriMax.z = _vB.getZ();
		}
	}
	computeCenter(mPriCenter);
	if (!BoundingBox::initialized){
		init();
	}
}

BoundingBox::BoundingBox(const V4f& _vA, const V4f& _vB) :
//	mPrivMin(1.0f), mPrivMax(1.0f),
	mPriMin(numeric_limits<float>::max()), mPriMax(-numeric_limits<float>::max()),
	mPriEdgeSizes(0.0f), mPriCenter(0.0f), mTriBoxTest()
{
	V3f vA (_vA);
	V3f vB (_vB);
	if (vA < vB) {
		mPriMin = vA;
		mPriMax = vB;
	}
	else {
		mPriMin = vB;
		mPriMax = vA;
	}
	if (!BoundingBox::initialized){
		init();
	}
}
/**
 * Copy Constructor
 */
BoundingBox::BoundingBox(const BoundingBox& _bb) : mTriBoxTest()
{
	mPriMax = _bb.getMax();
	mPriMin = _bb.getMin();
	if (!BoundingBox::initialized){
		init();
	}
	computeCenter(mPriCenter);
}

// TODO
BoundingBox::BoundingBox(fs::path bbFile) : mTriBoxTest()
{

	if (!BoundingBox::initialized){
		init();
	}
	computeCenter(mPriCenter);
}

// TODO
BoundingBox::BoundingBox(std::string bbFile) : mTriBoxTest()
{

	if (!BoundingBox::initialized){
		init();
	}
	computeCenter(mPriCenter);
}

BoundingBox::~BoundingBox()
{
//	cout << "BoundingBox..." << endl;
}

void BoundingBox::init()
{
	// face-normals
	BoundingBox::normals.push_back(V3f( 0.0, 0.0,-1.0)); //front 0 1 2 3 4 5 6 7
	BoundingBox::normals.push_back(V3f( 0.0, 0.0, 1.0)); //back 4 5 6 7 0 1 2 3
	BoundingBox::normals.push_back(V3f( 0.0, 1.0, 0.0)); //top 0 1 4 5 2 3 6 7
	BoundingBox::normals.push_back(V3f( 0.0,-1.0, 0.0)); //bottom 2 3 6 7 0 1 4 5
	BoundingBox::normals.push_back(V3f(-1.0, 0.0, 0.0)); //left 1 3 5 7 0 2 4 6
	BoundingBox::normals.push_back(V3f( 1.0, 0.0, 0.0)); //right 0 2 4 6 1 3 5 7

	// vertex-normals
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[0] + BoundingBox::normals[3] + BoundingBox::normals[4]).normalize()); //fbl 3 7 2 1 6 5 0 4
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[0] + BoundingBox::normals[3] + BoundingBox::normals[5]).normalize()); //fbr 2 3 6 0 7 1 4 5
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[0] + BoundingBox::normals[2] + BoundingBox::normals[5]).normalize()); //ftr 0 4 1 2 5 6 3 7
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[0] + BoundingBox::normals[2] + BoundingBox::normals[4]).normalize()); //ftl 1 0 5 3 4 2 7 6

	BoundingBox::normals.push_back(V3f(BoundingBox::normals[1] + BoundingBox::normals[2] + BoundingBox::normals[5]).normalize()); //btr 4 5 0 6 1 7 2 3
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[1] + BoundingBox::normals[2] + BoundingBox::normals[4]).normalize()); //btl 5 1 4 7 0 3 6 2
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[1] + BoundingBox::normals[3] + BoundingBox::normals[4]).normalize()); //bbl 7 6 3 5 2 4 1 0
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[1] + BoundingBox::normals[3] + BoundingBox::normals[5]).normalize()); //bbr 6 2 7 4 3 0 5 1

	// Edge-normals
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[6] + BoundingBox::normals[7]).normalize()); //fb 2 3 0 1 6 7 4 5
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[7] + BoundingBox::normals[8]).normalize()); //fr 0 2 1 3 4 6 5 7
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[8] + BoundingBox::normals[9]).normalize()); //ft 0 1 4 5 2 3 6 7
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[9] + BoundingBox::normals[6]).normalize()); //fl 1 3 0 2 5 7 4 6

	BoundingBox::normals.push_back(V3f(BoundingBox::normals[10] + BoundingBox::normals[11]).normalize()); //bt 4 5 0 1 6 7 2 3
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[11] + BoundingBox::normals[12]).normalize()); //bl 5 7 1 3 4 6 0 2
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[12] + BoundingBox::normals[13]).normalize()); //bb 6 7 2 3 4 5 0 1
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[10] + BoundingBox::normals[13]).normalize()); //br 4 6 5 7 0 2 1 3

	BoundingBox::normals.push_back(V3f(BoundingBox::normals[9] + BoundingBox::normals[11]).normalize()); //tl 1 5 0 4 3 7 2 6
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[6] + BoundingBox::normals[12]).normalize()); //bl 3 7 1 5 2 6 0 4
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[8] + BoundingBox::normals[10]).normalize()); //tr 0 4 1 5 2 6 3 7
	BoundingBox::normals.push_back(V3f(BoundingBox::normals[7] + BoundingBox::normals[13]).normalize()); //br 2 6 0 4 3 7 1 5

	BoundingBox::initialized = true;
}

void
BoundingBox::draw() const
{
//	glColor3ub(255,0,0);
	drawLineStrip();
}

void
BoundingBox::draw(float texCoord) const
{
	drawLineStrip(texCoord);
}

void
BoundingBox::draw(float _r, float _g, float _b) const
{
	glColor3f(_r,_g,_b);
	drawLineStrip();
}
void
BoundingBox::draw(int _r, int _g, int _b) const
{
	glColor3ub(_r,_g,_b);
	drawLineStrip();
}

void
BoundingBox::drawImmediate() const
{
	glBegin(GL_LINES);
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());

		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());

		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());

		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());

		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
	glEnd();
}

void
BoundingBox::drawLineStrip() const
{
	glBegin(GL_LINE_LOOP);
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());

		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
	glEnd();
}

void
BoundingBox::drawLineStrip(float texCoord) const
{
	glBegin(GL_LINE_LOOP);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
	glEnd();

	glBegin(GL_LINES);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);

		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
	glEnd();
}

void
BoundingBox::expand(const V3f &_v)
{
	mPriMax.maxMe(_v);
	mPriMin.minMe(_v);
	computeCenter(mPriCenter);
}

void
BoundingBox::expand(const V4f &_v)
{
	V3f v(_v);
	mPriMax.maxMe(v);
	mPriMin.minMe(v);
	computeCenter(mPriCenter);
}


void
BoundingBox::expand(const float *_v)
{
	expand(_v[0], _v[1], _v[2]);
	computeCenter(mPriCenter);
}

void
BoundingBox::expand(float _x, float _y, float _z)
{
	mPriMax.maxMe(_x, _y, _z);
	mPriMin.minMe(_x, _y, _z);
	computeCenter(mPriCenter);
}

void
BoundingBox::expand(const BoundingBox &_bb)
{
	expand(_bb.getMin());
	expand(_bb.getMax());
	computeCenter(mPriCenter);
}

bool
BoundingBox::hasSharedComponent(const BoundingBox &_bb) const
{
	// TODO WTF? which returns what now?
	return _bb.getMax().hasSharedComponent(getMax());
	return _bb.getMin().hasSharedComponent(getMin());
	return false;
}

std::string
BoundingBox::toString() const
{
	std::string s;
	std::stringstream ss;
	ss << mPriMin.toString() << " - " << mPriMax.toString();
	return ss.str();
}

void
BoundingBox::drawSolid() const
{
	glBegin(GL_QUADS);
		//backface
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());

		//top
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());

		//right
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());

		//left
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());

		//front
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());

		//bottom
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());



	glEnd();
}

void
BoundingBox::drawSolid(float texCoord) const
{
	glBegin(GL_QUADS);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);

		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);

		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);

		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);

		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);

		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);



	glEnd();
}

void
BoundingBox::drawSolidTriFan() const
{
	// center vertex at min,min,min
	glBegin(GL_TRIANGLE_FAN);
		//left
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());

		//front
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());

		//bottom
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
	glEnd();

	// center vertex at max,max,max
	glBegin(GL_TRIANGLE_FAN);
		//top
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ());
		glVertex3f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ());

		//back
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());

		//right
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
	glEnd();

}

void
BoundingBox::drawSolidTriFan(float texCoord) const
{
	// center vertex at min,min,min
	glBegin(GL_TRIANGLE_FAN);
		//left
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);

		//front
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ(), texCoord);

		//bottom
		glVertex4f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ(), texCoord);
	glEnd();

	// center vertex at max,max,max
	glBegin(GL_TRIANGLE_FAN);
		//top
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);
		glVertex4f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMin.getZ(), texCoord);
		glVertex4f(mPriMin.getX(), mPriMax.getY(), mPriMax.getZ(), texCoord);

		//back
		glVertex3f(mPriMin.getX(), mPriMin.getY(), mPriMax.getZ());
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMax.getZ());

		//right
		glVertex3f(mPriMax.getX(), mPriMin.getY(), mPriMin.getZ());
		glVertex3f(mPriMax.getX(), mPriMax.getY(), mPriMin.getZ());
	glEnd();

}

//TODO
void saveToFile(fs::path bbFile);
void saveToFile(std::string bbFile);

void
BoundingBox::computeCenter(V3f &_center) const
{
	V3f edges;
	computeEdgeSizes(edges);
//	V3f step = (mPrivMin.getAbs() + mPrivMax.getAbs()) *0.5f;
//	_center = (mPrivMin + step);
	_center = mPriMin + (edges*0.5f);
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
	return mTriBoxTest.triBoxOverlap((float*)center.getData(),(float*)edges.getData(), triverts);
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
	return mTriBoxTest.triBoxOverlap((float*)center.getData(),(float*)edges.getData(), triverts);
}

bool
BoundingBox::intersects9Plus(const float* _a) const
{
	V3f a(_a);
	V3f b(_a+4);
	V3f c(_a+8);
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
	return mTriBoxTest.triBoxOverlap((float*)center.getData(),(float*)edges.getData(), triverts);

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

	unsigned int matches = 0;
	for (int i=0; i<3; ++i) {
		if (fabs(d.getData()[i]) < 0.5*(s1.getData()[i] + s2.getData()[i]))
			++matches;
	}
	if (matches == 3)
		return true;
	else
		return false;
}

bool
BoundingBox::hasInside3(const float *_v) const
{
	bool result = hasInside(V3f(_v[0], _v[1], _v[2]));
//	delete _temp;
	return result;
}

bool
BoundingBox::hasInside3Plus(const float *_v) const
{
	bool result = hasInside(V4f(_v[0], _v[1], _v[2], _v[3]));
//	delete _temp;
	return result;
}

bool
BoundingBox::hasInside9(const float *_v) const
{
	return (hasInside3(_v) && hasInside3(_v+3) && hasInside3(_v+6));

}

bool
BoundingBox::hasInside9Plus(const float *_v) const
{
	return (hasInside3Plus(_v) && hasInside3Plus(_v+4) && hasInside3Plus(_v+8));
}

// vertex
bool
BoundingBox::hasInside(float _f1, float _f2, float _f3) const
{
	bool result = hasInside(V3f(_f1, _f2, _f3));
//	delete _temp;
	return result;
}

// dreieck
bool
BoundingBox::hasInside(const V3f& _v1, const V3f& _v2, const V3f& _v3) const
{
	if (hasInside(_v1) && hasInside(_v2) && hasInside(_v3)) return true;
	else return false;
}

bool
BoundingBox::hasInside(const V4f& _v1, const V4f& _v2, const V4f& _v3) const
{
	V3f v1(_v1);
	V3f v2(_v2);
	V3f v3(_v3);
	if (hasInside(v1) && hasInside(v2) && hasInside(v3)) return true;
	else return false;
}

bool
BoundingBox::hasInside(const V3f& _v) const
{
	if (_v >= mPriMin && _v < mPriMax) return true;
	else return false;
}

bool
BoundingBox::hasInside(const V4f& _v) const
{
	V3f temp(_v);
	if (temp > mPriMin && temp < mPriMax) return true;
	else return false;
}

bool
BoundingBox::hasInside(const BoundingBox& _b) const
{
	if (hasInside(_b.getMin()) && hasInside(_b.getMax()))
		return true;
	else return false;
}

void
BoundingBox::computeEdgeSizes(V3f &_sizes) const
{
	V3f e = (mPriMax-mPriMin);
	_sizes = e.getAbs();
}

float
BoundingBox::computeDiameter() const
{
	return (mPriMax-mPriMin).getAbs().calculateMagnitude();
}

BoundingBox&
BoundingBox::operator=(const BoundingBox& _bb)
{
	if (*this != _bb){
		mPriMax = _bb.getMax();
		mPriMin = _bb.getMin();
	}
	return *this;
}

bool
BoundingBox::operator==(const BoundingBox& _bb)
{
	if (mPriMax==_bb.getMax() && mPriMin==_bb.getMin())
		return true;
	else return false;
}

bool
BoundingBox::operator!=(const BoundingBox& _bb)
{
	return !(*this == _bb);
}

void
BoundingBox::saveToFile(fs::path bbFile){
	fs::ofstream of;
	of.open(bbFile, ios::binary | ios::out);
	of.seekp(0, ios::beg);
	of.write((char*) mPriMin.x, sizeof(float));
	of.write((char*) mPriMin.y, sizeof(float));
	of.write((char*) mPriMin.z, sizeof(float));
	of.write((char*) mPriMax.x, sizeof(float));
	of.write((char*) mPriMax.y, sizeof(float));
	of.write((char*) mPriMax.z, sizeof(float));
	of.close();
}

BoundingBox
BoundingBox::fromFile(fs::path bbFile)
{
	V3f min, max;
	fs::ifstream input;
	input.open(bbFile, ios::binary | ios::in);
	input.seekg(0, ios::beg);
	input.read((char*) min.x, sizeof(float));
	input.read((char*) min.y, sizeof(float));
	input.read((char*) min.z, sizeof(float));
	input.read((char*) max.x, sizeof(float));
	input.read((char*) max.y, sizeof(float));
	input.read((char*) max.z, sizeof(float));
	input.close();
	BoundingBox bb(min, max);
	return bb;
}

const unsigned BoundingBox::getMinDotIdx(const V3f& view)
{
	unsigned minIdx = 0;
	float minDot = 2.0f;
	for (unsigned i=0; i< 14; ++i){
		float temp = BoundingBox::normals[i].dot(view);
		if (temp < minDot){
			minIdx = i;
			minDot = temp;
		}
//		temp = normal[i]
	}
	return minIdx;
//	V3f temp
}

V3f
BoundingBox::getCenterLeft() const
{
	V3f temp = mPriCenter;
	temp.setX(mPriMin.getX());
	return temp;
}

V3f
BoundingBox::getCenterRight() const
{
	V3f temp = mPriCenter;
	temp.setX(mPriMax.getX());
	return temp;
}

V3f
BoundingBox::getCenterTop() const
{
	V3f temp = mPriCenter;
	temp.setY(mPriMax.getY());
	return temp;
}

V3f
BoundingBox::getCenterBottom() const
{
	V3f temp = mPriCenter;
	temp.setY(mPriMin.getY());
	return temp;
}

V3f
BoundingBox::getCenterFront() const
{
	V3f temp = mPriCenter;
	temp.setZ(mPriMin.getZ());
	return temp;
}

V3f
BoundingBox::getCenterBack() const
{
	V3f temp = mPriCenter;
	temp.setZ(mPriMax.getZ());
	return temp;
}

V3f
BoundingBox::getFrontTopLeft() const
{
	V3f temp = mPriMin;
	temp.setY(mPriMax.getY());
	return temp;
}

V3f
BoundingBox::getFrontTopRight() const
{
	V3f temp = mPriMax;
	temp.setZ(mPriMin.getZ());
	return temp;
}

V3f
BoundingBox::getFrontBottomLeft() const
{
	V3f temp = mPriMin;
	return temp;
}

V3f
BoundingBox::getFrontBottomRight() const
{
	V3f temp = mPriMin;
	temp.setX(mPriMax.getX());
	return temp;
}

V3f
BoundingBox::getBackTopLeft() const
{
	V3f temp = mPriMax;
	temp.setX(mPriMin.getX());
	return temp;
}

V3f
BoundingBox::getBackTopRight() const
{
	V3f temp = mPriMax;
	return temp;
}

V3f
BoundingBox::getBackBottomLeft() const
{
	V3f temp = mPriMin;
	temp.setZ(mPriMax.getZ());
	return temp;
}

V3f
BoundingBox::getBackBottomRight() const
{
	V3f temp = mPriMax;
	temp.setY(mPriMin.getY());
	return temp;
}

V3f
BoundingBox::getCenterTopLeft() const
{
	V3f temp = mPriMin;
	temp.setZ(mPriCenter.getZ());
	temp.setY(mPriMax.getY());
	return temp;
}

V3f
BoundingBox::getCenterTopRight() const
{
	V3f temp = mPriMax;
	temp.setZ(mPriCenter.getZ());
	return temp;
}

V3f
BoundingBox::getCenterBottomLeft() const
{
	V3f temp = mPriMin;
	temp.setZ(mPriCenter.getZ());
	return temp;
}

V3f
BoundingBox::getCenterBottomRight() const
{
	V3f temp = mPriMin;
	temp.setZ(mPriCenter.getZ());
	temp.setX(mPriMax.getX());
	return temp;
}


V3f
BoundingBox::getCenterFrontRight() const
{
	V3f temp = mPriMin;
	temp.setY(mPriCenter.getY());
	temp.setX(mPriMax.getX());
	return temp;
}

V3f
BoundingBox::getCenterFrontLeft() const
{
	V3f temp = mPriMin;
	temp.setY(mPriCenter.getY());
	return temp;
}

V3f
BoundingBox::getCenterFrontTop() const
{
	V3f temp = mPriMin;
	temp.setX(mPriCenter.getX());
	temp.setY(mPriMax.getY());
	return temp;
}

V3f
BoundingBox::getCenterFrontBottom() const
{
	V3f temp = mPriMin;
	temp.setX(mPriCenter.getX());
	return temp;
}

V3f
BoundingBox::getCenterBackRight() const
{
	V3f temp = mPriMax;
	temp.setY(mPriCenter.getY());
	return temp;
}

V3f
BoundingBox::getCenterBackLeft() const
{
	V3f temp = mPriMax;
	temp.setY(mPriCenter.getY());
	temp.setX(mPriMin.getX());
	return temp;
}

V3f
BoundingBox::getCenterBackTop() const
{
	V3f temp = mPriMax;
	temp.setX(mPriCenter.getX());
	return temp;
}

V3f
BoundingBox::getCenterBackBottom() const
{
	V3f temp = mPriMax;
	temp.setX(mPriCenter.getX());
	temp.setY(mPriMin.getY());
	return temp;
}

} // end of Namespace OOCTools
