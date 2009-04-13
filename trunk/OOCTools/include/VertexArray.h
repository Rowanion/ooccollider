/**
 * @file	VertexArray.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.02.2009
 *
 */

#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cstddef>
#include <cstring>
#include "BoundingBox.h"
#include "declarations.h"

namespace ooctools {

/**
 * @class VertexArray
 * @brief Class represents a vertexarray with all additional information, like BoundingBox,
 * Vertex-Count and Face-Count.
 * L
 */
template<typename T>
class VertexArray
{
	public:
		VertexArray(int _components);
		VertexArray(int _components, size_t _size);
		VertexArray(int _components, size_t _size, T *_data);
		VertexArray(const VertexArray<T>& _va);
		~VertexArray();

		void calcBB();
		void setBB(BoundingBox &_bb){mPriBb = _bb;};
		float b2f(unsigned int _idx);
		VertexArray<T>& operator+=(const VertexArray<T>& rhs);
		VertexArray<T> operator+(const VertexArray<T>& rhs);

		/**
		 * Adds 3 vertices to this VertexArray. For simplification we assume that the given array
		 * consists of array with the same number of components as this VertexArray.
		 * @param _verts
		 */
		void addTriangle(const T* _verts);

		int nComponents; // V2, or V3 or V4?
		size_t size; // number of grouped components in data; example: v1(1,2,3), v2(3,6,7) = 2 vertices
		int stride; // bytes zwischen vertices. Bsp: byte4 = (b,b,b,b) 4*sizeof(byte) = 1, float4 = (f,f,f,f) = 4*sizeof(float) = 4
		unsigned int nFaces; // = size/components;
		T* mData;
		BoundingBox mPriBb;
};

// -------------------- definition from here ---------------------------------------

template<typename T>
VertexArray<T>::VertexArray(int _components) :
	nComponents(_components), size(0), mData(0)
{
}

template<typename T>
VertexArray<T>::VertexArray(int _components, size_t _size) :
	nComponents(_components), size(_size), stride(_components * sizeof(T)),
			nFaces(_size / _components), mData(0)
{
}

template<typename T>
VertexArray<T>::VertexArray(int _components, size_t _size, T* _data) :
	nComponents(_components), size(_size), stride(_components * sizeof(T)),
			nFaces(_size / _components), mData(_data)
{
}

/**
 * Copy-Constructor
 * @return nothing
 */
template<typename T>
VertexArray<T>::VertexArray(const VertexArray<T>& _va) :
	nComponents(_va.nComponents), size(_va.size), stride(_va.nComponents
			* sizeof(T)), nFaces(_va.size / _va.nComponents)
{
	mData = new T[size * nComponents * sizeof(T)];
	memcpy(mData, _va.mData, size * sizeof(T) * nComponents);
}

template<typename T>
VertexArray<T>::~VertexArray()
{
	if (mData != 0) {
		delete[] mData;
		mData = 0;
	}
	//	if (mPriBb != 0){
	//		delete mPriBb;
	//		mPriBb = 0;
	//	}
	//	std::cout <()< "deleting VertexArray..." << std::endl;
}

template<typename T>
void VertexArray<T>::calcBB()
{
	//TODO reset BB for calculation
	if (mData != 0) {
		for (unsigned int i = 0; i < size * nComponents; i += nComponents) {
			mPriBb.expand(mData + i);
		}
	}
}

template<typename T>
float VertexArray<T>::b2f(unsigned int _idx)
{
	return 127.0f / mData[_idx];
}

template<typename T>
VertexArray<T>&
VertexArray<T>::operator+=(const VertexArray<T>& rhs)
{
	if (rhs.size == 0 || rhs.mData == 0)
		return *this;
	else {
		T* newData = new T[nComponents * (size + rhs.size)];
		T* halfPtr = newData + (nComponents * size);
		memcpy(newData, mData, size * sizeof(T) * nComponents);
		memcpy(halfPtr, rhs.mData, rhs.size * sizeof(T) * rhs.nComponents);
		mPriBb.expand(rhs.mPriBb);
		size += rhs.size;
		nFaces += rhs.nFaces;
		delete[] mData;
		mData = 0;
		mData = newData;
		return *this;
	}
}

template<typename T>
VertexArray<T>
VertexArray<T>::operator+(const VertexArray<T>& rhs)
{
	if (rhs.size == 0 || rhs.mData == 0)
		return *this;
	if (size == 0 || mData == 0)
		return rhs;
	else {
		T* newData = new T[nComponents * (size + rhs.size)];
		T* halfPtr = newData + (nComponents * size);
		memcpy(newData, mData, size * sizeof(T) * nComponents);
		memcpy(halfPtr, rhs.mData, rhs.size * sizeof(T) * rhs.nComponents);
		VertexArray<T> newVA(nComponents, size + rhs.size, newData);

		return newVA;
	}
}

template<typename T>
void
VertexArray<T>::addTriangle(const T* _verts)
{
	if (size == 0 || mData == 0){
		size = 3;
		stride = nComponents * sizeof(T);
		nFaces = 3;
		mData = new T[nComponents*size];
		memcpy(mData, _verts, size * sizeof(T) * nComponents);
	}
	else {
		T* newData = new T[nComponents * (size+3)];
		T* halfPtr = newData + (nComponents * size);
		memcpy(newData, mData, size * sizeof(T) * nComponents);
		memcpy(halfPtr, _verts, 3 * sizeof(T) * nComponents);
		delete[] mData;
		mData = 0;
		mData = newData;
		size += 3;
		++nFaces;
	}
}

} // end of namespace OOCTools

#endif /* VERTEXARRAY_H_ */
