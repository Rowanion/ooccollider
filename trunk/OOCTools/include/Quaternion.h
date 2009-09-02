/**
 * @file	Quaternion.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	Quaternion class declaration.
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

struct Vector3f{
	float x;
	float y;
	float z;
};

namespace ooctools{
class Quaternion;

/**
 * @class Quaternion
 * @brief This class deals with camera-rotations and transformations.
 */
class Quaternion {
public:
	Quaternion();
	Quaternion(float _w, float _x, float _y, float _z);

	virtual ~Quaternion();

	Quaternion operator*(const Quaternion &_quat) const;
	Quaternion operator+(const Quaternion &_quat) const;
	Vector3f operator*(const Vector3f &_vec) const;

	void conjugate();
	Quaternion getConjugate() const;
	void fromAxis(const Vector3f &_vec, float _angle);
	void fromEulerAngles(float _xrot, float _yrot, float _zrot);
	void getRotationMatrix(float* _dest);
	void fromRotationMatrix(float *_rotMat);
	void getAxisAngles(Vector3f *_vec, float *_angle);
	void normalize();
	void reset();
	float w, x, y, z, i, j, k;

private:

};

} // ooctools
#endif

