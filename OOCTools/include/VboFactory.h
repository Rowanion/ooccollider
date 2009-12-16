/**
 * @file	VboFactory.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 15.12.2009
 * @brief 	VboFactory class declaration.
 */

#ifndef VBOFACTORY_H_
#define VBOFACTORY_H_

#include "IndexedVbo.h"

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <set>

namespace ooctools {
class VboFactory;

struct Memory{
	Memory(unsigned _size, uint64_t* _addr);
	unsigned size;
	uint64_t* address;
	bool operator<(const Memory& _rhs) const;
};

typedef std::set<Memory>::iterator MemIt;
typedef std::set<Memory>::const_iterator MemCIt;
typedef std::set<Memory>::reverse_iterator MemRIt;

struct IVbo{
//	friend class VboFactory;
	static uint64_t getAddi();
	void debug();
	unsigned* indexData();
	V4N4* vertexData();
	uint64_t getId();
	unsigned getIndexCount();
	unsigned getVertexCount();
	unsigned getByteSize();

//private:
	uint64_t mPriId;
	unsigned mPriIndexCount;
	unsigned mPriVertexCount;
	IVbo** mPriRegistryPtr;
	unsigned mPriByteSize;
	char mPriIsOffline;
	char mPriFree;
};

class VboFactory {
public:
	virtual ~VboFactory();
	IVbo* newVbo(boost::filesystem::ifstream* _inFile, unsigned _pos);
	MemIt findFirstFit(const unsigned _byteSize);

	static VboFactory* getSingleton();
	IVbo** getNewRegistryPtr();
	IVbo** insertIntoRegistry(IVbo* _iVbo);

private:
	VboFactory();
	static VboFactory* mPriInstancePtr;
	uint64_t* mPriMem;
	std::set<Memory> mPriFreeMap;
	std::set<IVbo*> mPriIVboMap;

	std::set<IVbo**> mPriIVboRegistry;

};

}

#endif /* VBOFACTORY_H_ */
