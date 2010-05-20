/**
 * @file	ObjModel.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	ObjModel class declaration.
 */

#include "ObjModel.h"

#include <cmath>

#include "VboV4.h"
#include "VboV3N4.h"
#include "VboV4T2.h"
#include "VboV3N4T2.h"

ObjModel::ObjModel() : mPriVboCount(0), mPriVbos(0)
{

}

ObjModel::ObjModel(const ObjInfo* _info) : mPriVboCount(0), mPriVbos(0)
{
	if (_info != 0){
		mPriVboCount = std::max(_info->groupCount, (unsigned)1);
		mPriVbos = new Vbo *[mPriVboCount];
		for (unsigned i=0; i< mPriVboCount; ++i){
			if (_info->groupBits[i] & 1 && _info->groupBits[i] & 2){
				mPriVbos[i] = new VboV3N4T2();
			}
			else if (_info->groupBits[i] & 1){
				mPriVbos[i] = new VboV4T2();
			}
			else if (_info->groupBits[i] & 2){
				mPriVbos[i] = new VboV3N4();
			}
			else {
				mPriVbos[i] = new VboV4();
			}

		}
	}
}

ObjModel::~ObjModel() {
	for (unsigned i=0; i< mPriVboCount; ++i){
		delete mPriVbos[i];
		delete[] mPriVbos;
	}
}

void ObjModel::addVbo(const Vbo* _vbo)
{
	//TODO
}
