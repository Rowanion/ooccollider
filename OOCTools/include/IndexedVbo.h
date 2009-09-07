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

public:
	IndexedVbo(const unsigned* _idxData, unsigned _idxCount, const V4N4* _vertexData, unsigned _vertexCount, bool initiateOnline=true);
	IndexedVbo(fs::path path, uint64_t id, bool initiateOnline=true);
	virtual ~IndexedVbo();
	void managedDraw(bool dataNodeMode = false);
	void setOnline();
	void setOffline();
	inline const V4N4* getVertexData() const {return (const V4N4*)mPriVertexData;};
	inline const unsigned* getIndexData() const {return (const unsigned*)mPriIndexData;};
	inline unsigned getVertexCount() const {return mPriVertexCount;};
	inline unsigned getTriCount() const {return mPriVertexCount/3;};
	inline unsigned getIndexCount() const {return mPriIndexCount;};
	inline uint64_t getId() const {return mPriId;};

private:
	V4N4* mPriVertexData;
	unsigned mPriVertexCount;
	unsigned* mPriIndexData;
	unsigned mPriIndexCount;
	bool mPriIsOffline;
	GLuint mPriVertexId;
	GLuint mPriIdxId;
	bool mPriIsGpuOnly;
	uint64_t mPriId;
};

}

#endif /* INDEXEDVBO_H_ */
