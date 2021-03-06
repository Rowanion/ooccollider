/**
 * @file	OOCCamera.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	OOCCamera class declaration.
 */

#ifndef OOCCAMERA_H_
#define OOCCAMERA_H_

#include "declarations.h"
#include "Quaternion.h"

/**
 * @class OOCCamera
 * @brief Simple camera-class using quaternions.
 */
class OOCCamera {
public:
	OOCCamera();
	virtual ~OOCCamera();
	void initMatrices();
	void calcMatrix();

	inline void setXRot(float rot) {myXRot = rot;};
	inline void setYRot(float rot) {myYRot = rot;};
	inline void setZRot(float rot) {myZRot = rot;};

	inline void incXRot(float rot) {myXRot += rot;};
	inline void incYRot(float rot) {myYRot += rot;};
	inline void incZRot(float rot) {myZRot += rot;};

	inline void decXRot(float rot) {myXRot -= rot;};
	inline void decYRot(float rot) {myYRot -= rot;};
	inline void decZRot(float rot) {myZRot -= rot;};

	inline void setXMove(float move) {xmove = move;};
	inline void setYMove(float move) {ymove = move;};
	inline void setZMove(float move) {zmove = move;};

	inline void incXMove(float move) {xmove += move;};
	inline void incYMove(float move) {ymove += move;};
	inline void incZMove(float move) {zmove += move;};

	inline void decXMove(float move) {xmove -= move;};
	inline void decYMove(float move) {ymove -= move;};
	inline void decZMove(float move) {zmove -= move;};

	void setRotationMatrix(const float* mat);
private:
	float xmove, ymove, zmove;
	float myXRot;
	float myYRot;
	float myZRot;
	bool first;
	float myTranslateMatrix[16];
	float myGLRotMatrix[16];
	float myQuatRotMat[16];
	float myTempMat[16];
	ooctools::Quaternion localQuat;
	ooctools::Quaternion totalQuat;

	void multMatrix(float *m1, float *m2, float *res);
};

#endif /* OOCCAMERA_H_ */
