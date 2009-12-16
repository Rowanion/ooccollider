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

#define MEM_BLOCK_SIZE 1024*1024*8

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
	mPriMem = new uint64_t[MEM_BLOCK_SIZE];
	mPriFreeMap.insert(Memory(MEM_BLOCK_SIZE*sizeof(uint64_t), mPriMem));
	cerr << "sizeof iVBO: " << sizeof(IVbo)<< endl;

	cerr << "first addr: " << (uint64_t)mPriMem << endl;
	cerr << "sum: " << sizeof(V4N4*)+sizeof(unsigned)+sizeof(unsigned*)+sizeof(unsigned)+sizeof(bool)+(sizeof(GLuint)*2)+sizeof(bool)+sizeof(uint64_t)+sizeof(fs::ifstream*)+sizeof(unsigned) << endl;

	cerr << "sizeof bool " << sizeof(bool) << endl;

}

VboFactory::~VboFactory() {
	delete[] mPriMem;
}

MemIt VboFactory::findFirstFit(const unsigned _byteSize)
{
	set<Memory>::iterator mIt = mPriFreeMap.begin();
	for (; mIt != mPriFreeMap.end(); ++mIt){
		if (mIt->size >= _byteSize){
			return mIt;
		}
	}
	return mPriFreeMap.end();
}

/**
 * @warning Beware, he, who treads lightly in those dangerous and uncharted waters!
 */
IVbo* VboFactory::newVbo(boost::filesystem::ifstream* _inFile, unsigned _pos)
{

	uint64_t tmpId;
	unsigned tmpIdxCount, tmpVertCount, tmpSize;

	_inFile->seekg(_pos, ios::beg);
	_inFile->read((char*)&tmpId, sizeof(uint64_t));
	_inFile->seekg(_pos+sizeof(uint64_t), ios::beg);
	_inFile->read((char*)&tmpIdxCount, sizeof(unsigned));
	_inFile->seekg(_pos+sizeof(uint64_t)+sizeof(unsigned), ios::beg);
	_inFile->read((char*)&tmpVertCount, sizeof(unsigned));
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2), ios::beg);
	tmpSize = sizeof(IVbo) + sizeof(unsigned)*tmpIdxCount + sizeof(V4N4)*tmpVertCount;

	//TODO find fitting memory block, repack otherwise
	// ------------------------
	char* memPos;
	MemIt it = findFirstFit(tmpSize);
	if (it == mPriFreeMap.end()){
		//TODO resize/decache
		cerr << "last size: " << mPriFreeMap.rbegin()->size << endl;
		cerr << "last addr: " << (uint64_t)mPriFreeMap.rbegin()->address << endl;
		return 0;
	}
	else { // found a matching piece. Now get new free ptr
		uint64_t* newAddr = ((uint64_t*)(((char*)it->address)+tmpSize));
		Memory m = Memory(it->size-tmpSize, newAddr);
		memPos = (char*)it->address;
		//TODO this needs to be verified. <underscore>Seriously</underscore>!
		*getNewRegistryPtr() = (IVbo*)memPos;
		cerr << "mapsize b4: " << mPriFreeMap.size() << ", " << mPriFreeMap.begin()->size << endl;
		mPriFreeMap.erase(it);
		mPriFreeMap.insert(m);
//		cerr << "currentPTR: " << (uint64_t)m.address << endl;
//		cerr << "oldPTR: " << (uint64_t)memPos << endl;
		cerr << "mapsize aft: " << mPriFreeMap.size() << ", " << mPriFreeMap.begin()->size << endl;
	}
	// ------------------------
	((IVbo*)memPos)->mPriId = tmpId;
	((IVbo*)memPos)->mPriIndexCount = tmpIdxCount;
	((IVbo*)memPos)->mPriVertexCount = tmpVertCount;
	((IVbo*)memPos)->mPriByteSize = tmpSize;

	_inFile->read((char*)((IVbo*)memPos)->indexData(), sizeof(unsigned)*tmpIdxCount);
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*tmpIdxCount), ios::beg);

	_inFile->read((char*)((IVbo*)memPos)->vertexData(), sizeof(V4N4)*tmpVertCount);
//	((IVbo*)memPos)->debug();
	return (IVbo*)memPos;
}

IVbo** VboFactory::getNewRegistryPtr()
{
	//TODO
	pair<set<IVbo**>::iterator,bool> result;
	result = mPriIVboRegistry.insert(new IVbo*());
	return (*result.first);
//	return 0;
}

IVbo** VboFactory::insertIntoRegistry(IVbo* _iVbo)
{
	//TODO
	pair<set<IVbo**>::iterator,bool> result;
	result = mPriIVboRegistry.insert(new IVbo*());
	*(*result.first) = _iVbo;
	return (*result.first);
}

void IVbo::debug()
{
	cerr << "iID " << mPriId << " - " << ((uint64_t)&mPriId) - ((uint64_t)this) << endl;
	cerr << "iIdxc " << mPriIndexCount << " - " << ((uint64_t)&mPriIndexCount) - ((uint64_t)this) << endl;
	cerr << "iVertc " << mPriVertexCount << " - " << ((uint64_t)&mPriVertexCount) - ((uint64_t)this) << endl;
	cerr << "iSize " << mPriByteSize << " - " << ((uint64_t)&mPriByteSize) - ((uint64_t)this) << endl;
	cerr << "iIdx " << " - " << (uint64_t)(indexData())-((uint64_t)this) << endl;
	cerr << "iVert " << " - " << ((uint64_t)vertexData())-((uint64_t)this) << endl;
	cerr << "indices " << indexData()[0] << ", " << indexData()[1] << ", " << indexData()[2] << endl;
	cerr << "vertices " << vertexData()[0].v[0] << ", " << vertexData()[0].v[1] << ", " << vertexData()[0].v[2] << endl;
}

unsigned* IVbo::indexData()
{
	return (unsigned*)(this+1);
}

V4N4* IVbo::vertexData()
{
	return (V4N4*) (((char*)(this+1)) + (mPriIndexCount*sizeof(unsigned)));
}

uint64_t IVbo::getId()
{
	return mPriId;
}

unsigned IVbo::getIndexCount()
{
	return mPriIndexCount;
}

unsigned IVbo::getVertexCount()
{
	return mPriVertexCount;
}

unsigned IVbo::getByteSize()
{
	return mPriByteSize;
}

Memory::Memory(unsigned _size, uint64_t* _addr) :
	size(_size), address(_addr)
{

}

bool Memory::operator<(const Memory& _rhs) const
{
	return (address < _rhs.address);
}

}
