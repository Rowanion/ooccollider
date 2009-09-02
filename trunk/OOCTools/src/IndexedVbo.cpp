/**
 * @file	IndexedVbo.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.06.2009
 * @brief 	IndexedVbo class declaration.
 */

#include "IndexedVbo.h"

#include <string>
#include <iostream>
#include <cstring>

#include "GlToolkit.h"

namespace fs = boost::filesystem;
using namespace std;
namespace ooctools {

IndexedVbo::IndexedVbo(const unsigned* _idxData, unsigned _idxCount, const V4N4* _vertexData, unsigned _vertexCount, bool initiateOnline) :
	 mPriVertexData(0), mPriVertexCount(_vertexCount),
	 mPriIndexData(0), mPriIndexCount(_idxCount), mPriIsOffline(false),
	 mPriVertexId(0), mPriIdxId(0), mPriIsGpuOnly(false)
{
	mPriIndexData = new unsigned[_idxCount];
	mPriVertexData = new V4N4[_vertexCount];
	memcpy(mPriIndexData, _idxData, _idxCount*sizeof(unsigned));
	memcpy(mPriVertexData, _vertexData, _vertexCount*sizeof(V4N4));

	if (initiateOnline){
		setOnline();
	}
	else {
		mPriIsOffline = true;
	}
}

IndexedVbo::IndexedVbo(fs::path path, bool initiateOnline) :
	 mPriVertexData(0), mPriVertexCount(0),
	 mPriIndexData(0), mPriIndexCount(0), mPriIsOffline(false),
	 mPriVertexId(0), mPriIdxId(0), mPriIsGpuOnly(false)
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

	if (initiateOnline)
		setOnline();
}

IndexedVbo::~IndexedVbo() {
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
IndexedVbo::managedDraw(bool dataNodeMode)
{
	if (dataNodeMode){
		glEnableClientState(GL_VERTEX_ARRAY);

		if (!mPriIsOffline){
			if (mPriVertexId!=0) {
				glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
				glVertexPointer(3, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
			}
			if (mPriIdxId!=0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			}

			glDrawElements(GL_TRIANGLES, mPriIndexCount, GL_UNSIGNED_INT, bufferOffset(0));
		}
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		//	glEnableClientState(GL_INDEX_ARRAY);

		if (!mPriIsOffline){
			if (mPriVertexId!=0) {
				glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
				glVertexPointer(4, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
				glNormalPointer(GL_BYTE, sizeof(V4N4), bufferOffset(4*sizeof(float)));
			}
			if (mPriIdxId!=0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			}

			glDrawElements(GL_TRIANGLES, mPriIndexCount, GL_UNSIGNED_INT, bufferOffset(0));
		}
	}
}


/**
 *  @brief Loads the VertexArray back to the GPU.
 *  @return   the number of bytes of graphic memory you are using.
 *
 *  This is only useful if the VBO was switches off earlier and, for caching reasons
 *  was not entirely deleted.
 */
void
IndexedVbo::setOnline()
{
//	if (isOffline && !isGpuOnly){

	if (!mPriIsGpuOnly){
		if (mPriVertexData != 0){
			// do stuff to normals
			if (mPriVertexId==0)
				glGenBuffers(1, &mPriVertexId);
			glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
			glBufferData(GL_ARRAY_BUFFER, mPriVertexCount*sizeof(V4N4), mPriVertexData, GL_STATIC_DRAW);
			glVertexPointer(4, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
			glNormalPointer(GL_BYTE, sizeof(V4N4), bufferOffset(0));
		}
		if (mPriIndexData!=0){
			// do stuff to indices
			if (mPriIdxId==0)
				glGenBuffers(1, &mPriIdxId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPriIndexCount*sizeof(unsigned), mPriIndexData, GL_STATIC_DRAW);
		}
		mPriIsOffline = false;
	}
}

/**
 *  @brief	Erases the gpu-part of this VBO.
 *  @return	nothing
 *
 *  The VertexArrays are not deleted for caching reasons. Doing this, prevents this
 *  VBO from being rendered and the occupied memory is freed again. Effect is revertable
 *  by calling setOnline() at a later time.
 *  This does not affect the CPU-Memory or the VertexArray instances.
 */
void
IndexedVbo::setOffline()
{
	if (!mPriIsOffline && !mPriIsGpuOnly){
		if (mPriVertexId!=0){
			// do stuff to normals
			glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
			glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		}
		if (mPriIdxId!=0){
			// do stuff to indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		}
		mPriIsOffline=true;
	}
}


} //ooctools
