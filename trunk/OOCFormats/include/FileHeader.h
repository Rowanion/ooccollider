/*
 * FileHeader.h
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#ifndef FILEHEADER_H_
#define FILEHEADER_H_

#include "../../OOCTools/include/declarations.h"

namespace oocformats {

class FileHeader
{
	public:
		FileHeader();
		virtual ~FileHeader();

		/**
		 * Header-Format: 3xfloat32[minBB], 3xfloat32[maxBB], uint32[faceCount], int32[vertCount],
		 * int32[normCount], 3xuchar[color]
		 * Headersize: 39 byte
		 */
		static unsigned int getHeaderSize(){return 39;};

		ooctools::BoundingBox* bb;
		unsigned int nFaces;
		int nVertices;
		int nNormals;
		ooctools::V3ub* color;
};

} // oocformats

#endif /* FILEHEADER_H_ */
