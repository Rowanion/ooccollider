/**
 * @file	Face.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 23.03.2009
 * @brief   Class definition of Face
 */

#include "Face.h"

#include <sstream>

#include "V3f.h"
#include "V4f.h"
#include "declarations.h"

namespace ooctools {

Face::Face() : vert(false), tex(0), norm(false), matIdx(0), fNormal(0)
{
	// TODO Auto-generated constructor stub

}

Face::~Face()
{
	delete fNormal;
	fNormal = 0;
	// attention - a face only contains references to the global list - no deletion allowed!
	if (!vertexPtrList.empty())
		vertexPtrList.clear();
	if (!texturePtrList.empty())
		texturePtrList.clear();
	if (!normalPtrList.empty())
		normalPtrList.clear();
}

std::string
Face::toString(const float* _array, unsigned int _components){
	std::stringstream st;
	st << "[";
	if (_components==4){
		st << " " << V4f::toString(_array) << ", ";
		st << V4f::toString(_array+_components) << ", ";
		st << V4f::toString(_array+(_components*2)) << " ";
	}
	st << "]";
	return st.str();
}

} // ooctools
