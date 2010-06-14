/*
 * RsStructs.h
 *
 *  Created on: 14.03.2010
 *      Author: TheAvatar
 */

#ifndef RSSTRUCTS_H_
#define RSSTRUCTS_H_

#include <vector>
#include <map>
#include <string>

#include <GL/glew.h>

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
	bool operator==(const Tuplef& _rhs) const;
	bool operator<(const Tuplef& _rhs) const;
	float x;
	float y;
};

struct Triplef
{
	Triplef();
	Triplef(float _x, float _y, float _z);
	bool operator<(const Triplef& _rhs) const;
	bool operator==(const Triplef& _rhs) const;
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

struct Triplec
{
	Triplec();
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

struct Quadruplec
{
	Quadruplec();
	bool operator==(const Quadruplec& _rhs) const;
	bool operator<(const Quadruplec& _rhs) const;
	char x;
	char y;
	char z;
	char w;
};

struct RsV4
{
	Quadruplef v;
};

struct RsV3N4
{
	RsV3N4();
	RsV3N4(const float* _v, const char* _n);
	bool operator<(const RsV3N4& _rhs) const;
	Triplef v;
	Quadruplec n;
};

struct RsV3N4T3
{
	Triplef v;
	Quadruplec n;
	Triplef t3D;
#ifdef __x86_64
	unsigned dummy;
#endif
};

struct RsV3N4T2
{
	RsV3N4T2();
	RsV3N4T2(const float* _v, const float* _t, const char* _n);
	bool operator<(const RsV3N4T2& _rhs) const;
	Triplef v;
	Quadruplec n;
	Tuplef t;
};

struct RsV3N4T2T3
{
	Triplef v;
	Quadruplec n;
	Tuplef t;
	Triplef t3D;
#ifdef __x86_64
	unsigned dummy;
#endif
};

struct RsV4T2
{
	RsV4T2();
	RsV4T2(const float* _v, const char* _t);
	RsV4T2(float _vx, float _vy, float _vz, float _tx, float _ty);
	Quadruplef v;
	Tuplef t;
};

struct Material{
	Material();
	float shininess;
	Triplef ambient;
	Triplef diffuse;
	Triplef specular;
	GLuint texture;
};

struct ObjInfo
{
	ObjInfo();
	unsigned groupCount;
	unsigned vertexCount;
	unsigned texCount;
	unsigned faceCount;
	unsigned normalCount;
	unsigned materialCount;
	std::vector<unsigned> groupBits;
	std::vector<unsigned> groupFaces;
	std::map<std::string, Material> materials;
};

inline char* bufferOffset(int i){return (char *)0 + (i);}

#endif /* RSSTRUCTS_H_ */
