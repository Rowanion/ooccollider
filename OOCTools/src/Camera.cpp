/**
 * @file	Camera.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 07.04.2009
 *
 */

#include "Camera.h"

namespace ooctools {

Camera::Camera() : mEye(0.0f,0.0f,0.0f), mView(0.0f,0.0f,0.0f), mUp(0.0f,0.0f,0.0f)
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
}

void
Camera::positionCamera(const V3f& _eye, const V3f& _view, const V3f& _up)
{
	mEye = _eye;
	mView = _view;
	mUp = _up;
}

void
Camera::moveCamera(float speed)
{
	V3f direction(mView - mEye);

	direction.normalize();

	mEye += direction * speed;
	mView += direction * speed;
}

// TODO still broken - needs fixing!
void
Camera::rollRotation(float angleX)
{

	float x = 1.0f;
	float y = 0.0f;
	float z = 0.0f;
	V3f vNewView;

	// Get our view vector (The direction we are facing)
	V3f vView(mUp);

	// Calculate the sine and cosine of the angle once
	float cosTheta = cos(angleX);
	float sinTheta = sin(angleX);

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
	mUp = vNewView;
}

void
Camera::mouseRotate(float angleY, float angleZ)
{
	V3f vAxis = V3f::cross(mView - mEye, mUp);
	vAxis.normalize();

	// Rotate around our perpendicular axis and along the y-axis
	rotateView(angleZ, vAxis);
	rotateView(angleY, 0.0f, 1.0f, 0.0f);
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
}

} // ooctools
