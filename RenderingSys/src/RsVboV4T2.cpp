/**
 * @file	RsVboV4T2.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsVboV4T2 class declaration.
 */

#include "RsVboV4T2.h"

#include <cstring>

RsVboV4T2::RsVboV4T2(unsigned _indexCount, const unsigned* _indices, const std::map<RsV4T2, unsigned>* _data) : mProData(0), mProIndices(0){
	mProDataCount = _data->size();
	mProData = new RsV4T2[mProDataCount];
	mProIndices = new unsigned[_indexCount];
}

RsVboV4T2::RsVboV4T2(unsigned _indexCount, const unsigned* _indices, unsigned _dataCount, const RsV4T2* _data, GLenum _type)
{
	mProDataCount = _dataCount;
	mProIndexCount = _indexCount;
	mProIndices = new unsigned[mProIndexCount];
	mProData = new RsV4T2[mProDataCount];
	memcpy(mProIndices, _indices, sizeof(unsigned)*mProIndexCount);
	memcpy(mProData, _data, sizeof(RsV4T2)*mProDataCount);
	mPriFaceType = _type;

	// ------------------------------------------------
	// GL - VBO Init stuff from here
	// ------------------------------------------------
	glGenBuffers(1, &mPriDataId);
	glBindBuffer(GL_ARRAY_BUFFER, mPriDataId);

	glBufferData(GL_ARRAY_BUFFER, mProDataCount*sizeof(RsV4T2), mProData, GL_STATIC_DRAW);

	glGenBuffers(1, &mPriIndexId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIndexId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mProIndexCount*sizeof(unsigned), mProIndices, GL_STATIC_DRAW);


	// cleaning up - we have our data exclusively in GPU-Ram
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;

}

RsVboV4T2::~RsVboV4T2() {
	delete[] mProData;
	mProData = 0;
	delete[] mProIndices;
	mProIndices = 0;
}

unsigned RsVboV4T2::getIndexCount() const
{
	return mProIndexCount;
}

unsigned RsVboV4T2::getVertexCount() const
{
	return mProDataCount;
}

uint64_t RsVboV4T2::getComponentBytes() const
{
	return sizeof(RsV4T2);
}

void RsVboV4T2::draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glClientActiveTexture(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, mPriDataId);
	glVertexPointer(3, GL_FLOAT, sizeof(RsV4T2), bufferOffset(0));
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(RsV4T2), bufferOffset(sizeof(float)*4));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIndexId);
	glDrawElements(mPriFaceType, mProIndexCount, GL_UNSIGNED_INT, bufferOffset(0));

	glClientActiveTexture(GL_TEXTURE0_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}
