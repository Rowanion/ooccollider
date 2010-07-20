/**
 * @file	RsStructs.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	Collection of structs and data type declarations.
 */

#ifndef RSSTRUCTS_H_
#define RSSTRUCTS_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include <GL/glew.h>

#define TGA_ALPHA 0x1906
#define TGA_RGB 0x1907
#define TGA_RGBA 0x1908

/**
 * @struct RsTGAimage
 * @brief Holds meta-information about the image it represents, as well as the binary imagedata.
 */
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

/**
 * @struct RsTuplef
 * @brief A struct for a 2-component float container with basic operations.
 */
struct RsTuplef
{
	RsTuplef();
	bool operator==(const RsTuplef& _rhs) const;
	bool operator<(const RsTuplef& _rhs) const;
	float x;
	float y;
};

/**
 * @struct RsTriplef
 * @brief A struct for a 3-component float container with basic operations.
 */
struct RsTriplef
{
	RsTriplef();
	RsTriplef(float _x, float _y, float _z);
	bool operator<(const RsTriplef& _rhs) const;
	bool operator==(const RsTriplef& _rhs) const;
	float x;
	float y;
	float z;
};

/**
 * @struct RsTripleuc
 * @brief A struct for a 3-component unsigned char container.
 */
struct RsTripleuc
{
	RsTripleuc();
	unsigned char x;
	unsigned char y;
	unsigned char z;
};

/**
 * @struct RsTriplec
 * @brief A struct for a 3-component signed char container.
 */
struct RsTriplec
{
	RsTriplec();
	unsigned char x;
	unsigned char y;
	unsigned char z;
};

/**
 * @struct RsQuadruplef
 * @brief A struct for a 4-component float container.
 */
struct RsQuadruplef
{
	RsQuadruplef();
	float x;
	float y;
	float z;
	float w;
};

/**
 * @struct RsQuadrupleuc
 * @brief A struct for a 4-component unsigned char container.
 */
struct RsQuadrupleuc
{
	RsQuadrupleuc();
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char w;
};

/**
 * @struct RsQuadruplec
 * @brief A struct for a 4-component signed char container with basic operations.
 */
struct RsQuadruplec
{
	RsQuadruplec();
	bool operator==(const RsQuadruplec& _rhs) const;
	bool operator<(const RsQuadruplec& _rhs) const;
	char x;
	char y;
	char z;
	char w;
};

/**
 * @struct RsV4
 * @brief A struct for a 4-component vertex; due to alignment issues.
 */
struct RsV4
{
	RsQuadruplef v;
};

/**
 * @struct RsV3N4
 * @brief A struct for a 3-component vertex and a 4 component normal vector; due to alignment issues.
 */
struct RsV3N4
{
	RsV3N4();
	RsV3N4(const float* _v, const char* _n);
	bool operator<(const RsV3N4& _rhs) const;
	RsTriplef v;
	RsQuadruplec n;
};

/**
 * @struct RsV3N4T3
 * @brief A struct for a 3-component vertex, a 4 component normal vector and a 3D texture coordinate; due to alignment issues.
 */
struct RsV3N4T3
{
	RsTriplef v;
	RsQuadruplec n;
	RsTriplef t3D;
#ifdef __x86_64
	unsigned dummy;
#endif
};

/**
 * @struct RsV3N4T2
 * @brief A struct for a 3-component vertex, a 4 component normal vector and a 2D texture coordinate; due to alignment issues.
 */
struct RsV3N4T2
{
	RsV3N4T2();
	RsV3N4T2(const float* _v, const float* _t, const char* _n);
	bool operator<(const RsV3N4T2& _rhs) const;
	RsTriplef v;
	RsQuadruplec n;
	RsTuplef t;
};

/**
 * @struct RsV3N4T2T3
 * @brief A struct for a 3-component vertex, a 4 component normal vector, a 2D texture coordinate AND a 3D texture coordinate; due to alignment issues.
 */
struct RsV3N4T2T3
{
	RsTriplef v;
	RsQuadruplec n;
	RsTuplef t;
	RsTriplef t3D;
#ifdef __x86_64
	unsigned dummy;
#endif
};

/**
 * @struct RsV4T2
 * @brief A struct for a 4-component vertex and a 2D texture coordinate; due to alignment issues.
 */
struct RsV4T2
{
	RsV4T2();
	RsV4T2(const float* _v, const char* _t);
	RsV4T2(float _vx, float _vy, float _vz, float _tx, float _ty);
	RsQuadruplef v;
	RsTuplef t;
};

/**
 * @struct RsMaterial
 * @brief Data-class for components of a material, like, ambient- or diffuse color and texture IDs.
 */
struct RsMaterial{
	RsMaterial();
	float shininess;
	RsTriplef ambient;
	RsTriplef diffuse;
	RsTriplef specular;
	unsigned id;
	GLuint texture;
	std::string name;
	bool operator<(const RsMaterial& _rhs) const;
};

/**
 * @struct RsObjInfo
 * @brief Meta-information about the OBJ-Model, collected during the first parsing.
 */
struct RsObjInfo
{
	RsObjInfo();
	unsigned groupCount;
	unsigned vertexCount;
	unsigned texCount;
	unsigned faceCount;
	unsigned normalCount;
	unsigned materialCount;

	// stores, if a group uses texture coordinates or normals or nothing.
	// vector-entry equals group ID.
	std::vector<unsigned> groupBits;

	// stores how many faces each group  has = indexlist/3 (in case of triangular faces, what we assume here.
	std::vector<unsigned> groupFaces;

	// the key is the original material-name and the value is the material itself, to transform the name into an ID
	std::map<std::string, unsigned> materialNames;

	std::set<RsMaterial> material;

};

inline char* bufferOffset(int i){return (char *)0 + (i);}

#endif /* RSSTRUCTS_H_ */
