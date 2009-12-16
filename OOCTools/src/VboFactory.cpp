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

#ifdef __X86_64__
#define MEM_BLOCK_SIZE 1024*1024*8
#else
#define MEM_BLOCK_SIZE 1024*1024*16
#endif

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
	mPriMem = new mem_t[MEM_BLOCK_SIZE];
	mPriFreeMap.insert(Memory(MEM_BLOCK_SIZE*sizeof(mem_t), mPriMem));
	cerr << "sizeof iVBO: " << sizeof(IVbo)<< endl;

	cerr << "first addr: " << (mem_t)mPriMem << endl;
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
 * @warning Beware, he, who treads lightly in those uncharted and freakishly dangerous waters!
 */
IVbo** VboFactory::newVbo(boost::filesystem::ifstream* _inFile, unsigned _pos)
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
	IVbo* memPos;
	MemIt it = findFirstFit(tmpSize);
	if (it == mPriFreeMap.end()){
		//TODO resize/decache
		cerr << "last size: " << mPriFreeMap.rbegin()->size << endl;
		cerr << "last addr: " << (mem_t)mPriFreeMap.rbegin()->address << endl;
		return 0;
	}
	else { // found a matching chunk norris. Now get new free ptr
		mem_t* newAddr = ((mem_t*)(((char*)it->address)+tmpSize));
		Memory m = Memory(it->size-tmpSize, newAddr);
		memPos = (IVbo*)it->address;
		mPriFreeMap.erase(it);
		mPriFreeMap.insert(m);
	}
	// ------------------------

	memPos->mPriId = tmpId;
	memPos->mPriIndexCount = tmpIdxCount;
	memPos->mPriVertexCount = tmpVertCount;
	memPos->mPriByteSize = tmpSize;
	memPos->mPriRegistryPtr = insertIntoRegistry(memPos);
	IVbo** bla = memPos->mPriRegistryPtr;
	cerr << "xPtr Addr: " << (mem_t)memPos << " vs " << (mem_t)(*(memPos->mPriRegistryPtr)) << endl;
	cerr << "xPtr Addr: " << (mem_t)bla << " vs " << (mem_t)(memPos->mPriRegistryPtr) << endl;
	cerr << "xPtr Addr: " << (mem_t)memPos << " vs " << (mem_t)(*(memPos->mPriRegistryPtr)) << endl;
	cerr << "xPtr Addr: " << (mem_t)(*bla) << " vs " << (mem_t)(*memPos->mPriRegistryPtr) << endl;

	_inFile->read((char*)memPos->indexData(), sizeof(unsigned)*tmpIdxCount);
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*tmpIdxCount), ios::beg);

	_inFile->read((char*)memPos->vertexData(), sizeof(V4N4)*tmpVertCount);
	mPriIVboMap.insert(memPos);
//	((IVbo*)memPos)->debug();
//	return (IVbo*)memPos;
	return memPos->mPriRegistryPtr;
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
	pair<set<IVboWrap>::iterator, bool> result;
	result = mPriIVboRegistry.insert(IVboWrap(&_iVbo));
//	*(*result.first) = _iVbo;
	if (result.second){
		return result.first->iVbo;
	}
	else {
		return 0;
	}

}

void IVbo::debug()
{
	cerr << "iID " << mPriId << " - " << ((mem_t)&mPriId) - ((mem_t)this) << endl;
	cerr << "iIdxc " << mPriIndexCount << " - " << ((mem_t)&mPriIndexCount) - ((mem_t)this) << endl;
	cerr << "iVertc " << mPriVertexCount << " - " << ((uint64_t)&mPriVertexCount) - ((mem_t)this) << endl;
	cerr << "iSize " << mPriByteSize << " - " << ((mem_t)&mPriByteSize) - ((mem_t)this) << endl;
	cerr << "iIdx " << " - " << (mem_t)(indexData())-((mem_t)this) << endl;
	cerr << "iVert " << " - " << ((mem_t)vertexData())-((mem_t)this) << endl;
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

Memory::Memory(unsigned _size, mem_t* _addr) :
	size(_size), address(_addr)
{
}

bool Memory::operator<(const Memory& _rhs) const
{
	return (address < _rhs.address);
}

IVboWrap::IVboWrap(IVbo** _iVbo) :
	iVbo(_iVbo)
{
}

bool IVboWrap::operator<(const IVboWrap& _rhs) const
{
	return (iVbo < _rhs.iVbo);
}

}
