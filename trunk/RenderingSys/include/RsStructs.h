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

#endif /* RSSTRUCTS_H_ */
