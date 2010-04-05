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
	float data[2];
};

struct Triplef
{
	Triplef();
	float data[3];
};

struct Tripleuc
{
	Tripleuc();
	unsigned char data[3];
};

struct Quadruplef
{
	Quadruplef();
	float data[4];
};

struct Quadrupleuc
{
	Quadrupleuc();
	unsigned char data[4];
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
