/*
 * VertexArray.h
 *
 *  Created on: 12.02.2009
 *      Author: ava
 */

#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cstddef>

#include "BoundingBox.h"
#include "declarations.h"

namespace ooctools {
template <typename T>
class VertexArray
{
public:
	VertexArray();
	VertexArray(int _components, size_t _size);
	VertexArray(int _components, size_t _size, T *_data);
	~VertexArray();

	void calcBB();
	void setBB(BoundingBox *_bb){mPriBb = _bb;};
	float b2f(unsigned int _idx);

	int nComponents; // V2, or V3 or V4?
	size_t size; // number of grouped components in data; example: v1(1,2,3), v2(3,6,7) = 2 vertices
	int stride; // bytes zwischen vertices. Bsp: byte4 = (b,b,b,b) 4*sizeof(byte) = 1, float4 = (f,f,f,f) = 4*sizeof(float) = 4
	int nFaces; // = size/components;
	T* mData;
	BoundingBox *mPriBb;
};

// -------------------- definition from here ---------------------------------------

template <typename T>
VertexArray<T>::VertexArray() :
	nComponents(0), size(0), mData(0), mPriBb(0)
{
}

template <typename T>
VertexArray<T>::VertexArray(int _components, size_t _size) :
	nComponents(_components),size(_size), mData(0), mPriBb(0)
{
}

template <typename T>
VertexArray<T>::VertexArray(int _components, size_t _size, T* _data) :
	nComponents(_components), size(_size), stride(_components * sizeof(T)),
	nFaces(_size/_components), mData(_data), mPriBb(0)
{
}

template <typename T>
VertexArray<T>::~VertexArray()
{
	if (mData!=0){
		delete[] mData;
		mData = 0;
	}
	if (mPriBb != 0){
		delete mPriBb;
		mPriBb = 0;
	}
	std::cout << "deleting VertexArray..." << std::endl;
}

template<typename T>
void
VertexArray<T>::calcBB()
{
	if (mData!=0){
		if (mPriBb==0) mPriBb = new BoundingBox();
		for(unsigned int i=0; i< size; ++i){
			mPriBb->expand(&mData[i]);
		}
	}
}

template<typename T>
float
VertexArray<T>::b2f(unsigned int _idx)
{
	return 127.0f/mData[_idx];
}

} // end of namespace OOCTools

#endif /* VERTEXARRAY_H_ */
