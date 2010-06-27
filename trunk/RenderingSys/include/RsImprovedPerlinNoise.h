/**
 * @file	RsImprovedPerlinNoise.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	RsImprovedPerlinNoise class declaration.
 */

#ifndef RSIMPROVEDPERLINNOISE_H_
#define RSIMPROVEDPERLINNOISE_H_

/**
 * @class RsImprovedPerlinNoise
 * @brief Implementation of different Perlin noise functions based upon http://http.developer.nvidia.com/GPUGems/gpugems_ch05.html
 */
class RsImprovedPerlinNoise {
public:
	RsImprovedPerlinNoise();
	virtual ~RsImprovedPerlinNoise();
	/**
	 * @brief generate perlin noise at point x,y,z
	 * @param x
	 * @param y
	 * @param z
	 * @return seems to return double values between -1 and 1 \f$\rightarrow\f$ those need normalization for texture-usage.
	 */
	double noise(double _x, double _y, double _z);
	double noiseV1(double _x, double _y, double _z);

	/**
	 * @brief Little warning: value-range changed in this function because of abs-val: 0-1.
	 * @param x
	 * @param y
	 * @param z
	 * @return
	 */
	double noiseV2(double _x, double _y, double _z);
	double noiseV3(double _x, double _y, double _z);
	double fade(double _t);
	double pLerp(double _t, double _a, double _b);
	double grad(int _hash, double _x, double _y, double _z);
	double abs(double _val);
	static int p[];
	static const unsigned char permutation[];
private:
	static bool isInitialized;
};

#endif /* RSIMPROVEDPERLINNOISE_H_ */
