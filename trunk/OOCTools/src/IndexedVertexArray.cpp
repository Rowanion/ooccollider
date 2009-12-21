/**
 * @file	IndexedVertexArray.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.06.2009
 * @brief 	IndexedVertexArray class declaration.
 */

#include "IndexedVertexArray.h"

#include <string>
#include <iostream>
#include <cstring>

#include "GlToolkit.h"

namespace fs = boost::filesystem;
using namespace std;
namespace ooctools {

IndexedVertexArray::IndexedVertexArray(const unsigned* _idxData, unsigned _idxCount, const V4N4* _vertexData, unsigned _vertexCount) :
	 mPriVertexData(0), mPriVertexCount(_vertexCount),
	 mPriIndexData(0), mPriIndexCount(_idxCount),
	 mPriVertexId(0), mPriIdxId(0), mPriIsGpuOnly(false)
{
	mPriIndexData = new unsigned[_idxCount];
	mPriVertexData = new V4N4[_vertexCount];
	memcpy(mPriIndexData, _idxData, _idxCount*sizeof(unsigned));
	memcpy(mPriVertexData, _vertexData, _vertexCount*sizeof(V4N4));
}

IndexedVertexArray::IndexedVertexArray(fs::path path, uint64_t id) :
	 mPriVertexData(0), mPriVertexCount(0),
	 mPriIndexData(0), mPriIndexCount(0),
	 mPriVertexId(0), mPriIdxId(0), mPriIsGpuOnly(false), mPriId(id)
{
	fs::ifstream dataFile;
	fs::ifstream idxFile;
	if (path.extension() == ".idx"){
		idxFile.open(path, ios::binary);
		dataFile.open(path.parent_path() / (path.stem()+".data"), ios::binary);
	}
	else if (path.extension() == ".data"){
		dataFile.open(path, ios::binary);
		idxFile.open(path.parent_path() / (path.stem()+".idx"), ios::binary);
	}
	idxFile.seekg(0, ios::beg);
	idxFile.read((char*)&mPriIndexCount, sizeof(unsigned));
	idxFile.seekg(4, ios::beg);
	dataFile.seekg(0, ios::beg);
	dataFile.read((char*)&mPriVertexCount, sizeof(unsigned));
	dataFile.seekg(4, ios::beg);

	mPriIndexData = new unsigned[mPriIndexCount];
	mPriVertexData = new V4N4[mPriVertexCount];

	for(unsigned i=0; i< mPriIndexCount; ++i){
		idxFile.read((char*)&mPriIndexData[i], sizeof(unsigned));
	}
	idxFile.close();

	for(unsigned i=0; i< mPriVertexCount; ++i){
		dataFile.read((char*)&mPriVertexData[i], sizeof(V4N4));
	}
	dataFile.close();

//	cout << "Loaded " << mPriVertexCount << " vertices used in " << mPriIndexCount/3 << " triangles."<< endl;
//	cout << "first vertex after loading: " << endl;
//	cout << mPriVertexData[0].v[0] << endl;
//	cout << mPriVertexData[0].v[1] << endl;
//	cout << mPriVertexData[0].v[2] << endl;
//	cout << mPriVertexData[0].v[3] << endl;

//	if (mPriVertexCount < 3){
//		cerr << "loaded less than a triangle!!!" << endl;
//		cerr << "Error occured at " << path << endl;
//		exit(0);
//	}

}

IndexedVertexArray::IndexedVertexArray(fs::ifstream* _iStream, unsigned _pos) :
	 mPriVertexData(0), mPriVertexCount(0),
	 mPriIndexData(0), mPriIndexCount(0),
	 mPriVertexId(0), mPriIdxId(0), mPriIsGpuOnly(false), mPriId(0), mPriFPos(_pos)

{
	mPriIStream = _iStream;
	mPriIStream->seekg(mPriFPos, ios::beg);
	mPriIStream->read((char*)&mPriId, sizeof(uint64_t));
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t), ios::beg);
	mPriIStream->read((char*)&mPriIndexCount, sizeof(unsigned));
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+sizeof(unsigned), ios::beg);
	mPriIStream->read((char*)&mPriVertexCount, sizeof(unsigned));
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2), ios::beg);


	mPriIndexData = new unsigned[mPriIndexCount];
	mPriVertexData = new V4N4[mPriVertexCount];

	mPriIStream->read((char*)mPriIndexData, sizeof(unsigned)*mPriIndexCount);
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount), ios::beg);

	mPriIStream->read((char*)mPriVertexData, sizeof(V4N4)*mPriVertexCount);
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount)), ios::beg);
	mPriFPos += sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount));

//	cerr << "ID: " << mPriId << endl;
//	cerr << "indexcount: " << mPriIndexCount << endl;
//	cerr << "vertexcount: " << mPriVertexCount << endl;
//	for (unsigned i =0; i< mPriIndexCount; ++i){
//		cerr << mPriIndexData[i] << endl;
//	}
//	exit(0);
}

IndexedVertexArray::IndexedVertexArray(ooctools::Location _loc) :
	 mPriVertexData(0), mPriVertexCount(0),
	 mPriIndexData(0), mPriIndexCount(0),
	 mPriVertexId(0), mPriIdxId(0), mPriIsGpuOnly(false), mPriId(0), mPriFPos(_loc.position)

{

	fs::ifstream inFile;
	inFile.open(_loc.path, ios::binary);
	inFile.seekg(mPriFPos, ios::beg);
	inFile.read((char*)&mPriId, sizeof(uint64_t));
	inFile.seekg(mPriFPos+sizeof(uint64_t), ios::beg);
	inFile.read((char*)&mPriIndexCount, sizeof(unsigned));
	inFile.seekg(mPriFPos+sizeof(uint64_t)+sizeof(unsigned), ios::beg);
	inFile.read((char*)&mPriVertexCount, sizeof(unsigned));
	inFile.seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2), ios::beg);


	mPriIndexData = new unsigned[mPriIndexCount];
	mPriVertexData = new V4N4[mPriVertexCount];

	inFile.read((char*)mPriIndexData, sizeof(unsigned)*mPriIndexCount);
	inFile.seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount), ios::beg);

	inFile.read((char*)mPriVertexData, sizeof(V4N4)*mPriVertexCount);
	inFile.seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount)), ios::beg);
	mPriFPos += sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount));

//	cerr << "ID: " << mPriId << endl;
//	cerr << "indexcount: " << mPriIndexCount << endl;
//	cerr << "vertexcount: " << mPriVertexCount << endl;
//	for (unsigned i =0; i< mPriIndexCount; ++i){
//		cerr << mPriIndexData[i] << endl;
//	}
//	exit(0);
	inFile.close();
}

IndexedVertexArray::~IndexedVertexArray() {
	delete[] mPriVertexData;
	mPriVertexData = 0;
	delete[] mPriIndexData;
	mPriIndexData = 0;
	if (mPriVertexId != 0){
		glDeleteBuffers(1, &mPriVertexId);
		mPriVertexId = 0;
	}
	if (mPriIdxId != 0){
		glDeleteBuffers(1, &mPriIdxId);
		mPriIdxId = 0;
	}
}

void
IndexedVertexArray::managedDraw()
{
	glEnableClientState(GL_VERTEX_ARRAY_EXT);
	glVertexPointerEXT(3, GL_FLOAT, sizeof(V4N4), mPriVertexData);
	glDrawElements(GL_TRIANGLES, mPriIndexCount, GL_UNSIGNED_INT, mPriIndexData);
	glDisableClientState(GL_VERTEX_ARRAY_EXT);
}

void
IndexedVertexArray::next()
{
	delete[] mPriIndexData;
	mPriIndexData = 0;
	delete[] mPriVertexData;
	mPriVertexData = 0;

	mPriIStream->read((char*)&mPriId, sizeof(uint64_t));
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t), ios::beg);
	mPriIStream->read((char*)&mPriIndexCount, sizeof(unsigned));
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+sizeof(unsigned), ios::beg);
	mPriIStream->read((char*)&mPriVertexCount, sizeof(unsigned));
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2), ios::beg);

	mPriIndexData = new unsigned[mPriIndexCount];
	mPriVertexData = new V4N4[mPriVertexCount];

	mPriIStream->read((char*)mPriIndexData, sizeof(unsigned)*mPriIndexCount);
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount), ios::beg);

	mPriIStream->read((char*)mPriVertexData, sizeof(V4N4)*mPriVertexCount);
	mPriIStream->seekg(mPriFPos+sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount)), ios::beg);
	mPriFPos += sizeof(uint64_t)+(sizeof(unsigned)*2) + (sizeof(unsigned)*mPriIndexCount + (sizeof(V4N4)*mPriVertexCount));

}

} //ooctools
