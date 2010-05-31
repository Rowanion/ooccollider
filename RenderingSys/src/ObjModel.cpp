/**
 * @file	ObjModel.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	ObjModel class declaration.
 * @todo 	Need individualized shader for each VBO-Type
 */

#include "ObjModel.h"

#include <cmath>
#include <map>
#include <cstring>
#include <iostream>
#include <cstdlib>

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
//				mPriVbos[i] = new VboV3N4T2();
			}
			else if (_info->groupBits[i] & 1){
//				mPriVbos[i] = new VboV4T2();
			}
			else if (_info->groupBits[i] & 2){
//				mPriVbos[i] = new VboV3N4();
			}
			else {
//				mPriVbos[i] = new VboV4();
			}

		}
	}
}

void ObjModel::addVbo(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals, const float* _texCoords, const unsigned char* _colors)
{
	std::map<RsV4T2, unsigned> V4T2Map = std::map<RsV4T2, unsigned>();
	std::map<RsV4, unsigned> V4Map = std::map<RsV4, unsigned>();
	unsigned indices[_info->groupFaces[_gIdx]*3];
	unsigned mapSize = 0;
	if (_info != 0){
		if ((_info->groupBits[_gIdx] & 1) && (_info->groupBits[_gIdx] & 2)){ // normals AND textures
			std::map<RsV3N4T2, unsigned> V3N4T2Map = std::map<RsV3N4T2, unsigned>();
			std::map<RsV3N4T2, unsigned>::iterator it;
			for (unsigned i=0; i<_info->groupFaces[_gIdx]; i++){
				// re-compose the vertex- and index-data.
				// vertices are stored as v_a1, v_a2, v_a3, v_b1, v_b2, v_b3, v_b1,...
				// normals are stored as n_a1, n_a2, n_a3, n_b1, n_b2...
				// and texCoords as t_a1, t_a2, t_b1, t_b2, t_c1,...

				// Warning: there is no 4th component for normals.
				// at this stage the normals have only 3 (!!) components. The 4th (be with you) will be added in the vbo-constructor.

				// A single index in _group[groupId] refers either to a vertex-triple, a normal-quadruple or a tex-tuple.
				// Because all indices are stored in a single block, ie.
				//		vi_a1, ti_a1, ni_a1, vi_a2, t1_a2, ni_a2, vi_a3, ti_a3, ni_a3
				// 		=> this represents all indices for one single triangle (a) in the index-array.
				// the *9-multiplicand jumps to face i; the added number refers to a single component within the face, i.e. a v-index
				// and the last multiplicand (3, 4 or 2) jumps to the first component for the particular element, i.e. *2 for a tuple a.k.a texCoords
				// A face = triangle has 9 entries => a vertex, a normal and a texCoord (3) for each point of the tri (3) = 9
				RsV3N4T2 entry1 = RsV3N4T2(_vertices+(_group[(i*9)+0])*3, _texCoords+(_group[(i*9)+1])*2, _normals+(_group[(i*9)+2])*3);
				RsV3N4T2 entry2 = RsV3N4T2(_vertices+(_group[(i*9)+3])*3, _texCoords+(_group[(i*9)+4])*2, _normals+(_group[(i*9)+5])*3);
				RsV3N4T2 entry3 = RsV3N4T2(_vertices+(_group[(i*9)+6])*3, _texCoords+(_group[(i*9)+7])*2, _normals+(_group[(i*9)+8])*3);
				it = V3N4T2Map.find(entry1);
				if (it == V3N4T2Map.end()){ // entry does not exists .. yet
					V3N4T2Map.insert(std::make_pair(entry1, mapSize));
					indices[i*3] = mapSize;
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
		else if (_info->groupBits[_gIdx] & 1){ // only textures
			mPriVbos[_gIdx] = new VboV4T2();
			V4T2Map.clear();
			for (unsigned j=0; j<_info->groupFaces[_gIdx]; j++){
			}
		}
		else if (_info->groupBits[_gIdx] & 2){ // only normals
			std::map<RsV3N4, unsigned> V3N4Map = std::map<RsV3N4, unsigned>();
			std::map<RsV3N4, unsigned>::iterator it;
			for (unsigned i=0; i<_info->groupFaces[_gIdx]; i++){
				// re-compose the vertex- and index-data.
				// vertices are stored as v_a1, v_a2, v_a3, v_b1, v_b2, v_b3, v_b1,...
				// normals are stored as n_a1, n_a2, n_a3, n_b1, n_b2...

				// Warning: there is no 4th component for normals.
				// at this stage the normals have only 3 (!!) components. The 4th (be with you) will be added in the vbo-constructor.

				// A single index in _group[groupId] refers either to a vertex-triple, a normal-quadruple or a tex-tuple.
				// Because all indices are stored in a single block, ie.
				//		vi_a1, ni_a1, vi_a2, ni_a2, vi_a3, ni_a3
				// 		=> this represents all indices for one single triangle (a) in the index-array.
				// the *6-multiplicand jumps to face i; the added number refers to a single component within the face, i.e. a v-index
				// and the last multiplicand (3, 4 or 2) jumps to the first component for the particular element.
				RsV3N4 entry1 = RsV3N4(_vertices+(_group[(i*6)+0])*3, _normals+(_group[(i*6)+1])*3);
				RsV3N4 entry2 = RsV3N4(_vertices+(_group[(i*6)+2])*3, _normals+(_group[(i*6)+3])*3);
				RsV3N4 entry3 = RsV3N4(_vertices+(_group[(i*6)+4])*3, _normals+(_group[(i*6)+5])*3);

				// searching for triangle vertex 1....
				it = V3N4Map.find(entry1);
				if (it == V3N4Map.end()){ // entry does not exists .. yet
					V3N4Map.insert(std::make_pair(entry1, mapSize));
					indices[i*3] = mapSize;
					mapSize++;
				}
				else {
					indices[i*3] = it->second;
				}

				// searching for triangle vertex 2....
				it = V3N4Map.find(entry2);
				if (it == V3N4Map.end()){ // entry does not exists .. yet
					V3N4Map.insert(std::make_pair(entry2, mapSize));
					indices[(i*3)+1] = mapSize;
					mapSize++;
				}
				else {
					indices[(i*3)+1] = it->second;
				}

				// searching for triangle vertex 3....
				it = V3N4Map.find(entry3);
				if (it == V3N4Map.end()){ // entry does not exists .. yet
					V3N4Map.insert(std::make_pair(entry3, mapSize));
					indices[(i*3)+2] = mapSize;
					mapSize++;
				}
				else {
					indices[(i*3)+2] = it->second;
				}
			}

			mPriVbos[_gIdx] = new VboV3N4(_info->groupFaces[_gIdx]*3, indices, &V3N4Map);
			mPriVbos[_gIdx]->debug();
		}
		else {
			mPriVbos[_gIdx] = new VboV4(); // just vertices
			V4Map.clear();
			for (unsigned j=0; j<_info->groupFaces[_gIdx]; j++){

			}
		}
	}
}

void ObjModel::addVboDebug(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals, const float* _texCoords, const unsigned char* _colors)
{
	idxCount = _info->groupFaces[_gIdx]*3;
	vCount = _info->vertexCount;

	vData = new float[vCount*3];
	iData = new unsigned[idxCount];

	memcpy(vData, _vertices, sizeof(float)*_info->vertexCount*3);
	for (unsigned i=0; i< _info->groupFaces[_gIdx]; ++i){
		iData[i*3] = _group[(i*9)];
		iData[(i*3)+1] = _group[(i*9)+3];
		iData[(i*3)+2] = _group[(i*9)+6];
	}
//	memcpy(iData, _group, sizeof(unsigned)*_info->groupFaces[_gIdx]*3);

	unsigned min = 1000000000;
	unsigned max = 0;
	for (unsigned i=0; i< idxCount; ++i){
		std::cerr << i << ": " << iData[i] << std::endl;
		if (max < iData[i]) max = iData[i];
		if (min > iData[i]) min = iData[i];
	}
	std::cerr << "min: " << min << "; max: " << max << std::endl;
	std::cerr << "vertices: " << vCount << std::endl;
//	exit(0);
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

void ObjModel::draw()
{
	for (unsigned i=0; i<mPriVboCount; ++i){
		mPriVbos[i]->draw();
	}
}

void ObjModel::drawDebug()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 3*sizeof(float), vData);
	glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, iData);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ObjModel::calcWrapingTexCoords(bool _calcIndividual)
{
	//TODO
}
