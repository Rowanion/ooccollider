/*
 * Model.cpp
 *
 *  Created on: 30.01.2009
 *      Author: ava
 */

#include "Model.h"

#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <iostream>
#include <cstdlib>

#include "BoundingBox.h"
#include "Face.h"
#include "MetaGroup.h"
#include "StructDefs.h"
#include "V3f.h"
#include "V3b.h"
#include "V4f.h"
#include "VertexArray.h"

namespace ooctools {
using namespace std;

Model::Model() : mPriGrpCount(0), mPriMaterialCount(0), mPriFaceCount(0), mPriVertexCount(0) ,
				mPriTextureCount(0), mPriNormalCount(0), mPriUniqueVertexCount(0),
				mPriUniqueNormalCount(0), mPriUniqueTextureCount(0), mPriBB()
{

//	createDefaultMaterial();
	mPriCurrentGrpPtr = 0;
}

Model::~Model()
{

//	exit(0);
//	if (mPriBB!=0){
//		delete mPriBB;
//		mPriBB = 0;
//	}

	vector<V3f*>::iterator v3It;
	for(v3It=mPriVertexPtrList.begin(); v3It!=mPriVertexPtrList.end(); ++v3It){
		delete (*v3It);
	}
	mPriVertexPtrList.clear();

	for(v3It=mPriTexturePtrList.begin(); v3It!=mPriTexturePtrList.end(); ++v3It){
		delete (*v3It);
	}
	mPriTexturePtrList.clear();

	vector<V3b*>::iterator vbIt;
	for(vbIt=mPriNormalPtrList.begin(); vbIt!=mPriNormalPtrList.end(); ++vbIt){
		delete (*vbIt);
	}
	mPriNormalPtrList.clear();

	map<string, MetaGroup*>::iterator gIt;
	for(gIt=mPriGrpPtrMap.begin(); gIt!=mPriGrpPtrMap.end(); ++gIt){
		delete (gIt->second);
	}
	mPriGrpPtrMap.clear();

	mPriCurrentGrpPtr = 0;
//	cout << "Model deleted..." << endl;
}


/*
 * adds a pointer to a MetaGroup object to its internal list, thus increasing internal groupcount
 */
void
Model::addGrpPtr(MetaGroup* _g)
{
	mPriGrpPtrMap.insert(make_pair( _g->name, _g ));
	setCurrentGrp(_g->name);
	++mPriGrpCount;

}
/*
std::map<string, MetaObj>::iterator Model::getLastObj(){
	return (--_objList.end());
}
std::map<string, MetaObj>::iterator Model::getFirstObj(){
	return _objList.begin();
}
*/
void
Model::addVPtr(V3f* _vertex)
{
	mPriVertexPtrList.push_back(_vertex);
	++mPriUniqueVertexCount;
	mPriBB.expand(*_vertex);
}

void
Model::addFPtrToCurrent(Face* _face)
{
	if (mPriCurrentGrpPtr!=0) {
		mPriCurrentGrpPtr->fPtrList.push_back(_face);
		mPriCurrentGrpPtr->nFaces++;
		++mPriFaceCount;
	}
}
/*
void Model::addFtoId(Face fac, int i){
	fac.matIdx = _activeMaterialIndex;
	_objList.at(i).f.push_back(fac);
	_objList.at(i).faceCount++;
	++_faceCount;
}
*/
void
Model::addNPtr(V3b* _normal)
{
	mPriNormalPtrList.push_back(_normal);
	++mPriUniqueNormalCount;
}
void
Model::addTPtr(V3f* _tex)
{
	mPriTexturePtrList.push_back(_tex);
	++mPriUniqueTextureCount;
}

void
Model::setCurrentGrp(std::string s)
{
	mPriCurrentGrpPtr = mPriGrpPtrMap[s];
}

MetaGroup*
Model::getCurrentGrpPtr()
{
	return mPriCurrentGrpPtr;
}

//MetaObj *Model::getObj(int i){
//	return &_objList.at(i);
//}

VertexArray<float>*
Model::getVArrayPtr(map<string, MetaGroup*>::iterator _it)
{
	return getVArrayPtr(_it->second);
}

VertexArray<float>*
Model::getVArrayPtr(string _id)
{
	return getVArrayPtr(mPriGrpPtrMap[_id]);
}

VertexArray<float>*
Model::getVArrayPtr(MetaGroup* _grp)
{
	VertexArray<float>* va = new VertexArray<float>(4);
	va->mData = new float[_grp->nVertices*4];
	va->stride = 4*sizeof(float);
	va->size = _grp->nVertices;
	size_t posCount = 0;
	vector<Face*>::const_iterator face_it;
	for (face_it = _grp->fPtrList.begin(); face_it != _grp->fPtrList.end(); ++face_it){
		vector<V3f *>::const_iterator v_it;
		for (v_it=(*face_it)->vertexPtrList.begin(); v_it!=(*face_it)->vertexPtrList.end(); ++v_it){
			va->mData[posCount++] = (*v_it)->getX();
			va->mData[posCount++] = (*v_it)->getY();
			va->mData[posCount++] = (*v_it)->getZ();
			va->mData[posCount++] = mPriCTable.calculateTexCoord(mPriCTable.addColori(_grp->getMat()));
		}
	}
	va->setBB(*_grp->bb);
	return va;
}

VertexArray<char>*
Model::getNArrayPtr(map<string, MetaGroup*>::iterator _it)
{
	return getNArrayPtr(_it->second);
}

VertexArray<char>*
Model::getNArrayPtr(string _id)
{
	return getNArrayPtr(mPriGrpPtrMap[_id]);
}

VertexArray<char>*
Model::getNArrayPtr(MetaGroup *_grpPtr)
{
	VertexArray<char>* va = new VertexArray<char>(4);
	va->mData = new char[_grpPtr->nNormals*4]; // we have normalCount normals and each of it has 3 components + 1 harmonic component -> power of 4 FTW!
	va->stride = 4*sizeof(char);
	va->size = _grpPtr->nNormals;

	size_t posCount = 0;
	vector<Face*>::const_iterator face_it;
	for (face_it = _grpPtr->fPtrList.begin(); face_it != _grpPtr->fPtrList.end(); ++face_it){
		vector<V3b *>::const_iterator v_it;
		for (v_it=(*face_it)->normalPtrList.begin(); v_it!=(*face_it)->normalPtrList.end(); ++v_it){
			va->mData[posCount++] = (*v_it)->getX();
			va->mData[posCount++] = (*v_it)->getY();
			va->mData[posCount++] = (*v_it)->getZ();
			va->mData[posCount++] = 1;
		}
	}
	return va;
}

void
Model::expandBB(V3f* _v)
{
	mPriBB.expand(*_v);
}

void
Model::expandBB(V4f* _v){
	mPriBB.expand(*_v);
}

std::map<std::string, MetaGroup*>::iterator
Model::getGrpStart()
{
	return mPriGrpPtrMap.begin();
}

std::map<std::string, MetaGroup*>::iterator
Model::getGrpEnd()
{
	return mPriGrpPtrMap.end();
}

V3f*
Model::getVPtr(size_t _id)
{
	return mPriVertexPtrList.at(_id);
}

V3f*
Model::getTPtr(size_t _id)
{
	return mPriTexturePtrList.at(_id);
}

V3b*
Model::getNPtr(size_t _id)
{
	return mPriNormalPtrList.at(_id);
}

void
Model::setColorTable(ColorTable& _ct)
{
	mPriCTable = _ct;
}

void
Model::calculateNormals()
{
	map<string, MetaGroup*>::iterator gIt;
	for(gIt = mPriGrpPtrMap.begin(); gIt!=mPriGrpPtrMap.end(); ++gIt){
		vector<Face*>::iterator fIt;
		for (fIt = gIt->second->fPtrList.begin(); fIt!=gIt->second->fPtrList.end(); ++fIt){
			V3f::calcFNormal(*fIt);
		}
	}

	for(gIt = mPriGrpPtrMap.begin(); gIt!=mPriGrpPtrMap.end(); ++gIt){
		vector<Face*>::iterator fIt;
		for (fIt = gIt->second->fPtrList.begin(); fIt!=gIt->second->fPtrList.end(); ++fIt){
			for(size_t i = 0; i< (*fIt)->vertexPtrList.size(); ++i){
				V3f *currentV = (*fIt)->vertexPtrList.at(i);
				V3b *currentN = (*fIt)->normalPtrList.at(i);
				V3f temp(0,0,0);
				for(size_t j=0; j<currentV->mPriFaceList->size(); ++j){
					temp += (*currentV->mPriFaceList->at(j)->fNormal);
				}
				temp/=currentV->mPriFaceList->size();
				temp.normalize();

				currentN->setX((char)127.0f*temp.getX());
				currentN->setY((char)127.0f*temp.getY());
				currentN->setZ((char)127.0f*temp.getZ());
			}
		}
	}
















//	vector<V3f*>::iterator vIt;
//	for(size_t i = 0; i< mPriVertexPtrList.size(); ++i){
//		// TODO take care of the case that we don't have ANY normals yet
//		V3f *currentV = mPriVertexPtrList.at(i);
//		V3b *currentN = mPriNormalPtrList.at(i);
//		cout << "No Faces: " << currentV->mPriFaceList->size()<< endl;
//		V3f temp(0,0,0);
//		for(size_t j = 0; j<currentV->mPriFaceList->size(); ++j){
//			temp+=(*currentV->mPriFaceList->at(j)->fNormal);
//		}
//		temp/=mPriVertexPtrList.size();
//		temp.normalize();
//
//		currentN->setX((char)127.0f*temp.getX());
//		currentN->setY((char)127.0f*temp.getY());
//		currentN->setZ((char)127.0f*temp.getZ());
//	}
}

const ColorTable&
Model::getColorTable() const
{
	return mPriCTable;
}

} // namespace OOCTools {
