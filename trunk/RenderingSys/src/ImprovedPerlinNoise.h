/**
 * @file	ImprovedPerlinNoise.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	ImprovedPerlinNoise class declaration.
 */

#ifndef IMPROVEDPERLINNOISE_H_
#define IMPROVEDPERLINNOISE_H_

class ImprovedPerlinNoise {
public:
	ImprovedPerlinNoise();
	virtual ~ImprovedPerlinNoise();
	/**
	 * @brief generate perlin noise at point x,y,z
	 * @param x
	 * @param y
	 * @param z
	 * @return seems to return double values between -1 and 1 \f$\rightarrow\f$ those need normalization for texture-usage.
	 */
	double noise(double x, double y, double z);
	double noiseV1(double x, double y, double z);

	/**
	 * @brief Little warning: value-range changed in this function because of abs-val: 0-1.
	 * @param x
	 * @param y
	 * @param z
	 * @return
	 */
	double noiseV2(double x, double y, double z);
	double noiseV3(double x, double y, double z);
	double fade(double t);
	double pLerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
	double abs(double val);
	static int p[];
	static const unsigned char permutation[];
private:
	static bool isInitialized;
};

#endif /* IMPROVEDPERLINNOISE_H_ */
