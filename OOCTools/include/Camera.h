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
};

} // ooctools

#endif /* CAMERA_H_ */
