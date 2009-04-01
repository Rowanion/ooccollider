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

		ooctools::BoundingBox* bb;
		int nVertices;
		int nNormals;
		ooctools::V3ub* color;
};

} // oocformats

#endif /* FILEHEADER_H_ */
