/**
 * @file	VboFactory.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	VboFactory class declaration.
 */

#ifndef VBOFACTORY_H_
#define VBOFACTORY_H_

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <set>
#include <list>

#include "StructDefs.h"
#include "LooseRenderOctree.h"

// make sure to use the correct pointer-width

namespace oocframework {

#ifdef __X86_64__
typedef uint64_t mem_t;
#else
typedef unsigned mem_t;
#endif

class VboFactory;

struct Memory{
	Memory(unsigned _size, mem_t* _addr);
	unsigned size;
	mem_t* address;
	bool operator<(const Memory& _rhs) const;
};

typedef std::set<Memory>::iterator MemIt;
typedef std::set<Memory>::const_iterator MemCIt;
typedef std::set<Memory>::reverse_iterator MemRIt;


class VboFactory {
public:
	virtual ~VboFactory();
	ooctools::IVbo* newVbo(boost::filesystem::ifstream* _inFile, unsigned _pos);
	ooctools::IVbo* newVbo(unsigned _iCount, const unsigned* _iArray, unsigned _vCount, const ooctools::V4N4* _vArray);
	//new IndexedVbo(ve.getIndexArray(i), ve.getIndexCount(i), ve.getVertexArray(i), ve.getVertexCount(i), false);
	MemIt findFirstFit(const unsigned _byteSize);

	static VboFactory* getSingleton();
	void defrag(std::list<oocformats::WrappedOcNode*>* _wNodeList);
	void freeVbo(ooctools::IVbo* _iVbo);
	void freeVbo(oocformats::WrappedOcNode* _wNode);
	void clear();
	void debug();
	void setOnline(ooctools::IVbo* _iVbo);
	void setOffline(ooctools::IVbo* _iVbo);
	void draw(ooctools::IVbo* _iVbo, bool _dataNodeMode = false);
	void drawAlt(ooctools::IVbo* _iVbo);
	unsigned getFreeMem();
	unsigned getUsedMem();
	unsigned getTotalMem();
	unsigned getFreeBlocks();
	unsigned getLargestFreeBlock();
	unsigned getSmallestFreeBlock();

private:
	VboFactory();
	static VboFactory* mPriInstancePtr;
	mem_t* mPriMem;
	mem_t* mPriEndOfSpace;
	std::set<Memory> mPriFreeMap;
	unsigned mPriFreeMem;
	unsigned mPriUsedMem;
	unsigned mPriFreeMemTheshold;
};

}

#endif /* VBOFACTORY_H_ */
