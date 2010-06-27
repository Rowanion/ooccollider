/**
 * @file	RsCamera.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	RsCamera class definition.
 */

#include "RsCamera.h"

#include <cstring>

#include <GL/glew.h>

RsCamera::RsCamera() : xmove(0.0), ymove(0.0), zmove(0.0), myXRot(0.0f), myYRot(0.0f), myZRot(0.0f), first(true), localQuat(RsQuaternion()), totalQuat(RsQuaternion())
{
	initMatrices();
}

RsCamera::~RsCamera() {
	// TODO Auto-generated destructor stub
}


/* CAMERAPART BEGIN */

void RsCamera::initMatrices()
{
	myTranslateMatrix[0]  = 1.0f;
	myTranslateMatrix[1]  = 0.0f;
	myTranslateMatrix[2]  = 0.0f;
	myTranslateMatrix[3]  = 0.0f;

	myTranslateMatrix[4]  = 0.0f;
	myTranslateMatrix[5]  = 1.0f;
	myTranslateMatrix[6]  = 0.0f;
	myTranslateMatrix[7]  = 0.0f;

	myTranslateMatrix[8]  = 0.0f;
	myTranslateMatrix[9]  = 0.0f;
	myTranslateMatrix[10] = 1.0f;
	myTranslateMatrix[11] = 0.0f;

	myTranslateMatrix[12] = 0.0f;
	myTranslateMatrix[13] = 0.0f;
	myTranslateMatrix[14] = 0.0f;
	myTranslateMatrix[15] = 1.0f;

	myGLRotMatrix[0]  = 1.0f;
	myGLRotMatrix[1]  = 0.0f;
	myGLRotMatrix[2]  = 0.0f;
	myGLRotMatrix[3]  = 0.0f;

	myGLRotMatrix[4]  = 0.0f;
	myGLRotMatrix[5]  = 1.0f;
	myGLRotMatrix[6]  = 0.0f;
	myGLRotMatrix[7]  = 0.0f;

	myGLRotMatrix[8]  = 0.0f;
	myGLRotMatrix[9]  = 0.0f;
	myGLRotMatrix[10] = 1.0f;
	myGLRotMatrix[11] = 0.0f;

	myGLRotMatrix[12] = 0.0f;
	myGLRotMatrix[13] = 0.0f;
	myGLRotMatrix[14] = 0.0f;
	myGLRotMatrix[15] = 1.0f;
}

void RsCamera::calcMatrix()
{
	localQuat.fromEulerAngles(myYRot, myZRot, myXRot);
	totalQuat.reset();
	totalQuat = localQuat * totalQuat;
	totalQuat.getRotationMatrix(myQuatRotMat);

	multMatrix(myQuatRotMat, myGLRotMatrix, myTempMat);

        myTranslateMatrix[12] = xmove;
        myTranslateMatrix[13] = ymove;
        myTranslateMatrix[14] = zmove;

	multMatrix(myTranslateMatrix, myTempMat, myGLRotMatrix);

	glMultMatrixf(myGLRotMatrix);

	myXRot = myYRot = myZRot = 0.0f;
	xmove = ymove = zmove = 0.0f;
}

void RsCamera::multMatrix(float *m1, float *m2, float *res)
{
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j){
			res[i*4 + j] = m1[j]*m2[i*4] + m1[j+4]*m2[i*4+1] + m1[j+8]*m2[i*4+2] + m1[j+12]*m2[i*4+3];
		}
	}
}

void RsCamera::setRotationMatrix(const float* mat)
{
	memcpy(myGLRotMatrix, mat, sizeof(float)*16);
}


/* CAMERAPART END */
