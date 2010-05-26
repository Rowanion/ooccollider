/**
 * @file	ObjModel.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	ObjModel class declaration.
 */

#ifndef OBJMODEL_H_
#define OBJMODEL_H_

#include "Vbo.h"
#include "RsStructs.h"

class ObjModel {
public:
	ObjModel();
	ObjModel(const ObjInfo* _info);
	void addVbo(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);
	void addVboDebug(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);
	virtual ~ObjModel();
	void addVbo(const Vbo* _vbo);
	void draw();
	void drawDebug();

private:
	unsigned mPriVboCount;
	Vbo** mPriVbos;

	unsigned idxCount;
	unsigned vCount;
	unsigned* iData;
	float* vData;
};

#endif /* OBJMODEL_H_ */
