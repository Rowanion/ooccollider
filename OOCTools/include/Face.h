/*
 * Face.h
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#ifndef FACE_H_
#define FACE_H_

#include <vector>

#include "declarations.h"

namespace ooctools {

class Face
{
	public:
		Face();
		virtual ~Face();

		bool vert;
		int tex;
		bool norm;
		std::vector<V3f *>vertexPtrList;
		std::vector<V3f *>texturePtrList;
		std::vector<V3b *>normalPtrList;
		int matIdx;
		V3f *fNormal;
};

} // ooctools

#endif /* FACE_H_ */
