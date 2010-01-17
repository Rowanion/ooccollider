/**
 * @file	VboFactory.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	VboFactory class declaration.
 */

#include "VboFactory.h"
#include "LowMemoryEvent.h"
#include "EventManager.h"

#include <iostream>
#include <cstdlib>
#include <cstring>


#ifdef __X86_64__
#define MEM_BLOCK_SIZE 1024*1024*128 // 1024 = 1gb
#else
#define MEM_BLOCK_SIZE 1024*1024*256 // 1024
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

VboFactory::VboFactory() :	mPriMem(0), mPriFreeMem(0), mPriUsedMem(0)
{
	mPriMem = new mem_t[MEM_BLOCK_SIZE];
	mPriFreeMap.insert(Memory(MEM_BLOCK_SIZE*sizeof(mem_t), mPriMem));
	mPriEndOfSpace = mPriMem + MEM_BLOCK_SIZE;
	//TODO maybe remove in final....
	memset(mPriMem, 0, MEM_BLOCK_SIZE*sizeof(mem_t));
	mPriFreeMem = MEM_BLOCK_SIZE*sizeof(mem_t);
	mPriUsedMem = 0;
	mPriFreeMemTheshold = mPriFreeMem/20; // 5 percent
	cerr << "sizeof iVBO: " << sizeof(IVbo)<< endl;

	cerr << "first addr: " << (mem_t)mPriMem << endl;
	cerr << "freespace: " << mPriFreeMap.begin()->size << endl;

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

	unsigned tmpIdxCount, tmpVertCount, tmpSize;

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
		if (m.size>0){
			mPriFreeMap.insert(m);
		}
	}
	// ------------------------

	mPriFreeMem -= tmpSize;
	mPriUsedMem += tmpSize;

	memPos->mPriIndexCount = tmpIdxCount;
	memPos->mPriVertexCount = tmpVertCount;
	memPos->mPriByteSize = tmpSize;
	memPos->mPriIdxId = 0;
	memPos->mPriVertexId = 0;
	memPos->mPriIsOffline = true;

	_inFile->read((char*)memPos->indexData(), sizeof(unsigned)*tmpIdxCount);
	_inFile->seekg(_pos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*tmpIdxCount), ios::beg);

	_inFile->read((char*)memPos->vertexData(), sizeof(V4N4)*tmpVertCount);
//	((IVbo*)memPos)->debug();
//	return (IVbo*)memPos;

	if (mPriFreeMem <= mPriFreeMemTheshold){
		LowMemoryEvent lme = LowMemoryEvent();
		EventManager::getSingleton()->fire(lme);
	}
	return memPos;
}

ooctools::IVbo* VboFactory::newVbo(unsigned _iCount, const unsigned* _iArray, unsigned _vCount, const ooctools::V4N4* _vArray)
{
//	cerr << "creating new vbo w " << _iCount << " indices and " << _vCount << " vertices." << endl;
	unsigned tmpSize;

	tmpSize = sizeof(IVbo) + sizeof(unsigned)*_iCount + sizeof(V4N4)*_vCount;

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
		if (m.size>0){
			mPriFreeMap.insert(m);
		}
	}

	mPriFreeMem -= tmpSize;
	mPriUsedMem += tmpSize;

	memPos->mPriIndexCount = _iCount;
	memPos->mPriVertexCount = _vCount;
	memPos->mPriByteSize = tmpSize;
	memPos->mPriIdxId = 0;
	memPos->mPriVertexId = 0;
	memPos->mPriIsOffline = true;

	// ------------------------
	memcpy(memPos->indexData(), _iArray, sizeof(unsigned)*_iCount);
	memcpy(memPos->vertexData(), _vArray, sizeof(V4N4)*_vCount);

//	((IVbo*)memPos)->debug();
//	return (IVbo*)memPos;
	if (mPriFreeMem <= mPriFreeMemTheshold){
		LowMemoryEvent lme = LowMemoryEvent();
		EventManager::getSingleton()->fire(lme);
	}
	return memPos;
}

void VboFactory::freeVbo(IVbo* _iVbo)
{
	setOffline(_iVbo);

	mPriFreeMem += _iVbo->mPriByteSize;
	mPriUsedMem -= _iVbo->mPriByteSize;

	if (_iVbo->mPriVertexId != 0){
		glDeleteBuffers(1, &_iVbo->mPriVertexId);
		_iVbo->mPriVertexId = 0;
	}
	if (_iVbo->mPriIdxId != 0){
		glDeleteBuffers(1, &_iVbo->mPriIdxId);
		_iVbo->mPriIdxId = 0;
	}

	Memory m = Memory(_iVbo->mPriByteSize, (mem_t*)_iVbo);
	set<Memory>::iterator freeBeforeIt;
	set<Memory>::iterator freeAfterIt;
	// find adjacent free space after this
	char* temp = (char*)_iVbo;
	temp += _iVbo->mPriByteSize;
	freeAfterIt = mPriFreeMap.find(Memory(0,(mem_t*)temp));
	if (freeAfterIt == mPriFreeMap.begin()){
		//no free space before, but after
		m.size+=freeAfterIt->size;
		mPriFreeMap.insert(m);
		mPriFreeMap.erase(freeAfterIt);
	}
	else if (freeAfterIt == mPriFreeMap.end()){
		// looking in front of the vbo.....
		//TODO the case that the freeList is empty is not covered here.
		// It would cause a segfault or worse because .begin would be empty.
		do{
			freeAfterIt--;
			temp = (char*)freeAfterIt->address;
			temp += freeAfterIt->size;
			if (temp == (char*)_iVbo){
				m.size+=freeAfterIt->size;
				m.address = freeAfterIt->address;
				mPriFreeMap.erase(freeAfterIt);
				break; // no need to look further
			}
		}while(temp > (char*)_iVbo && freeAfterIt != mPriFreeMap.begin());
		mPriFreeMap.insert(m);
	}
	else {
		freeBeforeIt = freeAfterIt;
		freeBeforeIt--;
		if (((char*)freeBeforeIt->address)+freeBeforeIt->size == (char*)_iVbo){
			m.size+=freeBeforeIt->size;
			m.address = freeBeforeIt->address;
			mPriFreeMap.erase(freeBeforeIt);
		}

		if (freeAfterIt != mPriFreeMap.end()){
			// free space before and after
			m.size+=freeAfterIt->size;
			mPriFreeMap.erase(freeAfterIt);
		}
		mPriFreeMap.insert(m);
	}

	// TODO maybe remove the memset, because the memory is already in freelist
	// and there are no pointers anymore pointing to this location.
	memset(_iVbo, 0, _iVbo->mPriByteSize);
	_iVbo = 0;
}

//TODO fit this method to above one
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
		if ((*wnIt)->iVbo != 0 && (*wnIt)->iVbo >= (IVbo*)currentFreeAddress){
			(*wnIt)->iVbo = (IVbo*)memmove((char*)currentFreeAddress, (char*)(*wnIt)->iVbo, (*wnIt)->iVbo->mPriByteSize);
			currentFreeAddress+=(*wnIt)->iVbo->mPriByteSize;
		}
	}
	mPriFreeMap.clear();
	mPriFreeMap.insert(Memory((mem_t)mPriEndOfSpace-((mem_t)currentFreeAddress), (mem_t*)currentFreeAddress));

	mPriFreeMem = mPriFreeMap.begin()->size;
	mPriUsedMem = (MEM_BLOCK_SIZE*sizeof(mem_t)) - mPriFreeMap.begin()->size;
}

void VboFactory::clear()
{
	mPriFreeMap.clear();
	mPriFreeMap.insert(Memory(MEM_BLOCK_SIZE*sizeof(mem_t), mPriMem));
	//TODO maybe remove in final.....see freeVBO
	memset(mPriMem, 0, MEM_BLOCK_SIZE*sizeof(mem_t));

	mPriFreeMem = MEM_BLOCK_SIZE*sizeof(mem_t);
	mPriUsedMem = 0;
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

void VboFactory::setOnline(ooctools::IVbo* _iVbo)
{
//	std::cerr << " -------------------------------------- setting vbo online...." << std::endl;
	if (_iVbo->mPriIsOffline){

		// do stuff to normals
		if (_iVbo->mPriVertexId==0){
//			std::cerr << "generating vertexBuffer4VBO...." << std::endl;
			glGenBuffers(1, &_iVbo->mPriVertexId);
		}
		else {
//			std::cerr << "somewhow we already have a vertexBuffer4VBO.... " << _iVbo->mPriVertexId << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, _iVbo->mPriVertexId);
		glBufferData(GL_ARRAY_BUFFER, _iVbo->mPriVertexCount*sizeof(V4N4), _iVbo->vertexData(), GL_STATIC_DRAW);
		glVertexPointer(4, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
		glNormalPointer(GL_BYTE, sizeof(V4N4), bufferOffset(0));
		// do stuff to indices
		if (_iVbo->mPriIdxId==0){
//			std::cerr << "generating indexBuffer4VBO...." << std::endl;
			glGenBuffers(1, &_iVbo->mPriIdxId);
		}
		else {
//			std::cerr << "somewhow we already have a indexBuffer4VBO.... " << _iVbo->mPriIdxId << std::endl;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iVbo->mPriIdxId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _iVbo->mPriIndexCount*sizeof(unsigned), _iVbo->indexData(), GL_STATIC_DRAW);
		_iVbo->mPriIsOffline = false;
	}

}

void VboFactory::setOffline(ooctools::IVbo* _iVbo)
{
	if (!_iVbo->mPriIsOffline){
		if (_iVbo->mPriVertexId!=0){
			// do stuff to normals
			glBindBuffer(GL_ARRAY_BUFFER, _iVbo->mPriVertexId);
			glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		}
		if (_iVbo->mPriIdxId!=0){
			// do stuff to indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iVbo->mPriIdxId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		}
		_iVbo->mPriIsOffline=true;
	}

}

void
VboFactory::draw(ooctools::IVbo* _iVbo, bool _dataNodeMode)
{
	if (_dataNodeMode){
		glEnableClientState(GL_VERTEX_ARRAY);

		if (!_iVbo->mPriIsOffline){
			if (_iVbo->mPriVertexId!=0) {
				glBindBuffer(GL_ARRAY_BUFFER, _iVbo->mPriVertexId);
				glVertexPointer(3, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
			}
			if (_iVbo->mPriIdxId!=0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iVbo->mPriIdxId);
			}

			glDrawElements(GL_TRIANGLES, _iVbo->mPriIndexCount, GL_UNSIGNED_INT, bufferOffset(0));
		}
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		if (!_iVbo->mPriIsOffline){
			if (_iVbo->mPriVertexId!=0) {
				glBindBuffer(GL_ARRAY_BUFFER, _iVbo->mPriVertexId);
				glVertexPointer(4, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
				glNormalPointer(GL_BYTE, sizeof(V4N4), bufferOffset(4*sizeof(float)));
			}
			if (_iVbo->mPriIdxId!=0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iVbo->mPriIdxId);
			}

			glDrawElements(GL_TRIANGLES, _iVbo->mPriIndexCount, GL_UNSIGNED_INT, bufferOffset(0));
		}
	}
}

void VboFactory::drawAlt(ooctools::IVbo* _iVbo)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(V4N4), _iVbo->vertexData());
	glDrawElements(GL_TRIANGLES, _iVbo->mPriIndexCount, GL_UNSIGNED_INT, _iVbo->indexData());
	glDisableClientState(GL_VERTEX_ARRAY);
}

unsigned VboFactory::getFreeMem()
{
	return mPriFreeMem;
}

unsigned VboFactory::getUsedMem()
{
	return mPriUsedMem;
}

unsigned VboFactory::getFreeBlocks()
{
	return mPriFreeMap.size();
}

unsigned VboFactory::getLargestFreeBlock()
{
	unsigned count = 0;
	std::set<Memory>::iterator memIt = mPriFreeMap.begin();
	for (; memIt != mPriFreeMap.end(); ++memIt){
		if (memIt->size > count){
			count = memIt->size;
		}
	}
	return count;
}

unsigned VboFactory::getSmallestFreeBlock()
{
	unsigned count = MEM_BLOCK_SIZE*sizeof(mem_t);
	std::set<Memory>::iterator memIt = mPriFreeMap.begin();
	for (; memIt != mPriFreeMap.end(); ++memIt){
		if (memIt->size < count){
			count = memIt->size;
		}
	}
	return count;
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
