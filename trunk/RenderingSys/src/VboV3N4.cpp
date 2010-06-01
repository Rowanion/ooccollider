/**
 * @file	VboV3N4.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV3N4 class declaration.
 */

#include "VboV3N4.h"

#include <GL/glew.h>

#include <cstring>
#include <float.h>
#include <iostream>
#include <cstdlib>
#include <map>

VboV3N4::VboV3N4(unsigned _indexCount, const unsigned* _indices, const std::map<RsV3N4, unsigned>* _data) :
	mProDataCount(0), mProIndexCount(_indexCount), mPriMath(RsMathTools())
{
	std::map<RsV3N4, unsigned>::const_iterator it;
	unsigned entryCount = _data->size();
	mProData = new RsV3N4T3[entryCount];
	mProIndices = new unsigned[_indexCount];
	// ----------------------------------

//	unsigned* testarray = new unsigned[_indexCount];
//	for (unsigned i=0; i<_indexCount; i++){
//		testarray[i]= 0;
//	}
//	for (std::map<RsV3N4, unsigned>::const_iterator iter = _data->begin(); iter != _data->end(); iter++){
//		testarray[iter->second] = 1;
//	}
//	for (unsigned i=0; i<_indexCount; i++){
//		if (testarray[i]== 0){
//			std::cerr << "entry " << i << " is missing!"<< std::endl;
//		}
//	}
//	delete[] testarray;
//	exit(0);

	// ----------------------------------
	memcpy(mProIndices, _indices, sizeof(unsigned)*_indexCount);

	// generating 3D texture coordinates from vertex-data.
	// This surrounds each VBO with a boundingbox in texture coordinates
	// ranging from 0.0^3 to 1.0^3 - as texture coordinates tend to like it.
	float minX = FLT_MAX;
	float maxX = FLT_MIN;
	float minY = FLT_MAX;
	float maxY = FLT_MIN;
	float minZ = FLT_MAX;
	float maxZ = FLT_MIN;

	for (it = _data->begin(); it != _data->end(); it++){
		memcpy(mProData+it->second, &it->first, sizeof(RsV3N4));
		if (mProData[it->second].v.x>maxX) maxX = mProData[it->second].v.x;
		if (mProData[it->second].v.x<minX) minX = mProData[it->second].v.x;
		if (mProData[it->second].v.y>maxY) maxY = mProData[it->second].v.y;
		if (mProData[it->second].v.y<minY) minY = mProData[it->second].v.y;
		if (mProData[it->second].v.z>maxZ) maxZ = mProData[it->second].v.z;
		if (mProData[it->second].v.z<minZ) minZ = mProData[it->second].v.z;
		mProDataCount++;
	}

	float rangeX = 1.0f / mPriMath.abs(minX - maxX);
	float rangeY = 1.0f / mPriMath.abs(minY - maxY);
	float rangeZ = 1.0f / mPriMath.abs(minZ - maxZ);
	for (unsigned i=0; i<entryCount; ++i){
		mProData[i].t3D.x = (mProData[i].v.x - minX) * rangeX;
		mProData[i].t3D.y = (mProData[i].v.y - minY) * rangeY;
		mProData[i].t3D.z = (mProData[i].v.z - minZ) * rangeZ;
	}

	// ------------------------------------------------
	// GL - VBO Init stuff from here
	// ------------------------------------------------
	glGenBuffers(1, &mPriDataId);
	glBindBuffer(GL_ARRAY_BUFFER, mPriDataId);

	glBufferData(GL_ARRAY_BUFFER, entryCount*sizeof(RsV3N4T3), mProData, GL_STATIC_DRAW);

	glGenBuffers(1, &mPriIndexId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIndexId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mProIndexCount*sizeof(unsigned), mProIndices, GL_STATIC_DRAW);


	// cleaning up - we have our data exclusively in GPU-Ram
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;
}

VboV3N4::~VboV3N4() {
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;
}

unsigned VboV3N4::getIndexCount() const
{
	return mProIndexCount;
}

unsigned VboV3N4::getVertexCount() const
{
	return mProDataCount;
}

uint64_t VboV3N4::getComponentBytes() const
{
	return sizeof(RsV3N4);
}

void VboV3N4::draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, mPriDataId);
	glVertexPointer(3, GL_FLOAT, sizeof(RsV3N4T3), bufferOffset(0));
	glNormalPointer(GL_BYTE, sizeof(RsV3N4T3), bufferOffset(sizeof(float)*3));
	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(3, GL_FLOAT, sizeof(RsV3N4T3), bufferOffset(sizeof(float)*3 + sizeof(char)*4));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIndexId);
	glDrawElements(GL_TRIANGLES, mProIndexCount, GL_UNSIGNED_INT, bufferOffset(0));

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
