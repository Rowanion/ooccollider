/**
 * @file	VboFactory.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	VboFactory class declaration.
 */

#include "VboFactory.h"

#include <iostream>
#include <cstdlib>

using namespace std;
namespace fs=boost::filesystem;

namespace ooctools {

VboFactory* VboFactory::mPriInstancePtr = 0;

VboFactory* VboFactory::getSingleton()
{
	if (mPriInstancePtr == 0)
	{
		mPriInstancePtr = new VboFactory();
	}
	return mPriInstancePtr;
}

VboFactory::VboFactory() :	mPriMem(0)
{
	mPriMem = new char[1024*1024*64];
	cerr << "sizeof iVBO: " << sizeof(IndexedVbo)<< endl;

	cerr << "sum: " << sizeof(V4N4*)+sizeof(unsigned)+sizeof(unsigned*)+sizeof(unsigned)+sizeof(bool)+(sizeof(GLuint)*2)+sizeof(bool)+sizeof(uint64_t)+sizeof(fs::ifstream*)+sizeof(unsigned) << endl;
	cerr << "sizeof bool " << sizeof(bool) << endl;

}

VboFactory::~VboFactory() {
	delete[] mPriMem;
}

IVbo* VboFactory::newVbo(boost::filesystem::ifstream* _inFile, unsigned _pos)
{
	//TODO beware, he, who treads lightly in those uncharted waters!
	char* memPos = mPriMem;

	unsigned tmpId, tmpIdxCount, tmpVertCount, tmpSize;

	_inFile->seekg(_pos, ios::beg);
	_inFile->read((char*)&tmpId, sizeof(uint64_t));
	_inFile->seekg(_pos+sizeof(uint64_t), ios::beg);
	_inFile->read((char*)&tmpIdxCount, sizeof(unsigned));
	_inFile->seekg(_pos+sizeof(uint64_t)+sizeof(unsigned), ios::beg);
	_inFile->read((char*)&tmpVertCount, sizeof(unsigned));
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2), ios::beg);
	tmpSize = sizeof(uint64_t) + sizeof(unsigned)*3 + sizeof(bool)*2 + sizeof(unsigned)*tmpIdxCount + sizeof(V4N4)*tmpVertCount;

	//TODO find fitting memory block, repack otherwise
	*((uint64_t*)memPos) = tmpId;
	*((unsigned*)(memPos+sizeof(uint64_t))) = tmpIdxCount;
	*((unsigned*)(memPos+sizeof(uint64_t) + sizeof(unsigned))) = tmpVertCount;
	*((unsigned*)(memPos+sizeof(uint64_t) + sizeof(unsigned)*2)) = tmpSize;

	((IVbo*)memPos)->debug();
	((IVbo*)memPos)->indexData();
	cerr << "mempos " << (uint64_t)memPos << endl;

	_inFile->read((char*)((IVbo*)memPos)->indexData(), sizeof(unsigned)*tmpIdxCount);
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*tmpIdxCount), ios::beg);

	_inFile->read((char*)((IVbo*)memPos)->vertexData(), sizeof(V4N4)*tmpVertCount);
//	_inFile->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount)), ios::beg);
//	mPriFPos += sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount));
//	cerr << "id: " << tmpId<< endl;
//	cerr << "idx: " << (((IVbo*)memPos)->indexData())[3] << ", " << (((IVbo*)memPos)->indexData())[4] << ", " << (((IVbo*)memPos)->indexData())[5] << endl;
//	cerr << "vert: " << (((IVbo*)memPos)->vertexData())[0].v[0] << ", " << (((IVbo*)memPos)->vertexData())[0].v[1] << ", " << (((IVbo*)memPos)->vertexData())[0].v[2] << endl;

	return 0;
}

void IVbo::debug()
{
	cerr << "iID " << mPriId << " - " << ((uint64_t)&mPriId) - ((uint64_t)this) << endl;
	cerr << "iIdxc " << mPriIndexCount << " - " << ((uint64_t)&mPriIndexCount) - ((uint64_t)this) << endl;
	cerr << "iVertc " << mPriVertexCount << " - " << ((uint64_t)&mPriVertexCount) - ((uint64_t)this) << endl;
	cerr << "iSize " << mPriByteSize << " - " << ((uint64_t)&mPriByteSize) - ((uint64_t)this) << endl;
	cerr << "iIdx " << indexData()[0] << " - " << ((uint64_t)indexData())-((uint64_t)this) << endl;
	cerr << "iVert " << vertexData()[0].v[0] << " - " << ((uint64_t)vertexData())-((uint64_t)this) << endl;
}

unsigned* IVbo::indexData()
{
	return (unsigned*)(this + sizeof(uint64_t) + (3*sizeof(unsigned)) + sizeof(char)*2);
}

V4N4* IVbo::vertexData()
{
	return (V4N4*)(this + sizeof(uint64_t) + (3*sizeof(unsigned)) + (mPriIndexCount*sizeof(unsigned)));
}

}
