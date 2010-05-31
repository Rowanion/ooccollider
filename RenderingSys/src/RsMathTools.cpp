/**
 * @file	RsMathTools.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2010
 * @brief 	RsMathTools class declaration.
 */

#include "RsMathTools.h"

#include <cmath>
#include <iostream>
#include <cstdlib>

#include "MersenneTwister.h"
#include "perlin.h"
#include "ImprovedPerlinNoise.h"

float RsMathTools::pi = 3.1415927;

RsMathTools::RsMathTools() {
	// TODO Auto-generated constructor stub

}

RsMathTools::~RsMathTools() {
	// TODO Auto-generated destructor stub
}


long RsMathTools::sqrt(long _i) const
{
	long r,rnew=1,rold=r;

	do
	{
		rold=r;
		r=rnew;
		rnew = (r+(_i/r));
		rnew >>= 1;
	}
	while(rold != rnew);
	return rnew;
}

bool RsMathTools::isPrime(long _i) const
{
	long si,j;

	si = this->sqrt(_i);

	for (j=2; (j<=si); j++)
	{
		if (_i%j == 0)
			return false;
	}

	return true;

}

unsigned RsMathTools::findPrimesRange(long _min, long _max, std::vector<long>* _primes) const
{
	long i,i1,i2,f=0;
	unsigned primCount = 0;

	i1 = _min;
	i2 = _max;

	for (i=i1; i<i2; i++)
		if (this->isPrime(i))
		{
			f++;
			if (f==16)
			{
//				cout << endl;
				f=0;
			}
			_primes->push_back(i);
			primCount++;
		}
	if (primCount>0){
		return primCount;
	}
	else {
		return 0;
	}
}

float RsMathTools::linInterpolate(float _a, float _b, float _factor) const
{
	return _a*(1.0f-_factor) + _b*_factor;
}

float RsMathTools::cosInterpolate(float _a, float _b, float _factor) const
{
	float ft = _factor * RsMathTools::pi;
	float f = (1.0f - cos(ft)) * 0.5f;

	return  _a*(1.0f-f) + _b*f;
}

float RsMathTools::cubicInterpolate(float _a, float _b, float _c, float _d, float _factor)
{
	float f2 = _factor*_factor;
	float a0 = (_d - _c) - (_a - _b);
	float a1 = (_a - _b) - a0;
	float a2 = _c - _a;
	float a3 = _b;

	return (a0*_factor*f2 + a1*f2 + a2*_factor + a3);
}

unsigned char* RsMathTools::generateRandomTexture(unsigned _dim, unsigned _comp, bool _createColor)
{
	MTRand mt = MTRand();

	unsigned char* tmp = new unsigned char[_dim*_comp];
	if (_createColor){
		for (unsigned i=0; i<_dim*_comp; i++){
			tmp[i] = (unsigned char)mt.randInt();
		}
	}
	else {
		for (unsigned i=0; i<_dim*_comp; i+=_comp){
			tmp[i] = (unsigned char)mt.randInt();
			for (unsigned j=1; j<_comp; ++j){
				tmp[i+j] = tmp[i];
			}
		}
	}
	return tmp;
}

float* RsMathTools::generatePerlinTexture(unsigned _x, unsigned _y, bool _createColor)
{
	PerlinNoise pn = PerlinNoise();
	ImprovedPerlinNoise ipn = ImprovedPerlinNoise();

	float min = 10000.0f;
	float max = -10000.0f;

	float xDiv = 4.0/(float)_x;
	float yDiv = 4.0/(float)_y;
	float* tmp = new float[_x*_y];
	for (unsigned i=0; i<_x; i++){
		for (unsigned k=0; k<_y; k++){
//			tmp[(i*_y)+k] = (float)pn.PerlinNoise2D((xDiv*i)-1.0, (yDiv*k)-1.0, 1.0, 2.0, 16);
			tmp[(i*_y)+k] = 0.5f*(float)ipn.noiseV3((float)(xDiv*i), (float)(yDiv*k), 1.7)+0.5f ;
//			tmp[(i*_y)+k] = perlinNoise2D((float)i, (float)k);
			if (tmp[(i*_y)+k] < min){
				min = tmp[(i*_y)+k];
			}
			if (tmp[(i*_y)+k] > max){
				max = tmp[(i*_y)+k];
			}
//			if (tmp[(i*_y)+k]<=0){
//				tmp[(i*_y)+k]=0.0;
//			}
//			if (tmp[(i*_y)+k]!=0){
//				std::cerr << "non-zero: "<< tmp[(i*_y)+k] << std::endl;
//				exit(0);
//			}
		}
	}
	std::cerr << "min: " << min << ", max: " << max << std::endl;
	return tmp;

}

float* RsMathTools::generatePerlinTexture(unsigned _x, unsigned _y, unsigned _z, bool _createColor)
{
	//TODO try to create 1byte-texture opposed to float
	PerlinNoise pn = PerlinNoise();
	ImprovedPerlinNoise ipn = ImprovedPerlinNoise();

	float min = 10000.0f;
	float max = -10000.0f;

	float xDiv = 4.0/(float)_x;
	float yDiv = 4.0/(float)_y;
	float zDiv = 4.0/(float)_z;
	float* tmp = new float[_x*_y*_z];
	for (unsigned x=0; x<_x; x++){
		for (unsigned y=0; y<_y; y++){
			for (unsigned z=0; z<_z; z++){
				//			tmp[(i*_y)+k] = (float)pn.PerlinNoise2D((xDiv*i)-1.0, (yDiv*k)-1.0, 1.0, 2.0, 16);
				tmp[(_x*_y*z)+(y*_y)+x] = 0.5f*(float)ipn.noiseV3((float)(xDiv*x), (float)(yDiv*y), (float)(zDiv*z))+0.5f ;
				//			tmp[(i*_y)+k] = perlinNoise2D((float)i, (float)k);
				if (tmp[(_x*_y*z)+(y*_y)+x] < min){
					min = tmp[(_x*_y*z)+(y*_y)+x];
				}
				if (tmp[(_x*_y*z)+(y*_y)+x] > max){
					max = tmp[(_x*_y*z)+(y*_y)+x];
				}
			}
		}
	}
	std::cerr << "min: " << min << ", max: " << max << std::endl;
	return tmp;

}

float RsMathTools::noise(int x)
{
    x = (x<<13) ^ x;
    return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}
float RsMathTools::noise(int x, int y)
{
    int n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float RsMathTools::smoothedNoise(float x)
{
    return noise(x)/2.0f + noise(x-1.0)/4.0f + noise(x+1.0)/4.0f;
}

float RsMathTools::smoothedNoise(float x, float y)
{
	float corners = ( noise(x-1, y-1)+noise(x+1, y-1)+noise(x-1, y+1)+noise(x+1, y+1) ) / 16.0f;
	float sides   = ( noise(x-1, y)  +noise(x+1, y)  +noise(x, y-1)  +noise(x, y+1) ) /  8.0f;
	float center  =  noise(x, y) / 4.0f;
	return corners + sides + center;
}

float RsMathTools::interpolatedNoise(float x)
{
	float integer_X = (int) x;
    float fractional_X = ((float)x - integer_X);

    float v1 = smoothedNoise(integer_X);
    float v2 = smoothedNoise(integer_X + 1.0f);

    return linInterpolate(v1, v2, fractional_X);
}

float RsMathTools::interpolatedNoise(float x, float y)
{
    float integer_X    = int(x);
    float fractional_X = ((float)x - integer_X);

    float integer_Y    = int(y);
    float fractional_Y = ((float)y - integer_Y);

    float v1 = smoothedNoise(integer_X,     integer_Y);
    float v2 = smoothedNoise(integer_X + 1.0f, integer_Y);
    float v3 = smoothedNoise(integer_X,     integer_Y + 1.0f);
    float v4 = smoothedNoise(integer_X + 1.0f, integer_Y + 1.0f);

    float i1 = linInterpolate(v1 , v2 , fractional_X);
    float i2 = linInterpolate(v3 , v4 , fractional_X);

    return linInterpolate(i1 , i2 , fractional_Y);

}

float RsMathTools::perlinNoise1D(float x)
{
	float total = 0;
    float p = 1/2;  // persistence;
    unsigned n = 10; // number of octaves
	unsigned frequency = 1;
	float amplitude = 1;

    for (unsigned i=0; i<n; ++i){
    	for (unsigned j=0; j<i; ++j){

            total += interpolatedNoise(x * frequency) * amplitude;
    		amplitude *= p;
    		frequency *= 2;
    	}
    }
    return total;

}

float RsMathTools::perlinNoise2D(float x, float y)
{
	float total = 0;
	float p = 1/2; //persistence
	unsigned n = 6; //Number_Of_Octaves
	float amplitude = 1;
	unsigned frequency = 1;

	for (unsigned i=0; i<n; ++i){
		for (unsigned j=0; j<i; ++j){
			total += interpolatedNoise(x * frequency, y * frequency) * amplitude;
			amplitude *= p;
			frequency *= 2;
		}
	}
      return total;
}

float RsMathTools::abs(float _val)
{
	if (_val<0){
		return -1.0*_val;
	}
	return _val;
}
