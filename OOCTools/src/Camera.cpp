/**
 * @file	Camera.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 07.04.2009
 *
 */

#include "Camera.h"

#include <cmath>

#include "boost/math/quaternion.hpp"
#include "GlToolkit.h"
#include "V4f.h"

namespace bm = boost::math;

namespace ooctools {

Camera::Camera() : mEye(0.0f,0.0f,0.0f), mView(0.0f,0.0f,0.0f), mUp(0.0f,0.0f,0.0f), mRollDegrees(180.0f)
{

}

Camera::~Camera()
{
	// TODO Auto-generated destructor stub
}

void
Camera::positionCamera(float eX, float eY, float eZ,
		float vX, float vY, float vZ,
		float uX, float uY, float uZ)
{
	mEye.setX(eX);
	mEye.setY(eY);
	mEye.setZ(eZ);
	mView.setX(vX);
	mView.setY(vY);
	mView.setZ(vZ);
	mUp.setX(uX);
	mUp.setY(uY);
	mUp.setZ(uZ);
//	applyToGL();
}

void
Camera::positionCamera(const V3f& _eye, const V3f& _view, const V3f& _up)
{
	mEye = _eye;
	mView = _view;
	mUp = _up;
//	applyToGL();
}

void
Camera::moveCamera(float speed)
{
	V3f direction(mView - mEye);

	direction.normalize();

	mEye += direction * speed;
	mView += direction * speed;
//	applyToGL();
}

// TODO still broken - needs fixing!
void
Camera::rollRotation(float angleX)
{
	// Get our view vector (The direction we are facing)
	V3f vView(mView - mEye);
	float x = vView.getX();
	float y = vView.getY();
	float z = vView.getZ();

	V3f vNewUp;

	// Calculate the sine and cosine of the angle once
	float cosTheta = cos(angleX);
	float sinTheta = sin(angleX);

	// Find the new x position for the new rotated point
	*vNewUp.x = ((cosTheta + (1.0f - cosTheta) * x * x) * vView.getX());
	*vNewUp.x += ((1.0f - cosTheta) * x * y - z * sinTheta)	* mUp.getY();
	*vNewUp.x += ((1.0f - cosTheta) * x * z + y * sinTheta)	* mUp.getZ();

	// Find the new y position for the new rotated point
	*vNewUp.y  = ((1.0f - cosTheta) * x * y + z * sinTheta)	* mUp.getX();
	*vNewUp.y += (cosTheta + (1.0f - cosTheta) * y * y)		* mUp.getY();
	*vNewUp.y += ((1.0f - cosTheta) * y * z - x * sinTheta)	* mUp.getZ();

	// Find the new z position for the new rotated point
	*vNewUp.z  = ((1.0f - cosTheta) * x * z - y * sinTheta)	* mUp.getX();
	*vNewUp.z += ((1.0f - cosTheta) * y * z + x * sinTheta)	* mUp.getY();
	*vNewUp.z += (cosTheta + (1.0f - cosTheta) * z * z)		* mUp.getZ();

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	mUp = vNewUp;
//	mView = mEye + vNewView;
}

void
Camera::mouseRotate(float angleY, float angleZ)
{
	V3f vAxis = V3f::cross(mView - mEye, mUp);
	vAxis.normalize();

	// Rotate around our perpendicular axis and along the y-axis
//	rotateView(angleZ, vAxis);
//	rotateView(angleY, 0.0f, 1.0f, 0.0f);
	rotateView(angleZ, vAxis);
	rotateView(angleY, 0.0f, 1.0f,0.0f);
//	applyToGL();
}


void
Camera::rotateView(float angle, const V3f& vec)
{
	rotateView(angle, vec.getX(), vec.getY(), vec.getZ());
}

void
Camera::rotateView(float angle, float x, float y, float z)
{

	V3f vNewView;

	// Get our view vector (The direction we are facing)
	V3f vView(mView - mEye);

	// Calculate the sine and cosine of the angle once
	float cosTheta = cos(angle);
	float sinTheta = sin(angle);

	// Find the new x position for the new rotated point
	*vNewView.x = ((cosTheta + (1.0f - cosTheta) * x * x) * vView.getX());
	*vNewView.x += ((1.0f - cosTheta) * x * y - z * sinTheta)	* vView.getY();
	*vNewView.x += ((1.0f - cosTheta) * x * z + y * sinTheta)	* vView.getZ();

	// Find the new y position for the new rotated point
	*vNewView.y  = ((1.0f - cosTheta) * x * y + z * sinTheta)	* vView.getX();
	*vNewView.y += (cosTheta + (1.0f - cosTheta) * y * y)		* vView.getY();
	*vNewView.y += ((1.0f - cosTheta) * y * z - x * sinTheta)	* vView.getZ();

	// Find the new z position for the new rotated point
	*vNewView.z  = ((1.0f - cosTheta) * x * z - y * sinTheta)	* vView.getX();
	*vNewView.z += ((1.0f - cosTheta) * y * z + x * sinTheta)	* vView.getY();
	*vNewView.z += (cosTheta + (1.0f - cosTheta) * z * z)		* vView.getZ();

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	mView = mEye + vNewView;
}

void
Camera::strafeCamera(float speed)
{
	// Strafing is quite simple if you understand what the cross product is.
	// If you have 2 vectors (say the up vVector and the view vVector) you can
	// use the cross product formula to get a vVector that is 90 degrees from the 2 vectors.
	// For a better explanation on how this works, check out the OpenGL "Normals" tutorial at our site.
	// In our new Update() function, we set the strafing vector (m_vStrafe).  Due
	// to the fact that we need this vector for many things including the strafing
	// movement and camera rotation (up and down), we just calculate it once.
	//
	// Like our MoveCamera() function, we add the strafing vector to our current position
	// and view.  It's as simple as that.  It has already been calculated in Update().

	V3f vStrafe = V3f::cross(mView - mEye, mUp);
	vStrafe.normalize();

	// Add the strafe vector to our position
	*mEye.x += vStrafe.getX() * speed;
	*mEye.z += vStrafe.getZ() * speed;

	// Add the strafe vector to our view
	*mView.x += vStrafe.getX() * speed;
	*mView.z += vStrafe.getZ() * speed;
//	applyToGL();
}

void
Camera::applyToGL() const
{
	gluLookAt(*mEye.x, *mEye.y, *mEye.z,
						*mView.x, *mView.y, *mView.z,
						*mUp.x, *mUp.y, *mUp.z);
//	gluLookAt(mEye.getX(), mEye.getY(), mEye.getZ(), mView.getX(),
//			mView.getY(), mView.getZ(), mUp.getX(), mUp.getZ(), mUp.getZ());
}

void
Camera::rotateVectorQuat(float angle, float x, float y, float z, V3f& vector) const
{
	bm::quaternion<float> temp(x * sin(angle / 2.0f), y * sin(angle / 2.0f), z
			* sin(angle / 2.0f), cos(angle / 2.0f));
	bm::quaternion<float> quat_vec(*vector.x, *vector.y, *vector.z, 0.0);
	bm::quaternion<float> result = (temp * quat_vec) * bm::conj(temp);

	result.real();
	vector.setX(result.R_component_1());
	vector.setY(result.R_component_2());
	vector.setZ(result.R_component_3());
}

void
Camera::rotateVectorQuat(float angle, const V3f& axis, V3f& vector) const
{
	rotateVectorQuat(angle, *axis.x, *axis.y, *axis.z, vector);
}

void Camera::setRoll(float _roll)
{
	mRollDegrees = _roll;
	if (mRollDegrees >= 360.0 || mRollDegrees <= -360.0f)
		mRollDegrees = 0.0f;
}

void Camera::SetPerspective()
{
	GLfloat Matrix[16];
	glQuaternion q;

	// Make the Quaternions that will represent our rotations
	m_qPitch.CreateFromAxisAngle(1.0f, 0.0f, 0.0f, m_PitchDegrees);
	m_qHeading.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, m_HeadingDegrees);
	m_qRoll.CreateFromAxisAngle(0.0f, 0.0f, 1.0f, m_RollDegrees);

	// Combine the pitch and heading rotations and store the results in q
//	q = m_qPitch * m_qHeading;
	q = m_qPitch * m_qHeading * m_qRoll;
	q.CreateMatrix(Matrix);

	// Let OpenGL set our new prespective on the world!
	glMultMatrixf(Matrix);

	// Create a matrix from the pitch Quaternion and get the j vector
	// for our direction.
	m_qPitch.CreateMatrix(Matrix);
	*m_DirectionVector.y = Matrix[9];

	// Combine the heading and pitch rotations and make a matrix to get
	// the i and j vectors for our direction.
	q = m_qHeading * m_qPitch;
	q.CreateMatrix(Matrix);
	*m_DirectionVector.x = Matrix[8];
	*m_DirectionVector.z = Matrix[10];

	// Scale the direction by our speed.
	m_DirectionVector *= m_ForwardVelocity;

	// Increment our position by the vector
	*m_Position.x += *m_DirectionVector.x;
	*m_Position.y += *m_DirectionVector.y;
	*m_Position.z += *m_DirectionVector.z;

	// Translate to our new position.
	glTranslatef(-*m_Position.x, -*m_Position.y, *m_Position.z);

}


} // ooctools
