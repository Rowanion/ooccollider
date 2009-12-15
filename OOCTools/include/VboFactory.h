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

namespace ooctools {
class VboFactory;

struct IVbo{
	friend class VboFactory;
	static uint64_t getAddi();
	void debug();
	unsigned* indexData();
	V4N4* vertexData();
private:
	uint64_t mPriId;
	unsigned mPriIndexCount;
	unsigned mPriVertexCount;
	unsigned mPriByteSize;
	char mPriIsOffline;
	char mPriFree;
};

class VboFactory {
public:
	virtual ~VboFactory();
	IVbo* newVbo(boost::filesystem::ifstream* _inFile, unsigned _pos);

	static VboFactory *getSingleton();

private:
	VboFactory();
	static VboFactory* mPriInstancePtr;
	char* mPriMem;
};

}

#endif /* VBOFACTORY_H_ */
