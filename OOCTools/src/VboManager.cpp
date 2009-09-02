/**
 * @file	VboManager.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.02.2009
 * @brief   Class definition of VboManager
 */

#include "VboManager.h"

#include <string>
#include <map>
#include <iostream>
#include <stdio.h>

#include "CgToolkit.h"
#include "MetaGroup.h"
#include "Model.h"
#include "BoundingBox.h"
#include "Vbo.h"

namespace ooctools {

using namespace std;

VboManager* VboManager::mPriInstancePtr = 0;

VboManager* VboManager::getSingleton()
{
	if (mPriInstancePtr == 0)
	{
		mPriInstancePtr = new VboManager();
	}
	return mPriInstancePtr;
}

VboManager::VboManager() :
	mPriNVertices(0), mPriNFaces(0), usingIdxPtr(false), usingVertexPtr(false),
			usingNormalPtr(false), usingGlColor(false), mPriCgDiffuseParam(0),
			mPriMemoryUsage(0)
{
	// TODO Auto-generated constructor stub

}

VboManager::~VboManager()
{
	if (!mPriVboMap.empty()){
		for (mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
			delete it->second;
		}
	}
	mPriVboMap.clear();
}

void
VboManager::calcBB()
{
	BoundingBox newBb;
	// TODO reset BB prior to calculation
	for(mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
		newBb.expand(it->second->getBb());
	}
	mPriBb = newBb;
}

void
VboManager::addVbo(std::string id, Vbo* _vbo)
{
	mPriVboMap.insert(make_pair(id, _vbo));
	mPriBb.expand(_vbo->getBb());
	mPriNVertices += _vbo->mPriVertices->size;
	mPriNFaces += _vbo->mPriVertices->nFaces;
	mPriMemoryUsage += (_vbo->mPriVertices->size * _vbo->mPriVertices->nComponents * sizeof(float))
			+ (_vbo->mPriVertices->size * _vbo->mPriVertices->nComponents * sizeof(char));
}

const Vbo*
VboManager::getVbo(std::string id) const
{
	mcIterator mit = mPriVboMap.find(id);
	if (mit != mPriVboMap.end())
		return mit->second;
	else return 0;
}

const Vbo*
VboManager::getVbo(mIterator it) const
{
	return getVbo(it->first);
}

void
VboManager::delVbo(std::string id)
{
	Vbo* vbo = mPriVboMap[id];
	mPriNVertices-=vbo->mPriVertices->size;
	mPriNFaces-=vbo->mPriVertices->nFaces;
	mPriVboMap.erase(id);
	if (vbo->getBb().hasSharedComponent(mPriBb)) calcBB();
	delete vbo;
}

void
VboManager::delVbo(mIterator it)
{
	Vbo* vbo = it->second;
	mPriNVertices-=vbo->mPriVertices->size;
	mPriNFaces-=vbo->mPriVertices->nFaces;
	mPriVboMap.erase(it);
	if (vbo->getBb().hasSharedComponent(mPriBb)) calcBB();
	delete vbo;
}

void
VboManager::drawNormals()
{
	for(mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
		if(it->second->mPriNId!=0 && !usingNormalPtr){
			for (unsigned int i=0; i<it->second->mPriNormals->size; ++i){
			glBegin(GL_LINES);
				glColor3f(1,1,0);
				glVertex3fv(&it->second->mPriVertices->mData[i+i*it->second->mPriVertices->nComponents]);
				glColor3f(1,0,0);
				glVertex3f(it->second->mPriNormals->b2f(i+i*it->second->mPriNormals->nComponents), it->second->mPriNormals->b2f(i+1+i*it->second->mPriNormals->nComponents), it->second->mPriNormals->b2f(i+2+i*it->second->mPriNormals->nComponents));
			glEnd();
			}
		}
	}
}

void
VboManager::drawVbos()
{
	for(mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
		if(it->second->mPriNId!=0 && !usingNormalPtr) Vbo::enableNPtr();
		else if (it->second->mPriNId==0 && usingNormalPtr) Vbo::disableNPtr();
		if(it->second->mPriVId!=0 && !usingVertexPtr) Vbo::enableVPtr();
		else if (it->second->mPriVId==0 && usingVertexPtr) Vbo::disableVPtr();
		if(it->second->mPriIdxId!=0 && !usingIdxPtr) Vbo::enableIPtr();
		else if (usingIdxPtr && it->second->mPriIdxId==0) Vbo::disableIPtr();

//		if (usingGlColor)
//			it->second->setGlColor();
//		else if (mPriCgDiffuseParam!=0)
//			CgToolkit::getInstancePtr()->cgSetParam3fv(mPriCgDiffuseParam, it->second->mPriCurrentColorF);

		it->second->managedDraw();

	}
	if(usingIdxPtr) Vbo::disableIPtr();
	if(usingNormalPtr) Vbo::disableNPtr();
	if(usingVertexPtr) Vbo::disableVPtr();
//	drawNormals();
}

void
VboManager::drawVbo(std::string id)
{
	Vbo *vbo = mPriVboMap[id];
	if(vbo->mPriNId!=0 && !usingNormalPtr) Vbo::enableNPtr();
	else if (vbo->mPriNId==0 && usingNormalPtr) Vbo::disableNPtr();
	if(vbo->mPriVId!=0 && !usingVertexPtr) Vbo::enableVPtr();
	else if (vbo->mPriVId==0 && usingVertexPtr) Vbo::disableVPtr();
	if(vbo->mPriIdxId!=0 && !usingIdxPtr) Vbo::enableIPtr();
	else if (usingIdxPtr && vbo->mPriIdxId==0) Vbo::disableIPtr();

//	if (usingGlColor)
//		vbo->setGlColor();
//	else if (mPriCgDiffuseParam!=0)
//		CgToolkit::getInstancePtr()->cgSetParam3fv(mPriCgDiffuseParam, vbo->mPriCurrentColorF);

	vbo->managedDraw();

	if(usingIdxPtr) Vbo::disableIPtr();
	if(usingNormalPtr) Vbo::disableNPtr();
	if(usingVertexPtr) Vbo::disableVPtr();
}

void
VboManager::drawBbs(float r, float g, float b)
{
	for (mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
		it->second->getBb().draw(r,g,b);
	}
}

void
VboManager::drawBbs(int r, int g, int b)
{
	for (mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
		it->second->getBb().draw(r,g,b);
	}
}

void
VboManager::drawBb(std::string id, float r, float g, float b)
{
	mPriVboMap[id]->getBb().draw(r, g, b);
}

void
VboManager::drawBb(std::string id, int r, int g, int b)
{
	mPriVboMap[id]->getBb().draw(r, g, b);
}

void
VboManager::makeVbos(Model* _model)
{
	mPriCt = _model->getColorTable();
	std::map<std::string, MetaGroup*>::iterator it;
	for (it = _model->getGrpStart(); it != _model->getGrpEnd(); ++it) {
		VertexArray<float>* va = _model->getVArrayPtr(it);
		VertexArray<char>* na = _model->getNArrayPtr(it);
		Vbo *vbo = new Vbo(0, na, va);
//		vbo->setVData(va);
//		vbo->setNData(na);
		cout << "passedMat: " << it->second->getMat().kdR << ", "
				<< it->second->getMat().kdG << ", "
				<< it->second->getMat().kdB << endl;

//		vbo->setColor(it->second->getMat().kdR, it->second->getMat().kdG,
//				it->second->getMat().kdB);
		addVbo(it->first, vbo);
	}
	for (mIterator mit = mPriVboMap.begin(); mit != mPriVboMap.end(); ++mit) {
		cout << "BoundingBox for Group " << mit->first << ": "
				<< mit->second->mPriVertices->mPriBb.toString() << endl;
	}
}

void
VboManager::printInfo()
{
	cout << "--------------------------------------------------------" << endl;
	cout << "Loaded No. of Vertices: " << mPriNVertices << "(x2 for the normals)"<< endl;
	cout << "Loaded No. of Triangles: " << mPriNFaces << endl;
	cout << "Number of VBOs: " << mPriVboMap.size() << endl;
	size_t memory = sizeof(float) * mPriNVertices * 4 + sizeof(char)
			* mPriNVertices * 4;
	if (mPriMemoryUsage > 1073741824)
		cout << "Memory usage: " << (float) (mPriMemoryUsage / 1073741824.0f)
				<< " Gigabytes." << endl;
	else if (mPriMemoryUsage > 1048576)
		cout << "Memory usage: " << (float) (mPriMemoryUsage / 1048576.0f)
				<< " Megabytes." << endl;
	else if (mPriMemoryUsage > 1024)
		cout << "Memory usage: " << (float) (mPriMemoryUsage / 1024.0f) << " Kilobytes."
				<< endl;
	else if (mPriMemoryUsage <= 1024)
		cout << "Memory usage: " << mPriMemoryUsage << " bytes." << endl;
	cout << "--------------------------------------------------------" << endl;
}

void
VboManager::switchOffVBO(std::string id)
{
	if (mPriVboMap.find(id) != mPriVboMap.end())
		mPriVboMap.find(id)->second->setOffline();
}

void
VboManager::switchOnVBO(std::string id)
{
	if (mPriVboMap.find(id) != mPriVboMap.end())
		mPriVboMap.find(id)->second->setOnline();
}

ColorTable&
VboManager::getColorTable()
{
	return mPriCt;
}

void
VboManager::setColorTable(const ColorTable& _ct)
{
	mPriCt = _ct;
}

/**
 * @brief Debug-Function which merges all VBOs into the first one and removes the rest.
 * Used to test the Addition-Operator for the later Octree-Generation.
 */
void VboManager::mergeDown()
{
	Vbo* firstVbo = new Vbo(*(mPriVboMap.begin()->second));
	string firstId = mPriVboMap.begin()->first;
	mIterator mIt = mPriVboMap.begin();
	for (; mIt != mPriVboMap.end(); ++mIt) {
		if (mIt->first != firstId) {
//			cout << "size of orig vbo: " << mIt->second->mPriVertices->size
//					<< endl;
			*firstVbo += *mIt->second;
		}
	}
//	cout << "number of vbos now: " << mPriVboMap.size() << endl;
	clear();
	addVbo("1", firstVbo);
	mPriVboMap.begin()->second->mPriVertices->calcBB();
	firstVbo->stripDoubleTriangles();
}

void
VboManager::clear()
{
//	cout << "mapsize: " << mPriVboMap.size() << endl;
//	cout << "bb-max: " << mPriBb.getMax().toString() << endl;
//	cout << "nfaces: " << mPriNFaces << endl;
	if (!mPriVboMap.empty()){
		for (mIterator it=mPriVboMap.begin(); it!=mPriVboMap.end(); ++it){
			Vbo* value = it->second;
//			cout << value->getBb().toString() << endl;
			delete value;
			value = 0;

		}
	}
	mPriVboMap.clear();
}

void VboManager::debugSplit(BoundingBox* _bb)
{
	Vbo* v = mPriVboMap.begin()->second;
	Vbo* in = new Vbo;
	Vbo* out = new Vbo;
	v->split(*_bb, *in, *out);
	delVbo(mPriVboMap.begin());
	addVbo("defau", out);
	addVbo("defau1", in);
}

} // end of namespace OOCTools


