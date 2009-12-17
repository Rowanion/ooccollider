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
#include <cstring>


#ifdef __X86_64__
#define MEM_BLOCK_SIZE 1024*1024*8
#else
#define MEM_BLOCK_SIZE 1024*1024*16
#endif

using namespace std;
using namespace ooctools;

namespace fs=boost::filesystem;

namespace oocframework {

bool compListAddressAsc (const oocformats::WrappedOcNode* _lhs, const oocformats::WrappedOcNode* _rhs)
{
	return (_lhs->iVbo < _rhs->iVbo);
}

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
	mPriEndOfSpace = mPriMem + MEM_BLOCK_SIZE;
	cerr << "sizeof iVBO: " << sizeof(IVbo)<< endl;

	cerr << "first addr: " << (mem_t)mPriMem << endl;
	cerr << "freespace: " << mPriFreeMap.begin()->size << endl;

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

	_inFile->read((char*)memPos->indexData(), sizeof(unsigned)*tmpIdxCount);
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*tmpIdxCount), ios::beg);

	_inFile->read((char*)memPos->vertexData(), sizeof(V4N4)*tmpVertCount);
	mPriIVboMap.insert(memPos);
//	((IVbo*)memPos)->debug();
//	return (IVbo*)memPos;
	return memPos;
}

//TODO concatenate adjacent free space
//TODO auf multiset umstellen!!
void VboFactory::freeVbo(IVbo* _iVbo)
{
	// find adjacent free space after this
	set<Memory>::iterator chkIt = mPriFreeMap.find(Memory(0, (mem_t*)((char*)_iVbo)+_iVbo->mPriByteSize));
	if (chkIt != mPriFreeMap.end()){ //found some
		//TODO find adjacent space before current
		mPriFreeMap.insert(Memory(_iVbo->mPriByteSize+chkIt->size, (mem_t*)_iVbo));
		mPriFreeMap.erase(chkIt);
	}
	else { // no free space after this one
		mPriFreeMap.insert(Memory(_iVbo->mPriByteSize, (mem_t*)_iVbo));
	}
	memset(_iVbo, 0, _iVbo->mPriByteSize);
}

void VboFactory::freeVbo(oocformats::WrappedOcNode* _wNode)
{
	mPriFreeMap.insert(Memory(_wNode->iVbo->mPriByteSize, (mem_t*)_wNode->iVbo));
	memset(_wNode->iVbo, 0, _wNode->iVbo->mPriByteSize);
	_wNode->iVbo = 0;
}

void VboFactory::defrag(list<oocformats::WrappedOcNode*>* _wNodeList)
{
	// fetch first free entry
	// find first vbo >= this space
	// move vbo into free space and advance pointer by
	_wNodeList->sort(compListAddressAsc);
	char* currentFreeAddress = (char*)mPriFreeMap.begin()->address;
	list<oocformats::WrappedOcNode*>::iterator wnIt = _wNodeList->begin();
	for (; wnIt != _wNodeList->end(); ++wnIt){
		if ((*wnIt)->iVbo >= (IVbo*)currentFreeAddress){
			(*wnIt)->iVbo = (IVbo*)memmove((char*)currentFreeAddress, (char*)(*wnIt)->iVbo, (*wnIt)->iVbo->mPriByteSize);
			currentFreeAddress+=(*wnIt)->iVbo->mPriByteSize;
		}
	}
	mPriFreeMap.clear();
	mPriFreeMap.insert(Memory((mem_t)mPriEndOfSpace-((mem_t)currentFreeAddress), (mem_t*)currentFreeAddress));
}

void VboFactory::debug()
{
	cerr << "FreeListSize: " << mPriFreeMap.size() << endl;
	unsigned newFree = 0;
	for (set<Memory>::iterator freeIt = mPriFreeMap.begin(); freeIt != mPriFreeMap.end(); ++freeIt){
		cerr << "freePoint: " << (mem_t)freeIt->address << ": " << freeIt->size << endl;
		newFree += freeIt->size;
	}
	cerr << "Free Sum: " << newFree << endl;
}

Memory::Memory(unsigned _size, mem_t* _addr) :
	size(_size), address(_addr)
{
}

bool Memory::operator<(const Memory& _rhs) const
{
	return (address < _rhs.address);
}

}
