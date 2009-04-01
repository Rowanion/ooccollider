/*
 * Face.cpp
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#include "Face.h"

#include "V3f.h"
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

} // ooctools
