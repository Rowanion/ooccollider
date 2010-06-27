/**
 * @file	RsQuaternion.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	RsQuaternion class definition.
 */

#include "RsQuaternion.h"
#include <cmath>

#define TOLERANCE 0.0001
#define PIOVER180 0.0174532925199

RsQuaternion::RsQuaternion(){
	this->w = 1.0f;
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

RsQuaternion::RsQuaternion(float _w, float _x, float _y, float _z){
	this->w = _w;
        this->x = _x;
        this->y = _y;
        this->z = _z;
}

RsQuaternion::~RsQuaternion(){

}

RsQuaternion RsQuaternion::operator*(const RsQuaternion &_quat) const {
	return RsQuaternion(w * _quat.w - x * _quat.x - y * _quat.y - z * _quat.z,
			  w * _quat.x + x * _quat.w + y * _quat.z - z * _quat.y,
			  w * _quat.y + y * _quat.w + z * _quat.x - x * _quat.z,
			  w * _quat.z + z * _quat.w + x * _quat.y - y * _quat.x);
}

RsQuaternion RsQuaternion::operator+(const RsQuaternion &_quat) const {
	return RsQuaternion(w + _quat.w, x + _quat.x, x + _quat.y, z + _quat.z);
}

RsV3f RsQuaternion::operator*(const RsV3f &_vec) const{
	RsV3f aVec = _vec;
	float aScale = 1.0/sqrt(aVec.data.x*aVec.data.y + aVec.data.y*aVec.data.y + aVec.data.z*aVec.data.z);
        aVec.data.x *= aScale;
        aVec.data.y *= aScale;
        aVec.data.z *= aScale;

	RsQuaternion vecQuat, resQuat, conQuat;
	vecQuat.x = aVec.data.x;
	vecQuat.y = aVec.data.y;
	vecQuat.z = aVec.data.z;
	vecQuat.w = 0.0f;
	conQuat = this->getConjugate();
	resQuat = vecQuat * conQuat;
	resQuat = *this * resQuat;
	RsV3f aResVec = RsV3f(resQuat.x, resQuat.y, resQuat.z);

	return aResVec;
}

void RsQuaternion::conjugate(){
	x = -x;
	y = -y;
	z = -z;
}

RsQuaternion RsQuaternion::getConjugate() const {
	return RsQuaternion(w, -x, -y, -z);
}

void RsQuaternion::fromAxis(const RsV3f &_vec, float _angle){
	float aSinAngle;
	_angle *= 0.5f;
	RsV3f aVec = _vec;

	float aScale = 1.0/sqrt(aVec.data.x*aVec.data.x + aVec.data.y*aVec.data.y + aVec.data.z*aVec.data.z);
	aVec.data.x *= aScale;
	aVec.data.y *= aScale;
	aVec.data.z *= aScale;

	aSinAngle = sin(_angle);
	this->x = (aVec.data.x * aSinAngle);
	this->y = (aVec.data.y * aSinAngle);
	this->z = (aVec.data.z * aSinAngle);
	this->w = cos(_angle);
}

void RsQuaternion::fromEulerAngles(float _xrot, float _yrot, float _zrot){
	float aXR = _xrot * PIOVER180 / 2.0;
	float aYR = _yrot * PIOVER180 / 2.0;
	float aZR = _zrot * PIOVER180 / 2.0;

	float aSinp = sin(aXR);
	float aSiny = sin(aYR);
	float aSinr = sin(aZR);
	float aCosp = cos(aXR);
	float aCosy = cos(aYR);
	float aCosr = cos(aZR);

	this->x = aSinr * aCosp * aCosy - aCosr * aSinp * aSiny;
	this->y = aCosr * aSinp * aCosy + aSinr * aCosp * aSiny;
	this->z = aCosr * aCosp * aSiny - aSinr * aSinp * aCosy;
	this->w = aCosr * aCosp * aCosy + aSinr * aSinp * aSiny;

	normalize();
}

void RsQuaternion::getRotationMatrix(float* _dest){
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	_dest[0] = 1.0f - 2.0f * (y2 + z2);
	_dest[1] = 2.0f * (xy - wz);
	_dest[2] = 2.0f * (xz + wy);
	_dest[3] = 0.0f;

	_dest[4] = 2.0f * (xy + wz);
	_dest[5] = 1.0f - 2.0f * (x2 + z2);
	_dest[6] = 2.0f * (yz - wx);
	_dest[7] = 0.0f;

	_dest[8] = 2.0f * (xz - wy);
	_dest[9] = 2.0f * (yz + wx);
	_dest[10] = 1.0f - 2.0f * (x2 + y2);
	_dest[11] = 0.0f;

	_dest[12] = 0.0f;
	_dest[13] = 0.0f;
	_dest[14] = 0.0f;
	_dest[15] = 1.0f;
}

void RsQuaternion::fromRotationMatrix(float *_rotMat){
	float a[4][4];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			a[i][j] = _rotMat[i*4+j];
		}
	}
	float trace = a[0][0] + a[1][1] + a[2][2] + 1.0f;
  if( trace > 0.0001 ) {
    float s = 0.5f / sqrtf(trace);
    w = 0.25f / s;
    x = ( a[2][1] - a[1][2] ) * s;
    y = ( a[0][2] - a[2][0] ) * s;
    z = ( a[1][0] - a[0][1] ) * s;
  } else {
    if ( a[0][0] > a[1][1] && a[0][0] > a[2][2] ) {
      float s = 2.0f * sqrtf( 1.0f + a[0][0] - a[1][1] - a[2][2]);
      w = (a[2][1] - a[1][2] ) / s;
      x = 0.25f * s;
      y = (a[0][1] + a[1][0] ) / s;
      z = (a[0][2] + a[2][0] ) / s;
    } else if (a[1][1] > a[2][2]) {
      float s = 2.0f * sqrtf( 1.0f + a[1][1] - a[0][0] - a[2][2]);
      w = (a[0][2] - a[2][0] ) / s;
      x = (a[0][1] + a[1][0] ) / s;
      y = 0.25f * s;
      z = (a[1][2] + a[2][1] ) / s;
    } else {
      float s = 2.0f * sqrtf( 1.0f + a[2][2] - a[0][0] - a[1][1] );
      w = (a[1][0] - a[0][1] ) / s;
      x = (a[0][2] + a[2][0] ) / s;
      y = (a[1][2] + a[2][1] ) / s;
      z = 0.25f * s;
    }
  }

}

void RsQuaternion::getAxisAngles(RsV3f *_vec, float *_angle){
	float aScale = 1.0/sqrt(x*x + y*y + z*z);

	_vec->data.x = x*aScale;
	_vec->data.y = y*aScale;
	_vec->data.z = z*aScale;

	*_angle = cos(w) * 2.0f;
}

void RsQuaternion::normalize(){
	float mag2 = w * w + x * x + y * y + z * z;
	if (fabs(mag2 - 1.0f) > TOLERANCE) {
		float mag = sqrt(mag2);
		w /= mag;
		x /= mag;
		y /= mag;
		z /= mag;
	}
}

void RsQuaternion::reset() {
	w = 1;
	x = y = z = 0;
}
