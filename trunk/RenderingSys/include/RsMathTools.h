/**
 * @file	RsMathTools.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2010
 * @brief 	RsMathTools class declaration.
 */

#ifndef RSMATHTOOLS_H_
#define RSMATHTOOLS_H_

#include <vector>
#include "RsStructs.h"
#include "RsVectorMath.h"

class RsMathTools {
public:
	RsMathTools();
	virtual ~RsMathTools();
	long sqrt(long _i) const;
	bool isPrime(long _i) const;
	unsigned findPrimesRange(long _min, long _max, std::vector<long>* _primes) const;
	float linInterpolate(float _a, float _b, float _factor) const;
	float cosInterpolate(float _a, float _b, float _factor) const;
	float cubicInterpolate(float _a, float _b, float _c, float _d, float _factor);
	unsigned char* generateRandomTexture(unsigned _dim, unsigned _comp, bool _createColor);
	float* generatePerlinTexture(unsigned _x, unsigned _y, bool _createColor);
	float* generatePerlinTexture(unsigned _x, unsigned _y, unsigned _z, bool _createColor);
	float abs(float _val);

	float noise(int x);
	float noise(int x, int y);
	float smoothedNoise(float x);
	float smoothedNoise(float x, float y);
	float interpolatedNoise(float x);
	float interpolatedNoise(float x, float y);
	float perlinNoise1D(float x);
	float perlinNoise2D(float x, float y);

	static float pi;
};

#endif /* RSMATHTOOLS_H_ */
