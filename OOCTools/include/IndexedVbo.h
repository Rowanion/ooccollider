/**
 * @file	IndexedVbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.06.2009
 * @brief 	IndexedVbo class declaration.
 */

#ifndef INDEXEDVBO_H_
#define INDEXEDVBO_H_

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "GlToolkit.h"
#include "StructDefs.h"

namespace fs = boost::filesystem;
namespace ooctools {


class IndexedVbo {

friend class VboManager;
friend class VboFactory;

public:
	IndexedVbo(const unsigned* _idxData, unsigned _idxCount, const V4N4* _vertexData, unsigned _vertexCount, bool initiateOnline=true);
	IndexedVbo(fs::path path, uint64_t id, bool initiateOnline=true);
	IndexedVbo(fs::ifstream* _iStream, unsigned _pos = 0, bool initiateOnline=true);
	IndexedVbo(ooctools::Location _loc, bool initiateOnline = true);
	IndexedVbo(char* _data);
	virtual ~IndexedVbo();
	void managedDraw(bool dataNodeMode = false);
	void setOnline();
	void setOffline();
	const V4N4* getVertexData() const {return (const V4N4*)mPriVertexData;};
	const unsigned* getIndexData() const {return (const unsigned*)mPriIndexData;};
	unsigned getVertexCount() const {return mPriVertexCount;};
	unsigned getTriCount() const {return mPriIndexCount/3;};
	unsigned getIndexCount() const {return mPriIndexCount;};
	uint64_t getId() const {return mPriId;};
	void next();

private:
	V4N4* mPriVertexData;
	unsigned* mPriIndexData;
	fs::ifstream* mPriIStream;
	uint64_t mPriId;
	unsigned mPriVertexCount;
	unsigned mPriIndexCount;
	GLuint mPriVertexId;
	GLuint mPriIdxId;
	unsigned mPriFPos;
	bool mPriIsGpuOnly;
	bool mPriIsOffline;
};

}

#endif /* INDEXEDVBO_H_ */
