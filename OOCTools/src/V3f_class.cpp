/*
 * V3f.cpp
 *
 *  Created on: 30.01.2009
 *      Author: ava
 */

#include "V3f_class.h"
#define _USE_MATH_DEFINES

#include <math.h>
#include <GL/gl.h>

namespace ooctools_ {

V3f_class::V3f_class()
	: x(0.0f), y(0.0f), z(0.0f) {
	updateArray();
}

V3f_class::V3f_class(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z) {
	updateArray();
}

V3f_class::V3f_class(float tempVal[])
	: x(tempVal[0]), y(tempVal[1]), z(tempVal[2]) {
	updateArray();
}

V3f_class::V3f_class(V3f_class *tempVal)
	: x(tempVal->x), y(tempVal->y), z(tempVal->z) {
	updateArray();
}

V3f_class::~V3f_class() {
	// TODO Auto-generated destructor stub
}

void V3f_class::add(float *b)
{
	x=b[0];
	y+=b[1];
	z+=b[2];
	updateArray();
}
void V3f_class::add(V3f_class *b)
{
	x+=b->x;
	y+=b->y;
	z+=b->z;
	updateArray();
}
void V3f_class::add(float x,float y, float z)
{
	x+=x;
	y+=y;
	z+=z;
	updateArray();
}
void V3f_class::add(float scalar)
{
	x+=scalar;
	y+=scalar;
	z+=scalar;
	updateArray();
}
V3f_class *V3f_class::sub(V3f_class *a, V3f_class *b)
{
	return new V3f_class(a->x-b->x, a->y-b->y,a->z-b->z);
}
void V3f_class::mul(float *b)
{
	x*=b[0];
	y*=b[1];
	z*=b[2];
	updateArray();
}
void V3f_class::mul(V3f_class *b)
{
	x*=b->x;
	y*=b->y;
	z*=b->z;
	updateArray();
}
void V3f_class::mul(float x,float y, float z)
{
	x*=x;
	y*=y;
	z*=z;
	updateArray();
}
void V3f_class::mul(float scalar)
{
	x*=scalar;
	y*=scalar;
	z*=scalar;
	updateArray();
}
V3f_class *V3f_class::mul(V3f_class *a, V3f_class *b)
{
	return new V3f_class(a->x*b->x, a->y*b->y, a->z*b->z);
}

V3f_class *V3f_class::mul(V3f_class *a, float b)
{
	return new V3f_class(a->x*b, a->y*b, a->z*b);
}
float V3f_class::dot(float *a, float *b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}
float V3f_class::dot(V3f_class *a, V3f_class *b)
{
	return a->x*b->x+a->y*b->y+a->z*b->z;
}
float V3f_class::dot(V3f_class *a)
{
	return x*a->x+y*a->y+z*a->z;
}
float V3f_class::dot(V3f_class *a, float *b)
{
	return a->x*b[0]+a->y*b[1]+a->z*b[2];
}
V3f_class *V3f_class::cross (V3f_class *b)
{
	return new V3f_class(y*b->z-z*b->y,-(x*b->z-z*b->x),x*b->y-y*b->x);
}
V3f_class *V3f_class::cross (V3f_class *a, V3f_class *b)
{
	return new V3f_class(a->y*b->z-a->z*b->y,-(a->x*b->z-a->z*b->x),a->x*b->y-a->y*b->x);
}

const float *V3f_class::asArray()
{
	return (const float*) data;
}

std::vector<GLfloat> *V3f_class::asVector(){
	std::vector<GLfloat> *temp = new std::vector<GLfloat>;
	temp->push_back((GLfloat)x);
	temp->push_back((GLfloat)y);
	temp->push_back((GLfloat)z);

	return temp;

}

void V3f_class::setV3f(V3f_class *tempVal)
{
	x=tempVal->x;
	y=tempVal->y;
	z=tempVal->z;
	updateArray();
}
void V3f_class::setV3f(float *tempVal)
{
	x=tempVal[0];
	y=tempVal[1];
	z=tempVal[2];
	updateArray();
}
void V3f_class::setV3f(float _x, float _y, float _z)
{
	x=_x;
	y=_y;
	z=_z;
	updateArray();
}
void V3f_class::normalize()
{
	double reciSqrt = 1/sqrt(pow(x,2)+pow(y,2)+pow(z,2));
	x*=reciSqrt;
	y*=reciSqrt;
	z*=reciSqrt;
	updateArray();
}
V3f_class *V3f_class::getNormalized()
{
	V3f_class *result = new V3f_class(this);
	result->normalize();
	return result;
}
float V3f_class::getLength()
{
	return (float)sqrt((x*x)+(y*y)+(z*z));
}

float V3f_class::getSquaredLength()
{
	return (x*x)+(y*y)+(z*z);
}
void V3f_class::LoadZero()
{
	x=0.0f;
	y=0.0f;
	z=0.0f;
	updateArray();
}
void V3f_class::LoadOne()
{
	x=1.0f;
	y=1.0f;
	z=1.0f;
	updateArray();
}

//linear interpolate
V3f_class *V3f_class::lerp(V3f_class *p1, V3f_class *p2, float percent)
{
	V3f_class *p3 = V3f_class::mul(V3f_class::sub(p2, p1), percent);
	p3->add(p1);
	return p3;
	// x1 + t * (x2 - x1)
	// y1 + t * (y2 - y1)
}

V3f_class *V3f_class::lerp(V3f_class *v2, float factor)
{
	V3f_class *temp = V3f_class::mul(this, 1.0f-factor);
	temp->add(V3f_class::mul(v2, factor));
	return temp;
}

V3f_class *V3f_class::quadraticInterpolate(V3f_class *v2, V3f_class *v3, float factor)
{
	V3f_class *temp = V3f_class::mul(this, (1.0f-factor)*(1.0f-factor));
	temp->add(V3f_class::mul(v2, (1.0f-factor)*2.0f));
	temp->add(V3f_class::mul(v3, factor*factor));
	return temp;
}
/*
// TODO outputstream benutzen!!!
std::string V3f::toString()
{
	std::string s("x: ");
	s.append(x);
	s.append(", y: ");
	s.append(y);
	s.append(", z: ");
	s.append(z);
}
*/
void V3f_class::packTo01()
{
	setV3f(getPackedTo01());
	updateArray();
}

V3f_class *V3f_class::getPackedTo01()
{
	V3f_class *temp = new V3f_class(this);
	temp->normalize();
	temp->mul(0.5f);
	temp->add(0.5f);

	return temp;
}

void V3f_class::unPackTo11()
{
	setV3f(getUnPackedTo11());
	updateArray();
}

V3f_class *V3f_class::getUnPackedTo11()
{
	V3f_class *temp = new V3f_class(this);
	temp->add(-0.5f);
	temp->mul(2.0f);

	return temp;
}
V3f_class *V3f_class::getRotatedX(double angle)
{
	if(angle==0.0)
		return this;

	float sinAngle=(float)sin(M_PI*angle/180.0);
	float cosAngle=(float)cos(M_PI*angle/180.0);

	return new V3f_class(	x,
			y*cosAngle - z*sinAngle,
			y*sinAngle + z*cosAngle);
}

void V3f_class::rotateX(double angle)
{
	setV3f(getRotatedX(angle));
	updateArray();
}

V3f_class *V3f_class::getRotatedY(double angle)
{
	if(angle==0.0)
		return this;

	float sinAngle=(float)sin(M_PI*angle/180.0);
	float cosAngle=(float)cos(M_PI*angle/180.0);

	return new V3f_class(	x*cosAngle + z*sinAngle,
			y,
			-x*sinAngle + z*cosAngle);
}

void V3f_class::rotateY(double angle)
{
	setV3f(getRotatedY(angle));
	updateArray();
}

V3f_class *V3f_class::getRotatedZ(double angle)
{
	if(angle==0.0)
		return this;

	float sinAngle=(float)sin(M_PI*angle/180.0);
	float cosAngle=(float)cos(M_PI*angle/180.0);

	return new V3f_class(x*cosAngle - y*sinAngle,
			x*sinAngle + y*cosAngle,
			z);
}

void V3f_class::rotateZ(double angle)
{
	setV3f(getRotatedZ(angle));
	updateArray();
}

V3f_class *V3f_class::getRotatedAxis(double angle, V3f_class *axis)
{
	if(angle==0.0)
		return this;

	V3f_class *u=axis->getNormalized();

	V3f_class *rotMatrixRow0 = new V3f_class();
	V3f_class *rotMatrixRow1 = new V3f_class();
	V3f_class *rotMatrixRow2 = new V3f_class();

	float sinAngle=(float)sin(M_PI*angle/180.0);
	float cosAngle=(float)cos(M_PI*angle/180.0);
	float oneMinusCosAngle=1.0f-cosAngle;

	rotMatrixRow0->x=(u->x)*(u->x) + cosAngle*(1.0f-(u->x)*(u->x));
	rotMatrixRow0->y=(u->x)*(u->y)*(oneMinusCosAngle) - sinAngle*u->z;
	rotMatrixRow0->z=(u->x)*(u->z)*(oneMinusCosAngle) + sinAngle*u->y;

	rotMatrixRow1->x=(u->x)*(u->y)*(oneMinusCosAngle) + sinAngle*u->z;
	rotMatrixRow1->y=(u->y)*(u->y) + cosAngle*(1.0f-(u->y)*(u->y));
	rotMatrixRow1->z=(u->y)*(u->z)*(oneMinusCosAngle) - sinAngle*u->x;

	rotMatrixRow2->x=(u->x)*(u->z)*(oneMinusCosAngle) - sinAngle*u->y;
	rotMatrixRow2->y=(u->y)*(u->z)*(oneMinusCosAngle) + sinAngle*u->x;
	rotMatrixRow2->z=(u->z)*(u->z) + cosAngle*(1.0f-(u->z)*(u->z));

	return new V3f_class(	V3f_class::dot(this, rotMatrixRow0),
			V3f_class::dot(this, rotMatrixRow1),
			V3f_class::dot(this, rotMatrixRow2));
}

void V3f_class::rotateAxis(double angle, V3f_class *axis)
{
	setV3f(getRotatedAxis(angle, axis));
	updateArray();
}

void V3f_class::operator+=(float scalar){
	add(scalar);
}

void V3f_class::operator+=(V3f_class * v){
	add(v);
}

void V3f_class::updateArray(){
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

} // namespace OOCTools_
