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
	virtual ~ObjModel();
	void addVbo(const Vbo* _vbo);
private:
	unsigned mPriVboCount;
	Vbo** mPriVbos;
};

#endif /* OBJMODEL_H_ */
