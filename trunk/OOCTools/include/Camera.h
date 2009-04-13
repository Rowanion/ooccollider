/**
 * @file	Camera.h
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 07.04.2009
 *
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <cmath>

#include "declarations.h"
#include "V3f.h"
#include "glQuaternion.h"

namespace ooctools {

/**
 * @class Camera
 * @brief a representation of a camera to navigate realistic through space.
 *
 */
class Camera
{
	public:
		Camera();

		virtual ~Camera();

		/**
		 * Sets the camera's eye, lookAt and up-vectors.
		 * @param eX x-value of eye[]
		 * @param eY y-value of eye[]
		 * @param eZ z-value of eye[]
		 * @param vX x-value of view[]
		 * @param vY y-value of view[]
		 * @param vZ z-value of view[]
		 * @param uX x-value of up[]
		 * @param uY y-value of up[]
		 * @param uZ z-value of up[]
		 */
		void positionCamera(float eX, float eY, float eZ,
				float vX, float vY, float vZ,
				float uX, float uY, float uZ);

		/**
		 * Sets the camera's eye, lookAt and up-vectors.
		 * @param _eye the eye-vector
		 * @param _view the view-vector
		 * @param _up the up-vector
		 */
		void positionCamera(const V3f& _eye, const V3f& _view, const V3f& _up);

		/**
		 * Adds a front- or back-facing vector to our current eye- and lookAt-vector.
		 * @param speed
		 */
		void moveCamera(float speed);

		void
		rollRotation(float angleX);

		/**
		 * Initiates rotation of view around our two mouse-axes.
		 * @param angleY
		 * @param angleZ
		 */
		void mouseRotate(float angleY, float angleZ);

		/**
		 * Rotates our current view by angle around any axis. In our case either
		 * around the axis normal to our lookAt-vector (left/right) or a vector
		 * perpendicular to either of the sides of our current lookAt-vector
		 * (up/down).
		 * @param angle
		 * @param vec the vector
		 */
		void rotateView(float angle, const V3f& vec);

		/**
		 * Rotates our current view by angle around any axis. In our case either
		 * around the axis normal to our lookAt-vector (left/right) or a vector
		 * perpendicular to either of the sides of our current lookAt-vector
		 * (up/down).
		 * @param angle
		 * @param x
		 * @param y
		 * @param z
		 */
		void rotateView(float angle, float x, float y, float z);

		/**
		 * Strafes the camera (sidestep) by adding a vector to our eye-position and
		 * the lookAt-Vector.
		 * @param speed
		 */
		void strafeCamera(float speed);

		/**
		 * Returns the our eye-position.
		 * @return float[]
		 */
		const V3f& getEye() const {return mEye;};

		/**
		 * Returns the up-vector.
		 * @return float[]
		 */
		const V3f& getUp() const {return mUp;};

		/**
		 * Returns the view, the point we're looking at.
		 * @return float[]
		 */
		const V3f& getView() const {return mView;};

		/**
		 * Shortcut for gluLookAt() with our camera-object. This is called each time the camera is
		 * changed.
		 */
		void applyToGL() const;

		/**
		 * Rotation using boost::math::quaterions - <b>Highly experimental</b>
		 * @param Angle the angle to rotate by
		 * @param x x-part of the axis to rotate around
		 * @param y y-part of the axis to rotate around
		 * @param z z-part of the axis to rotate around
		 */
		void rotateVectorQuat(float Angle, float x, float y, float z, V3f& vector) const;

		void rotateVectorQuat(float Angle, const V3f& axis, V3f& vector) const;

		float getRoll() const {return mRollDegrees;};
		void setRoll(float _roll);

		// ----------------------------------------
		void SetPerspective();
		V3f m_DirectionVector;
		float m_ForwardVelocity;
		V3f m_Position;
		float m_MaxForwardVelocity;
		float m_MaxPitchRate;
		float m_MaxHeadingRate;
		float m_PitchDegrees;
		float m_HeadingDegrees;
		float m_RollDegrees;
		glQuaternion m_qPitch;
		glQuaternion m_qHeading;
		glQuaternion m_qRoll;

		// ---------------------------------------
	private:
		/**
		* Our current eye-position.
		*/
		V3f mEye; //= new float [] {0.0f,0.0f,0.0f};

		/**
		* The point we're looking at.
		*/
		V3f mView; //  = new float[] {0.0f,0.0f,0.0f};

		/**
		* Our UP-Vector for gluLookAt().
		*/
		V3f mUp; // = new float [] {0.0f,0.0f,0.0f};

		float mRollDegrees;
};

} // ooctools

#endif /* CAMERA_H_ */
