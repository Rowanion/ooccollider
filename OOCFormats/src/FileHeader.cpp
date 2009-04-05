/*
 * FileHeader.cpp
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#include "FileHeader.h"

#include "BoundingBox.h"
#include "V3ub.h"

namespace oocformats {

FileHeader::FileHeader() : bb(0), nFaces(0), nVertices(0), nNormals(0), color(0)
{
	// TODO Auto-generated constructor stub

}

FileHeader::~FileHeader()
{
	// presumably we're done here 'cause these two pointers are kept elsewhere
	delete bb;
	bb = 0;
	delete color;
	color = 0;
	nVertices = 0;
	nNormals = 0;
	nFaces = 0;
}

} // oocformats
