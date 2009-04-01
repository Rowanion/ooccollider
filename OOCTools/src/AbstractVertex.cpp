/*
 * AbstractVertex.cpp
 *
 *  Created on: 02.02.2009
 *      Author: ava
 */

#include "AbstractVertex.h"

namespace ooctools {

template <typename T>
AbstractVertex<T>::AbstractVertex() : _count(0){
	// TODO Auto-generated constructor stub
}

template <typename T>
AbstractVertex<T>::~AbstractVertex() {
	// TODO Auto-generated destructor stub
}

template <typename T>
int
AbstractVertex<T>::getCount()
{
	return _count;
}

template <typename T>
void
AbstractVertex<T>::setCount(int c)
{
	_count = c;
}

} // ooctools
