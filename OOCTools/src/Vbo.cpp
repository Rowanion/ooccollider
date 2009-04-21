/**
 * @file	Vbo.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 08.02.2009
 * @brief   Class definition of Vbo
 */

#include "Vbo.h"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

#include "declarations.h"
#include "Face.h"
#include "BoundingBox.h"
#include "V3ub.h"
#include "V4f.h"
#include "StructDefs.h"

using namespace std;
namespace ooctools {

typedef vector<GLuint>::const_iterator GLuint_v_It;

Vbo::Vbo(VertexArray<unsigned int>* _ia, VertexArray<char>* _na,
		VertexArray<float>* _va) :
	mPriVId(0), mPriNId(0), mPriIdxId(0), mPriUseIndices(false), _vboUsageMode(
			GL_STATIC_DRAW), mPriNormals(0), mPriVertices(0), mPriIndices(0),
			mPriCurrentColorF(0), mPriCurrentColorB(0), isOffline(false),
			isGpuOnly(false)
{

	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &mPriMaxVertices);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &mPriMaxIndices);
	_upperVertexBound = 500;
//	mPriUseGlColor = false;
//	mPriCurrentColorB = new unsigned char[3];
//	mPriCurrentColorB[0] = defaultColorB.getX();
//	mPriCurrentColorB[1] = defaultColorB.getY();
//	mPriCurrentColorB[2] = defaultColorB.getZ();
//	mPriCurrentColorF = new float[3];
//	mPriCurrentColorF[0] = defaultColorF.getX();
//	mPriCurrentColorF[1] = defaultColorF.getY();
//	mPriCurrentColorF[2] = defaultColorF.getZ();
	if (_ia != 0)
		setIndices(_ia);
	if (_na != 0)
		setNData(_na);
	if (_va != 0)
		setVData(_va);
}

Vbo::Vbo(const Vbo& _vbo) :
	mPriVId(0), mPriNId(0), mPriIdxId(0), mPriUseIndices(false), _vboUsageMode(
			GL_STATIC_DRAW), mPriNormals(0), mPriVertices(0), mPriIndices(0),
			mPriCurrentColorF(0), mPriCurrentColorB(0), isOffline(false),
			isGpuOnly(false)
{
	if (_vbo.hasIndices()){
		setIndices(new VertexArray<unsigned int>(*_vbo.getIndices()), false);
		mPriUseIndices = true;
	}
	if (_vbo.hasVertices()){
		setVData(new VertexArray<float>(*_vbo.getVData()), false);
	}
	if (_vbo.hasNormals()){
		setNData(new VertexArray<char>(*_vbo.getNData()), false);
	}
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &mPriMaxVertices);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &mPriMaxIndices);
	_upperVertexBound = 500;
}

Vbo::~Vbo()
{

	if (!isOffline && !isGpuOnly) setOffline();

	if (mPriVId!=0){
		glDeleteBuffers(1, &mPriVId);
		mPriVId=0;
	}
	if (mPriNId!=0){
		glDeleteBuffers(1, &mPriNId);
		mPriNId=0;
	}
	if (mPriIdxId!=0){
		glDeleteBuffers(1, &mPriIdxId);
		mPriIdxId=0;
	}
	if (!isGpuOnly){
		if (mPriVertices!=0){
			delete mPriVertices;
			mPriVertices = 0;
		}
		if (mPriNormals!=0){
			delete mPriNormals;
			mPriNormals = 0;
		}
		if (mPriIndices!=0){
			delete mPriIndices;
			mPriIndices = 0;
		}
	}
//	if (mPriCurrentColorF!=0){
//		delete[] mPriCurrentColorF;
//		mPriCurrentColorF = 0;
//	}
//	if (mPriCurrentColorB!=0){
//		delete[] mPriCurrentColorB;
//		mPriCurrentColorB = 0;
//	}
}

void
Vbo::draw(int vertexCount)
{
	if (!isOffline) {
		GLuint_v_It it;

//		if (mPriUseGlColor){
//			if (mPriCurrentColorB!=0)
//				glColor3ubv(mPriCurrentColorB);
//			else
//				glColor3ubv(defaultColorB.getData());
//		}
//		else {
//			if (mPriCurrentColorF!=0)
//				cgSetParameter3fv(mPriDiffuseParameter, mPriCurrentColorF);
//			else
//				cgSetParameter3fv(mPriDiffuseParameter, defaultColorF.getData());
//		}
		drawSingle(vertexCount);
	}
}

void
Vbo::setCgP(CGparameter cgp)
{
	mPriDiffuseParameter = cgp;
}

void
Vbo::drawSingle(int vertexCount)
{
	if (!isOffline){
		if (mPriNId!=0) {
			glBindBuffer(GL_ARRAY_BUFFER, mPriNId);
			glNormalPointer(GL_BYTE, mPriNormals->stride, bufferOffset(0));
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		if (mPriVId!=0) {
			glBindBuffer(GL_ARRAY_BUFFER, mPriVId);
			glVertexPointer(mPriVertices->nComponents, GL_FLOAT, mPriVertices->stride, bufferOffset(0));
			glEnableClientState(GL_VERTEX_ARRAY);
		}
		if (mPriIdxId!=0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			glEnableClientState(GL_INDEX_ARRAY);
		}


		if (mPriIdxId!=0)
			glDrawElements(GL_TRIANGLES, mPriVertices->size, GL_UNSIGNED_BYTE, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, mPriVertices->size);



		if (mPriNId!=0){
			glDisableClientState(GL_COLOR_ARRAY);
		}
		if (mPriVId!=0) {
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if (mPriIdxId!=0) {
			glDisableClientState(GL_INDEX_ARRAY);
		}
	}
}

/**
 *  @brief Sets the data for Vertices in the VBO
 *  @param  va  A pointer to a filled VertxArray instance.
 *  @return   Nothing.
 *
 *  This actually sets the vertex data of the VBO. Without this you cannot draw anything!
 */
void
Vbo::setVData(VertexArray<float>* va, bool useGl)
{
	mPriVertices = va;
	if (useGl) {
		if (mPriVId == 0)
			glGenBuffers(1, &mPriVId);

		glBindBuffer(GL_ARRAY_BUFFER, mPriVId);
		glBufferData(GL_ARRAY_BUFFER, va->nComponents * sizeof(float)
				* va->size, va->mData, _vboUsageMode);
		glVertexPointer(va->nComponents, GL_FLOAT, va->stride, bufferOffset(0));
	}
}

//void VBO::setIndices(void *idx, int idxSize){
//	if (mPriIdxId==0) glGenBuffers(1, &mPriIdxId);
//	_idxSize = idxSize;
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _idxSize, idx, _vboUsageMode);
//}
void
Vbo::setIndices(VertexArray<unsigned int>* _ia, bool useGl)
{
	mPriIndices = _ia;
	if (useGl) {
		if (mPriIdxId == 0)
			glGenBuffers(1, &mPriIdxId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _ia->nComponents
				* sizeof(unsigned int) * _ia->size, _ia->mData, _vboUsageMode);
	}
}

void
Vbo::setNData(VertexArray<char>* va, bool useGl)
{
	mPriNormals = va;
	if (useGl) {
		if (mPriNId == 0)
			glGenBuffers(1, &mPriNId);

		glBindBuffer(GL_ARRAY_BUFFER, mPriNId);
		glBufferData(
				GL_ARRAY_BUFFER, va->nComponents * sizeof(char) * va->size,
				va->mData, _vboUsageMode);
		glNormalPointer(GL_BYTE, va->stride, bufferOffset(0));
	}
}

void
Vbo::purge()
{
}

string
Vbo::toString(GLboolean b)
{
	if(b==GL_TRUE) return string("true");
	else return string("false");
}

/**
 *  @brief	 Special private method for drawing multiple VBOs.
 *  @return	nothing
 *
 *  This method is intended to be called only friendclass VBOManager when drawing multiple
 *  VBOs, thus avoiding switching on and off of the ClientStates
 */
void
Vbo::managedDraw()
{
	if (!isOffline){
		if (mPriNId!=0) {
			glBindBuffer(GL_ARRAY_BUFFER, mPriNId);
			glNormalPointer(GL_BYTE, mPriNormals->stride, bufferOffset(0));
		}
		if (mPriVId!=0) {
			glBindBuffer(GL_ARRAY_BUFFER, mPriVId);
			glVertexPointer(mPriVertices->nComponents, GL_FLOAT, mPriVertices->stride, bufferOffset(0));
		}
		if (mPriIdxId!=0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
		}

		if (mPriIdxId!=0)
			glDrawElements(GL_TRIANGLES, mPriVertices->size, GL_UNSIGNED_BYTE, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, mPriVertices->size);
	}
}

//void
//Vbo::setColor(V3f* _color)
//{
//
//	if (mPriCurrentColorF== 0) mPriCurrentColorF = new float[3];
//	mPriCurrentColorF[0] = _color->getX();
//	mPriCurrentColorF[1] = _color->getY();
//	mPriCurrentColorF[2] = _color->getZ();
//
//	if (mPriCurrentColorB== 0) mPriCurrentColorB = new unsigned char[3];
//	mPriCurrentColorB[0] =255*_color->getX();
//	mPriCurrentColorB[1] =255*_color->getY();
//	mPriCurrentColorB[2] =255*_color->getZ();
//}

//void
//Vbo::setColor(float r, float g, float b)
//{
//	cout << r << ", " << g << ", " << b << endl;
//	if (mPriCurrentColorF== 0) mPriCurrentColorF = new float[3];
//	mPriCurrentColorF[0] = r;
//	mPriCurrentColorF[1] = g;
//	mPriCurrentColorF[2] = b;
//
//	if (mPriCurrentColorB== 0) mPriCurrentColorB = new unsigned char[3];
//	mPriCurrentColorB[0] =255*r;
//	mPriCurrentColorB[1] =255*g;
//	mPriCurrentColorB[2] =255*b;
//}

//void
//Vbo::setColor(V3ub* _color)
//{
//	if (mPriCurrentColorF== 0) mPriCurrentColorF = new float[3];
//
//	float quotient = 1.0f/255.0f;
//	mPriCurrentColorF[0] = quotient * _color->getX();
//	mPriCurrentColorF[1] = quotient * _color->getY();
//	mPriCurrentColorF[2] = quotient * _color->getZ();
//
//	if (mPriCurrentColorB== 0) mPriCurrentColorB = new unsigned char[3];
//	mPriCurrentColorB[0] =_color->getX();
//	mPriCurrentColorB[1] =_color->getY();
//	mPriCurrentColorB[2] =_color->getZ();
//}

//void
//Vbo::setGlColor()
//{
//	glColor3ubv(defaultColorB.getData());
//}

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
Vbo::setOffline()
{
	if (!isOffline && !isGpuOnly){
		if (mPriNId!=0){
			// do stuff to normals
			glBindBuffer(GL_ARRAY_BUFFER, mPriNId);
			glBufferData(GL_ARRAY_BUFFER, 0, 0, _vboUsageMode);
		}
		if (mPriVId!=0){
			// do stuff to vertices
			glBindBuffer(GL_ARRAY_BUFFER, mPriVId);
			glBufferData(GL_ARRAY_BUFFER, 0, 0, _vboUsageMode);
		}
		if (mPriIdxId!=0){
			// do stuff to indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, _vboUsageMode);
		}
		isOffline=true;
	}
}

/**
 *  @brief Loads the VertexArray back to the GPU.
 *  @return   the number of bytes of grphic memory you are using.
 *
 *  This is only useful if the VBO was switches off earlier and, for caching reasons
 *  was not entirely deleted.
 */
void
Vbo::setOnline()
{
//	if (isOffline && !isGpuOnly){
	if (!isGpuOnly){
		if (mPriNormals != 0){
			// do stuff to normals
			if (mPriNId==0)
				glGenBuffers(1, &mPriNId);
			glBindBuffer(GL_ARRAY_BUFFER, mPriNId);
			glBufferData(GL_ARRAY_BUFFER, mPriNormals->nComponents*sizeof(char)*mPriNormals->size, mPriNormals->mData, _vboUsageMode);
			glNormalPointer(GL_BYTE, mPriNormals->stride, bufferOffset(0));
		}
		if (mPriVertices!=0){
			// do stuff to vertices
			if (mPriVId==0)
				glGenBuffers(1, &mPriVId);
			glBindBuffer(GL_ARRAY_BUFFER, mPriVId);
			glBufferData(GL_ARRAY_BUFFER, mPriVertices->nComponents*sizeof(float)*mPriVertices->size, mPriVertices->mData, _vboUsageMode);
			glVertexPointer(mPriVertices->nComponents, GL_FLOAT, mPriVertices->stride, bufferOffset(0));
		}
		if (mPriIndices!=0){
			// do stuff to indices
			if (mPriIdxId==0)
				glGenBuffers(1, &mPriIdxId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPriIndices->nComponents*sizeof(unsigned char)*mPriIndices->size, mPriIndices->mData, _vboUsageMode);
		}
		isOffline = false;
	}
}

/**
 *  @brief Calculate the CPU-Side amount of memory used by this VBO.
 *  @return   the number of bytes of cpu memory you are using.
 *
 *  Takes into account weather you are in GPU-Only Mode or not
 */
size_t
Vbo::getCpuMemory()
{
	if (isGpuOnly) return 0;
	else {
		size_t mem = 0;
		if (mPriVId!=0)
			mem += mPriVertices->size*mPriVertices->nComponents*sizeof(float);
		if (mPriNId!=0)
			mem  += mPriNormals->size*mPriNormals->nComponents*sizeof(char);
		if (mPriIdxId!=0)
			mem += mPriIndices->size*mPriIndices->nComponents*sizeof(int);
		return mem;
	}
}

/**
 *  @brief Calculate the GPU-Side amount of memory used by this VBO.
 *  @return   the number of bytes of graphic memory you are using.
 *
 *  Takes into account weather you are offline or not
 */
size_t
Vbo::getGpuMemory()
{
	if (isOffline) return 0;
	else {
		size_t mem = 0;
		if (mPriVId!=0)
			mem += mPriVertices->size*mPriVertices->nComponents*sizeof(float);
		if (mPriNId!=0)
			mem  += mPriNormals->size*3*sizeof(char);
		if (mPriIdxId!=0)
			mem += mPriIndices->size*mPriIndices->nComponents*sizeof(int);
		return mem;
	}
}

/**
 *  @brief Deletes vertexarrays and keeps the gpu-side up and running.
 *  @return   Nothing.
 *
 *  This deletes the VertexArrays and keeps the Mesh only in GPU-Memory.
 *  Be warned though, this is not revertable! After this, the VBO can not be turned offline.
 */
void
Vbo::setGpuOnly()
{
	if (!isOffline){
		if (mPriNormals!=0){
			delete mPriNormals;
			mPriNormals = 0;
		}
		if (mPriVertices!=0){
			delete mPriVertices;
			mPriVertices = 0;
		}
		if (mPriIndices){
			delete mPriIndices;
			mPriIndices = 0;
		}
		isGpuOnly = true;
	}
}

Vbo&
Vbo::operator+=(const Vbo& rhs)
{
	if (mPriIndices != 0) {
		*mPriIndices += *rhs.getIndices();
		setIndices(mPriIndices);
	}
	if (mPriNormals != 0) {
		*mPriNormals += *(rhs.getNData());
		setNData(mPriNormals, false);
	}
	if (mPriVertices != 0){
		*mPriVertices += *rhs.getVData();
		setVData(mPriVertices, false);
		mPriVertices->calcBB();
	}
//	stripDoubleTriangles();
	return *this;
}

Vbo
Vbo::operator+(const Vbo& rhs)
{
	Vbo b;

	if (mPriIndices != 0){
		VertexArray<unsigned int>* _vaui(mPriIndices);
		*_vaui += *rhs.getIndices();
		b.setIndices(_vaui);
	}
	if (mPriNormals != 0){
		VertexArray<char>* _vac(mPriNormals);
		*_vac += *rhs.getNData();
		b.setNData(_vac);
	}
	if (mPriVertices != 0){
		VertexArray<float>* _vaf(mPriVertices);
		*_vaf += *rhs.getVData();
		_vaf->calcBB();
		b.setVData(_vaf);
	}
	b.stripDoubleTriangles();
	return b;
}

bool
Vbo::operator==(const Vbo& rhs) const
{
	if (mPriIndices == rhs.mPriIndices && mPriVertices == rhs.mPriVertices && mPriNormals == rhs.mPriNormals){
		return true;
	} else {
		return false;
	}
}

void
Vbo::split(const BoundingBox& _bb, Vbo& _inside, Vbo& _outside)
{
	// each step a triangle is evaluated
	// notice that there is no evaluation of the correctness of given objects!
	if (_inside.getVData() == 0){
		_inside.setVData(new VertexArray<float>(4), false);
	}
	if (_inside.getNData() == 0){
		_inside.setNData(new VertexArray<char>(4), false);
	}
	if (_outside.getVData() == 0){
		_outside.setVData(new VertexArray<float>(4), false);
	}
	if (_outside.getNData() == 0){
		_outside.setNData(new VertexArray<char>(4), false);
	}

	for (unsigned int i = 0; i < mPriVertices->size * mPriVertices->nComponents; i
			+= mPriVertices->nComponents * 3) {
		// if it's completely inside
		if (_bb.isInside9Plus(mPriVertices->mData + i)) {
			_inside.addTriangle(mPriVertices->mData+i, mPriNormals->mData+i);
		}
		// if it intersects
		else if (_bb.intersects9Plus(mPriVertices->mData + i)){
			_inside.addTriangle(mPriVertices->mData+i, mPriNormals->mData+i);
			_outside.addTriangle(mPriVertices->mData+i, mPriNormals->mData+i);
		}
		// if it is outside
		else {
			_outside.addTriangle(mPriVertices->mData+i, mPriNormals->mData+i);
		}
	}

	// set the VBO-Data for the gpu, thus turning it on.
	// in practice for Octree-Generation the usefulness of this at this place remains to
	// be seen....
//	if (_inside.mPriVId==0)
//		glGenBuffers(1, &_inside.mPriVId);
//	if (_inside.mPriNId==0)
//		glGenBuffers(1, &_inside.mPriNId);
//	if (_outside.mPriVId==0)
//		glGenBuffers(1, &_outside.mPriVId);
//	if (_outside.mPriNId==0)
//		glGenBuffers(1, &_outside.mPriNId);
//
//	_inside.setOnline();
//	_outside.setOnline();

	/*
	 * foreach tri in *this as _t {
	 * 	if (_bb.isInside(_t)){
	 * 		copy all vertices and normals of _t into _inside
	 * 	}
	 * 	else if (_bb.intersects(_t))
	 * 		copy all vertices and normals of _t into _outside AND _inside
	 * 	else // is outside
	 * 		copy all vertices and normals of _t into _outside
	 * }
	 */
}

void Vbo::addTriangle(const float* _verts, const char* _normals)
{
	if (mPriVertices == 0){
		setVData(new VertexArray<float>(4), false);
	}
	if (mPriNormals == 0){
		setNData(new VertexArray<char>(4), false);
	}
	mPriVertices->addTriangle(_verts);
	mPriVertices->mPriBb.expand(_verts);
	mPriVertices->mPriBb.expand(_verts+4);
	mPriVertices->mPriBb.expand(_verts+8);
	mPriNormals->addTriangle(_normals);
}

//TODO optimize!
// like save a vector of indices which need deletion and then do the copy in one go.
void Vbo::stripDoubleTriangles()
{
	map<string, int> hMap;
	int faceComponents = mPriVertices->nComponents * 3;
	string hash;
	for (unsigned int i = 0; i < mPriVertices->nComponents * mPriVertices->size; i += faceComponents) {
		hash.assign(Face::toString(mPriVertices->mData + i, mPriVertices->nComponents));
		if (hMap.find(hash) == hMap.end()) {
			hMap.insert(make_pair(hash, 1));
		}
		else {
			mPriVertices->removeTriangle(i);
			mPriNormals->removeTriangle(i);
			i-=faceComponents;
		}
	}
//	setOnline();
}

bool
Vbo::hasVertices() const
{
	if (mPriVertices != 0 && mPriVertices->size>0) return true;
	else return false;
}

bool
Vbo::hasIndices() const
{
	if (mPriIndices != 0 && mPriIndices->size>0) return true;
	else return false;
}

bool
Vbo::hasNormals() const
{
	if (mPriNormals != 0 && mPriNormals->size>0) return true;
	else return false;
}

Vbo
Vbo::operator=(const Vbo& _rhs)
{
	if (_rhs == *this)
		return *this;
	else
		return Vbo(_rhs);

}

bool
Vbo::operator==(const Vbo& _rhs)
{
	if (this == &_rhs) return true;
	else return false;
}

void
Vbo::debug()
{
	cout << "VBO - Debug:" << endl;
	//cout << "We have a VertexbufferObject: " << toString(glIsBuffer(_vId)) << endl;
	//cout << "We have a NormalbufferObject: " << toString(glIsBuffer(_nId)) << endl;
	cout << "Size of vertices: " << mPriVertices->size<< endl;
	cout << "Size of normals: " << mPriNormals->size<< endl;
}

} // namespace OOCTools
