/*
 * MetaGroup.h
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#ifndef METAGROUP_H_
#define METAGROUP_H_

#include <string>
#include <vector>

#include "StructDefs.h"
#include "declarations.h"

namespace ooctools {

class MetaGroup
{
	public:
		MetaGroup();
		MetaGroup(std::string _name);
		virtual ~MetaGroup();

		void setMat(Material* mat);
		const Material& getMat() const {return matPtr;};

		std::string name;
		int nFaces;
		int nVertices;
		int nTextureCoords;
		int nNormals;
		BoundingBox *bb;
		std::vector<Face*> fPtrList;
		//		V3ub *color;
	private:
		Material matPtr;
};

} // ooctools

#endif /* METAGROUP_H_ */
