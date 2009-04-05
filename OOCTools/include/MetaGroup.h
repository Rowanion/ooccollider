/**
 * @file	MetaGroup.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 23.03.2009
 *
 */

#ifndef METAGROUP_H_
#define METAGROUP_H_

#include <string>
#include <vector>

#include "StructDefs.h"
#include "declarations.h"

namespace ooctools {

/**
 * @class C
 * @brief B
 * L
 */
class MetaGroup
{
	public:
		MetaGroup();
		MetaGroup(std::string _name);
		virtual ~MetaGroup();

		void setMat(const Material& mat);
		const Material& getMat() const {return mat;};

		std::string name;
		unsigned int nFaces;
		unsigned int nVertices;
		unsigned int nTextureCoords;
		unsigned int nNormals;
		BoundingBox *bb;
		std::vector<Face*> fPtrList;
		//		V3ub *color;
	private:
		Material mat;
};

} // ooctools

#endif /* METAGROUP_H_ */
