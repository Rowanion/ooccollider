/*
 * RsStructs.h
 *
 *  Created on: 14.03.2010
 *      Author: TheAvatar
 */

#ifndef RSSTRUCTS_H_
#define RSSTRUCTS_H_

#define TGA_ALPHA 0x1906
#define TGA_RGB 0x1907
#define TGA_RGBA 0x1908

struct RsTGAimage
{
	RsTGAimage();
	~RsTGAimage();
	unsigned short width;
	unsigned short height;
	unsigned int format;
	char* data;
	unsigned int intlFormat;
	unsigned int components;
};

struct Tuplef
{
	Tuplef();
	float x;
	float y;
};

struct Triplef
{
	Triplef();
	float x;
	float y;
	float z;
};

struct Tripleuc
{
	Tripleuc();
	unsigned char x;
	unsigned char y;
	unsigned char z;
};

struct Quadruplef
{
	Quadruplef();
	float x;
	float y;
	float z;
	float w;
};

struct Quadrupleuc
{
	Quadrupleuc();
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char w;
};


struct RsV3N4
{
	Triplef v;
	Quadrupleuc n;
};

struct RsV3N4T2
{
	Triplef v;
	Quadrupleuc n;
	Tuplef t;
};

struct RsV4T2
{
	Quadruplef v;
	Tuplef t;
};
#endif /* RSSTRUCTS_H_ */
