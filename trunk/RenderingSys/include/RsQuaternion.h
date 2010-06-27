/**
 * @file	RsQuaternion.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	RsQuaternion class declaration.
 */

#ifndef RSQUATERNION_H_
#define RSQUATERNION_H_

#include "RsVectorMath.h"

/**
 * @class RsQuaternion
 * @brief This class deals with camera-rotations and transformations.
 */
class RsQuaternion {
public:
	RsQuaternion();
	RsQuaternion(float _w, float _x, float _y, float _z);

	virtual ~RsQuaternion();

	RsQuaternion operator*(const RsQuaternion &_quat) const;
	RsQuaternion operator+(const RsQuaternion &_quat) const;
	RsV3f operator*(const RsV3f &_vec) const;

	void conjugate();
	RsQuaternion getConjugate() const;
	void fromAxis(const RsV3f &_vec, float _angle);
	void fromEulerAngles(float _xrot, float _yrot, float _zrot);
	void getRotationMatrix(float* _dest);
	void fromRotationMatrix(float *_rotMat);
	void getAxisAngles(RsV3f *_vec, float *_angle);
	void normalize();
	void reset();
private:
	float w, x, y, z, i, j, k;

};

#endif // RSQUATERNION_H_

