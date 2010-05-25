/**
 * @file	ObjModel.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	ObjModel class declaration.
 */

#include "ObjModel.h"

#include <cmath>
#include <map>

#include "VboV4.h"
#include "VboV3N4.h"
#include "VboV4T2.h"
#include "VboV3N4T2.h"
#include "RsStructs.h"

ObjModel::ObjModel() : mPriVboCount(0), mPriVbos(0)
{

}

ObjModel::ObjModel(const ObjInfo* _info) : mPriVboCount(0), mPriVbos(0)
{
	if (_info != 0){
		mPriVboCount = _info->groupCount;
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

void ObjModel::addVbo(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals, const float* _texCoords, const unsigned char* _colors)
{
	std::map<RsV4T2, unsigned> V4T2Set = std::map<RsV4T2, unsigned>();
	std::map<RsV3N4, unsigned> V3N4Set = std::map<RsV3N4, unsigned>();
	std::map<RsV4, unsigned> V4Set = std::map<RsV4, unsigned>();
	unsigned indices[_info->groupFaces[_gIdx]*3];
	unsigned mapSize = 0;
	if (_info != 0){
		if (_info->groupBits[_gIdx] & 1 && _info->groupBits[_gIdx] & 2){ // normals AND textures
			std::map<RsV3N4T2, unsigned> V3N4T2Map = std::map<RsV3N4T2, unsigned>();
			std::map<RsV3N4T2, unsigned>::iterator it;
			for (unsigned i=0; i<_info->groupFaces[_gIdx]; i++){
				RsV3N4T2 entry1 = RsV3N4T2(_vertices+_group[(i*9)+0], _normals+_group[(i*9)+1], _texCoords+_group[(i*9)+2]);
				RsV3N4T2 entry2 = RsV3N4T2(_vertices+_group[(i*9)+3], _normals+_group[(i*9)+4], _texCoords+_group[(i*9)+5]);
				RsV3N4T2 entry3 = RsV3N4T2(_vertices+_group[(i*9)+6], _normals+_group[(i*9)+7], _texCoords+_group[(i*9)+8]);
				it = V3N4T2Map.find(entry1);
				if (it == V3N4T2Map.end()){ // entry does not exists .. yet
					V3N4T2Map.insert(std::make_pair(entry1, mapSize));
					indices[i] = mapSize;
					mapSize++;
				}
				else {
					indices[i*3] = it->second;
				}

				it = V3N4T2Map.find(entry2);
				if (it == V3N4T2Map.end()){ // entry does not exists .. yet
					V3N4T2Map.insert(std::make_pair(entry2, mapSize));
					indices[(i*3)+1] = mapSize;
					mapSize++;
				}
				else {
					indices[(i*3)+1] = it->second;
				}

				it = V3N4T2Map.find(entry3);
				if (it == V3N4T2Map.end()){ // entry does not exists .. yet
					V3N4T2Map.insert(std::make_pair(entry3, mapSize));
					indices[(i*3)+2] = mapSize;
					mapSize++;
				}
				else {
					indices[(i*3)+2] = it->second;
				}
			}
			mPriVbos[_gIdx] = new VboV3N4T2(_info->groupFaces[_gIdx]*3, indices, &V3N4T2Map);
			mPriVbos[_gIdx]->debug();
		}
		else if (_info->groupBits[_gIdx] & 1){ // textures
			mPriVbos[_gIdx] = new VboV4T2();
			V4T2Set.clear();
			for (unsigned j=0; j<_info->groupFaces[_gIdx]; j++){
			}
		}
		else if (_info->groupBits[_gIdx] & 2){ // normals
			mPriVbos[_gIdx] = new VboV3N4();
			V3N4Set.clear();
			for (unsigned j=0; j<_info->groupFaces[_gIdx]; j++){

			}
		}
		else {
			mPriVbos[_gIdx] = new VboV4(); // just vertices
			V4Set.clear();
			for (unsigned j=0; j<_info->groupFaces[_gIdx]; j++){

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
