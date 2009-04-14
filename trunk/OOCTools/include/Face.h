/**
 * @file	Face.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 23.03.2009
 * @brief 	Face class declaration.
 */

#ifndef FACE_H_
#define FACE_H_

#include <vector>
#include <string>

#include "declarations.h"

namespace ooctools {

/**
 * @class Face
 * @brief Simple data class to represent a face (in our case <b>always a triangle</b>).
 * Keeps back-references to std::vectors with vertices and normals, but does not have to
 * clean them up.
 */
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

		/**
		 * @brief Converts a float-array into a string-representation.
		 * @param _array a float-array
		 * @param _components number of components per vertex
		 * @warning For now it only works with V4f, because that's what we have here.
		 * @return string representation of a triangle's coordinates.
		 */
		static std::string toString(const float* _array, unsigned int _components);
};

} // ooctools

#endif /* FACE_H_ */
