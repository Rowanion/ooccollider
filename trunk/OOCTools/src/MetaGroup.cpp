/*
 * MetaGroup.cpp
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#include "MetaGroup.h"

#include "BoundingBox.h"
#include "GlToolkit.h"
#include "StructDefs.h"
#include "Face.h"

namespace ooctools {

MetaGroup::MetaGroup() :
	name("defaultGrp"), nFaces(0), nVertices(0), nTextureCoords(0),
			nNormals(0), bb(new BoundingBox())
{
	matPtr.kdR = defaultColorF->getR();
	matPtr.kdG = defaultColorF->getG();
	matPtr.kdB = defaultColorF->getB();
}

MetaGroup::MetaGroup(std::string _name) :
	name(_name), nFaces(0), nVertices(0), nTextureCoords(0),
			nNormals(0), bb(new BoundingBox())
{
	matPtr.kdR = defaultColorF->getR();
	matPtr.kdG = defaultColorF->getG();
	matPtr.kdB = defaultColorF->getB();
}

MetaGroup::~MetaGroup()
{
	delete bb;
	bb = 0;
//	delete matPtr;
//	matPtr = 0;
	if (!fPtrList.empty()){
		std::vector<Face*>::iterator it;
		for (it=fPtrList.begin(); it != fPtrList.end(); ++it){
			delete (*it);
		}
		fPtrList.empty();
	}
}

void MetaGroup::setMat(Material* mat){
	matPtr = *mat;
}

} // ooctools
