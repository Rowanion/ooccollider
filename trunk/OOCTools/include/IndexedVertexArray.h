/**
 * @file	IndexedVertexArray.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.06.2009
 * @brief 	IndexedVertexArray class declaration.
 */

#ifndef INDEXEDVERTEXARRAY_H_
#define INDEXEDVERTEXARRAY_H_

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "GlToolkit.h"
#include "StructDefs.h"

namespace fs = boost::filesystem;
namespace ooctools {


class IndexedVertexArray {

public:
	IndexedVertexArray(const unsigned* _idxData, unsigned _idxCount, const V4N4* _vertexData, unsigned _vertexCount);
	IndexedVertexArray(fs::path path, uint64_t id);
	IndexedVertexArray(fs::ifstream* _iStream, unsigned _pos = 0);
	IndexedVertexArray(const char* _data);
	IndexedVertexArray(ooctools::Location _loc);
	virtual ~IndexedVertexArray();

	/**
	 * @note @Tim: Definition of stride: the byte-distance between two entries.
	 * array := v1,v2,v3,n1,n2,3 => stride for V3 vertices is sizeof(v)*3 + sizeof(n)*3
	 */
	void managedDraw();
	const V4N4* getVertexData() const {return (const V4N4*)mPriVertexData;};
	const unsigned* getIndexData() const {return (const unsigned*)mPriIndexData;};
	unsigned getVertexCount() const {return mPriVertexCount;};
	unsigned getTriCount() const {return mPriIndexCount/3;};
	unsigned getIndexCount() const {return mPriIndexCount;};
	uint64_t getId() const {return mPriId;};
	void next();

private:
	V4N4* mPriVertexData;
	unsigned mPriVertexCount;
	unsigned* mPriIndexData;
	unsigned mPriIndexCount;
	GLuint mPriVertexId;
	GLuint mPriIdxId;
	bool mPriIsGpuOnly;
	uint64_t mPriId;
	fs::ifstream* mPriIStream;
	unsigned mPriFPos;
};

}

#endif /* INDEXEDVERTEXARRAY_H_ */
