/**
 * @file	GeometricOps.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 10.05.2009
 * @brief 	GeometricOps class declaration.
 */

#include "GeometricOps.h"

#include <cmath>
#include <cstring>

#include "V3f.h"
#include "V4f.h"
namespace ooctools{

GeometricOps::GeometricOps() {
	// TODO Auto-generated constructor stub

}

GeometricOps::~GeometricOps() {
	// TODO Auto-generated destructor stub
}

void
GeometricOps::calcTriangleCenter(const V3f& a, const V3f& b, const V3f& c, V3f& center)
{
	center =  ((a+b+c)*(1.0f/3.0f));
}

void
GeometricOps::calcTriangleCenter(const V4f& a, const V4f& b, const V4f& c, V3f& center)
{
	V3f _a = a;
	V3f _b = b;
	V3f _c = c;
	center =  ((_a+_b+_c)*(1.0f/3.0f));
}

void
GeometricOps::calcTriangleCenter3(const float* triangle, V3f& center)
{
	V3f _a(triangle);
	V3f _b(triangle+3);
	V3f _c(triangle+6);
	center = ((_a + _b + _c) * (1.0f / 3.0f));
}

void
GeometricOps::calcTriangleCenter4(const float* triangle, V3f& center)
{
	V3f _a(triangle);
	V3f _b(triangle + 4);
	V3f _c(triangle + 8);
	center = ((_a + _b + _c) * (1.0f / 3.0f));
}

void GeometricOps::calcTriangleEdges4(const float* triangle, V3f& edges)
{
	edges.setX(calcLineLength3(triangle,triangle+4));
	edges.setY(calcLineLength3(triangle+4,triangle+8));
	edges.setZ(calcLineLength3(triangle+8,triangle));
}

float GeometricOps::calcMaxTriangleEdge(const V3f& a, const V3f& b, const V3f& c)
{
	float max = calcLineLength(a,b);
	float temp = calcLineLength(b,c);
	(temp > max) ? max = temp : max;
	temp = calcLineLength(c,a);
	(temp > max) ? max = temp : max;
	return max;
}

float GeometricOps::calcMaxTriangleEdge(const V4f& a, const V4f& b, const V4f& c)
{
	float max = calcLineLength(a,b);
	float temp = calcLineLength(b,c);
	(temp > max) ? max = temp : max;
	temp = calcLineLength(c,a);
	(temp > max) ? max = temp : max;
	return max;
}

double GeometricOps::calcMaxTriangleEdge3(const float* triangle)
{
	double max = calcLineLength3(triangle,triangle+3);
	double temp = calcLineLength3(triangle+3,triangle+6);
	(temp > max) ? max = temp : max;
	temp = calcLineLength3(triangle+6,triangle);
	(temp > max) ? max = temp : max;
	return max;
}

double GeometricOps::calcMaxTriangleEdge4(const float* triangle)
{
	double max = calcLineLength3(triangle,triangle+4);
	double temp = calcLineLength3(triangle+4,triangle+8);
	(temp > max) ? max = temp : max;
	temp = calcLineLength3(triangle+8,triangle);
	(temp > max) ? max = temp : max;
	return max;
}

float GeometricOps::calcMinTriangleEdge(const V3f& a, const V3f& b, const V3f& c)
{
	float min = calcLineLength(a,b);
	float temp = calcLineLength(b,c);
	(temp < min) ? min = temp : min;
	temp = calcLineLength(c,a);
	(temp < min) ? min = temp : min;
	return min;
}

float GeometricOps::calcMinTriangleEdge(const V4f& a, const V4f& b, const V4f& c)
{
	float min = calcLineLength(a,b);
	float temp = calcLineLength(b,c);
	(temp < min) ? min = temp : min;
	temp = calcLineLength(c,a);
	(temp < min) ? min = temp : min;
	return min;
}

long double GeometricOps::calcMinTriangleEdge3(const float* triangle)
{
	long double min = calcLineLength3(triangle,triangle+3);
	long double temp = calcLineLength3(triangle+3,triangle+6);
	(temp < min) ? min = temp : min = min;
	temp = calcLineLength3(triangle+6,triangle);
	(temp < min) ? min = temp : min =min;
	return min;
}

long double GeometricOps::calcMinTriangleEdge4(const float* triangle)
{
	long double min = calcLineLength3(triangle,triangle+4);
	long double temp = calcLineLength3(triangle+4,triangle+8);
	(temp < min) ? min = temp : min = min;
	temp = calcLineLength3(triangle+8,triangle);
	(temp < min) ? min = temp : min =min;
	return min;
}

float GeometricOps::calcLineLength(const V3f& a, const V3f& b)
{
	return sqrt(pow(b.getX()-a.getX(),2.0)+pow(b.getY()-a.getY(),2.0) + pow(b.getZ()-a.getZ(),2.0));
}

float GeometricOps::calcLineLength(const V4f& a, const V4f& b)
{
	return sqrt(pow(b.getX()-a.getX(),2.0)+pow(b.getY()-a.getY(),2.0) + pow(b.getZ()-a.getZ(),2.0));
}

long double GeometricOps::calcLineLength3(const float* a, const float* b)
{
	long double bax = b[0]-a[0];
	long double bay = b[1]-a[1];
	long double baz = b[2]-a[2];
	return sqrt((bax*bax) + (bay*bay) + (baz*baz));
}

long double GeometricOps::calcTriangleArea4(const float* triangle)
{
	// Area: 1/2 * b * c * sin(A) =
	// 1/4 * sqrt((a+b-c) (a-b+c) (-a+b+c) (a+b+c)) | where a,b,c edgelengths

	//16*A^2 = (a^2+b^2+c^2)^2    <- ??
	// where a,b,c are the edge magnitudes

	long double a = calcLineLength3(triangle, triangle+4);
	long double b = calcLineLength3(triangle+4, triangle+8);
	long double c = calcLineLength3(triangle+8, triangle);

	long double area = sqrt((a+b-c)*(a-b+c)*(-a+b+c)*(a+b+c))/4.0;



	return area;
}

void GeometricOps::transposeMat4( float* _mat ) {
	float	temp;
	int		i, j;

	for( i = 0; i < 4; i++ ) {
		for( j = i + 1; j < 4; j++ ) {
			temp = _mat[ i + (4 * j) ];
			_mat[ i + (4 * j)] = _mat[ j + (4 * i) ];
			_mat[ j + (4 * i) ] = temp;
        }
	}
}

bool GeometricOps::calcEyePosition( float* _mat, V3f& eye )
{
	float mat[16];
	memcpy(mat, _mat, 16*sizeof(float));
	transposeMat4(mat);
	// 84+4+16 = 104 multiplications
	//			   1 division
	double det, invDet;

//	xy
//	x+(y*4)

	// 2x2 sub-determinants required to calculate 4x4 determinant
	float det2_01_01 = mat[0 + (0 * 4)] * mat[1 + (1 * 4)] - mat[0 + (1 * 4)] * mat[1 + (0 * 4)];
	float det2_01_02 = mat[0 + (0 * 4)] * mat[1 + (2 * 4)] - mat[0 + (2 * 4)] * mat[1 + (0 * 4)];
	float det2_01_03 = mat[0 + (0 * 4)] * mat[1 + (3 * 4)] - mat[0 + (3 * 4)] * mat[1 + (0 * 4)];
	float det2_01_12 = mat[0 + (1 * 4)] * mat[1 + (2 * 4)] - mat[0 + (2 * 4)] * mat[1 + (1 * 4)];
	float det2_01_13 = mat[0 + (1 * 4)] * mat[1 + (3 * 4)] - mat[0 + (3 * 4)] * mat[1 + (1 * 4)];
	float det2_01_23 = mat[0 + (2 * 4)] * mat[1 + (3 * 4)] - mat[0 + (3 * 4)] * mat[1 + (2 * 4)];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	float det3_201_012 = mat[2 + (0 * 4)] * det2_01_12 - mat[2 + (1 * 4)] * det2_01_02 + mat[2 + (2 * 4)] * det2_01_01;
	float det3_201_013 = mat[2 + (0 * 4)] * det2_01_13 - mat[2 + (1 * 4)] * det2_01_03 + mat[2 + (3 * 4)] * det2_01_01;
	float det3_201_023 = mat[2 + (0 * 4)] * det2_01_23 - mat[2 + (2 * 4)] * det2_01_03 + mat[2 + (3 * 4)] * det2_01_02;
	float det3_201_123 = mat[2 + (1 * 4)] * det2_01_23 - mat[2 + (2 * 4)] * det2_01_13 + mat[2 + (3 * 4)] * det2_01_12;

	det = ( - det3_201_123 * mat[3 + (0 * 4)] + det3_201_023 * mat[3 + (1 * 4)] - det3_201_013 * mat[3 + (2 * 4)] + det3_201_012 * mat[3 + (3 * 4)] );

	if ( fabs(det) < 0.001 ) {
		return false;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	float det2_03_01 = mat[0 + (0 * 4)] * mat[3 + (1 * 4)] - mat[0 + (1 * 4)] * mat[3 + (0 * 4)];
	float det2_03_02 = mat[0 + (0 * 4)] * mat[3 + (2 * 4)] - mat[0 + (2 * 4)] * mat[3 + (0 * 4)];
	float det2_03_03 = mat[0 + (0 * 4)] * mat[3 + (3 * 4)] - mat[0 + (3 * 4)] * mat[3 + (0 * 4)];
	float det2_03_12 = mat[0 + (1 * 4)] * mat[3 + (2 * 4)] - mat[0 + (2 * 4)] * mat[3 + (1 * 4)];
	float det2_03_13 = mat[0 + (1 * 4)] * mat[3 + (3 * 4)] - mat[0 + (3 * 4)] * mat[3 + (1 * 4)];
	float det2_03_23 = mat[0 + (2 * 4)] * mat[3 + (3 * 4)] - mat[0 + (3 * 4)] * mat[3 + (2 * 4)];

	float det2_13_01 = mat[1 + (0 * 4)] * mat[3 + (1 * 4)] - mat[1 + (1 * 4)] * mat[3 + (0 * 4)];
	float det2_13_02 = mat[1 + (0 * 4)] * mat[3 + (2 * 4)] - mat[1 + (2 * 4)] * mat[3 + (0 * 4)];
	float det2_13_03 = mat[1 + (0 * 4)] * mat[3 + (3 * 4)] - mat[1 + (3 * 4)] * mat[3 + (0 * 4)];
	float det2_13_12 = mat[1 + (1 * 4)] * mat[3 + (2 * 4)] - mat[1 + (2 * 4)] * mat[3 + (1 * 4)];
	float det2_13_13 = mat[1 + (1 * 4)] * mat[3 + (3 * 4)] - mat[1 + (3 * 4)] * mat[3 + (1 * 4)];
	float det2_13_23 = mat[1 + (2 * 4)] * mat[3 + (3 * 4)] - mat[1 + (3 * 4)] * mat[3 + (2 * 4)];

	// remaining 3x3 sub-determinants
	float det3_203_012 = mat[2 + (0 * 4)] * det2_03_12 - mat[2 + (1 * 4)] * det2_03_02 + mat[2 + (2 * 4)] * det2_03_01;
	float det3_203_013 = mat[2 + (0 * 4)] * det2_03_13 - mat[2 + (1 * 4)] * det2_03_03 + mat[2 + (3 * 4)] * det2_03_01;
	float det3_203_023 = mat[2 + (0 * 4)] * det2_03_23 - mat[2 + (2 * 4)] * det2_03_03 + mat[2 + (3 * 4)] * det2_03_02;
	float det3_203_123 = mat[2 + (1 * 4)] * det2_03_23 - mat[2 + (2 * 4)] * det2_03_13 + mat[2 + (3 * 4)] * det2_03_12;

	float det3_213_012 = mat[2 + (0 * 4)] * det2_13_12 - mat[2 + (1 * 4)] * det2_13_02 + mat[2 + (2 * 4)] * det2_13_01;
	float det3_213_013 = mat[2 + (0 * 4)] * det2_13_13 - mat[2 + (1 * 4)] * det2_13_03 + mat[2 + (3 * 4)] * det2_13_01;
	float det3_213_023 = mat[2 + (0 * 4)] * det2_13_23 - mat[2 + (2 * 4)] * det2_13_03 + mat[2 + (3 * 4)] * det2_13_02;
	float det3_213_123 = mat[2 + (1 * 4)] * det2_13_23 - mat[2 + (2 * 4)] * det2_13_13 + mat[2 + (3 * 4)] * det2_13_12;

	float det3_301_012 = mat[3 + (0 * 4)] * det2_01_12 - mat[3 + (1 * 4)] * det2_01_02 + mat[3 + (2 * 4)] * det2_01_01;
	float det3_301_013 = mat[3 + (0 * 4)] * det2_01_13 - mat[3 + (1 * 4)] * det2_01_03 + mat[3 + (3 * 4)] * det2_01_01;
	float det3_301_023 = mat[3 + (0 * 4)] * det2_01_23 - mat[3 + (2 * 4)] * det2_01_03 + mat[3 + (3 * 4)] * det2_01_02;
	float det3_301_123 = mat[3 + (1 * 4)] * det2_01_23 - mat[3 + (2 * 4)] * det2_01_13 + mat[3 + (3 * 4)] * det2_01_12;

	mat[0 + (0 * 4)] = - det3_213_123 * invDet;
	mat[1 + (0 * 4)] = + det3_213_023 * invDet;
	mat[2 + (0 * 4)] = - det3_213_013 * invDet;
	mat[3 + (0 * 4)] = + det3_213_012 * invDet;

	mat[0 + (1 * 4)] = + det3_203_123 * invDet;
	mat[1 + (1 * 4)] = - det3_203_023 * invDet;
	mat[2 + (1 * 4)] = + det3_203_013 * invDet;
	mat[3 + (1 * 4)] = - det3_203_012 * invDet;

	mat[0 + (2 * 4)] = + det3_301_123 * invDet;
	mat[1 + (2 * 4)] = - det3_301_023 * invDet;
	mat[2 + (2 * 4)] = + det3_301_013 * invDet;
	mat[3 + (2 * 4)] = - det3_301_012 * invDet;

	mat[0 + (3 * 4)] = - det3_201_123 * invDet;
	mat[1 + (3 * 4)] = + det3_201_023 * invDet;
	mat[2 + (3 * 4)] = - det3_201_013 * invDet;
	mat[3 + (3 * 4)] = + det3_201_012 * invDet;

//	eye.setX(mat[3]);
//	eye.setX(mat[7]);
//	eye.setX(mat[11]);
	eye.setX(mat[12]);
	eye.setX(mat[13]);
	eye.setX(mat[14]);
	return true;
}

} // ooctools
