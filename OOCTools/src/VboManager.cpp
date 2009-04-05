/*
 * VBOManager.cpp
 *
 *  Created on: 28.02.2009
 *      Author: ava
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

VboManager* VboManager::getInstancePtr()
{
	if (mPriInstancePtr == 0)
	{
		mPriInstancePtr = new VboManager();
	}
	return mPriInstancePtr;
}

VboManager::VboManager() : mPriNVertices(0), mPriNFaces(0), mPriVboMap(0),
				usingIdxPtr(false), usingVertexPtr(false), usingNormalPtr(false),usingGlColor(false),
				mPriCgDiffuseParam(0), mPriMemoryUsage(0)
{
	// TODO Auto-generated constructor stub

}

VboManager::~VboManager()
{
	if (!mPriVboMap->empty()){
		for (mIterator it=mPriVboMap->begin(); it!=mPriVboMap->end(); ++it){
			delete it->second;
		}
	}
	if (mPriVboMap!=0) delete mPriVboMap;
}

void
VboManager::calcBB()
{
	// TODO reset BB prior to calculation
	for(mIterator it=mPriVboMap->begin(); it!=mPriVboMap->end(); ++it){
		mPriBb.expand(it->second->getBb());
	}
}

void
VboManager::addVbo(std::string id, Vbo* _vbo)
{
	if (mPriVboMap==0) mPriVboMap = new map<string, Vbo*>;
	mPriVboMap->insert(make_pair(id, _vbo));
	mPriBb.expand(_vbo->getBb());
	mPriNVertices += _vbo->mPriVertices->size;
	mPriNFaces += _vbo->mPriVertices->nFaces;
	mPriMemoryUsage += (_vbo->mPriVertices->size * _vbo->mPriVertices->nComponents * sizeof(float))
			+ (_vbo->mPriVertices->size * _vbo->mPriVertices->nComponents * sizeof(char));
}

Vbo*
VboManager::getVbo(std::string id)
{
	return mPriVboMap->at(id);
}

Vbo*
VboManager::getVbo(mIterator it)
{
	return getVbo(it->first);
}

void
VboManager::delVbo(std::string id)
{
	Vbo* vbo = mPriVboMap->at(id);
	mPriNVertices-=vbo->mPriVertices->size;
	mPriNFaces-=vbo->mPriVertices->nFaces;
	mPriVboMap->erase(id);
	if (vbo->getBb().hasSharedComponent(mPriBb)) calcBB();
	delete vbo;
}

void
VboManager::delVbo(mIterator it)
{
	Vbo* vbo = it->second;
	mPriNVertices-=vbo->mPriVertices->size;
	mPriNFaces-=vbo->mPriVertices->nFaces;
	mPriVboMap->erase(it);
	if (vbo->getBb().hasSharedComponent(mPriBb)) calcBB();
	delete vbo;
}

void
VboManager::drawNormals()
{
	for(mIterator it=mPriVboMap->begin(); it!=mPriVboMap->end(); ++it){
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
	for(mIterator it=mPriVboMap->begin(); it!=mPriVboMap->end(); ++it){
		if(it->second->mPriNId!=0 && !usingNormalPtr) Vbo::enableNPtr();
		else if (it->second->mPriNId==0 && usingNormalPtr) Vbo::disableNPtr();
		if(it->second->mPriVId!=0 && !usingVertexPtr) Vbo::enableVPtr();
		else if (it->second->mPriVId==0 && usingVertexPtr) Vbo::disableVPtr();
		if(it->second->mPriIdxId!=0 && !usingIdxPtr) Vbo::enableIPtr();
		else if (usingIdxPtr && it->second->mPriIdxId==0) Vbo::disableIPtr();

		if (usingGlColor)
			it->second->setGlColor();
		else if (mPriCgDiffuseParam!=0)
			CgToolkit::getInstancePtr()->cgSetParam3fv(mPriCgDiffuseParam, it->second->mPriCurrentColorF);

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
	Vbo *vbo = mPriVboMap->at(id);
	if(vbo->mPriNId!=0 && !usingNormalPtr) Vbo::enableNPtr();
	else if (vbo->mPriNId==0 && usingNormalPtr) Vbo::disableNPtr();
	if(vbo->mPriVId!=0 && !usingVertexPtr) Vbo::enableVPtr();
	else if (vbo->mPriVId==0 && usingVertexPtr) Vbo::disableVPtr();
	if(vbo->mPriIdxId!=0 && !usingIdxPtr) Vbo::enableIPtr();
	else if (usingIdxPtr && vbo->mPriIdxId==0) Vbo::disableIPtr();

	if (usingGlColor)
		vbo->setGlColor();
	else if (mPriCgDiffuseParam!=0)
		CgToolkit::getInstancePtr()->cgSetParam3fv(mPriCgDiffuseParam, vbo->mPriCurrentColorF);

	vbo->managedDraw();

	if(usingIdxPtr) Vbo::disableIPtr();
	if(usingNormalPtr) Vbo::disableNPtr();
	if(usingVertexPtr) Vbo::disableVPtr();
}

void
VboManager::drawBbs(float r, float g, float b)
{
	for (mIterator it=mPriVboMap->begin(); it!=mPriVboMap->end(); ++it){
		it->second->getBb().draw(r,g,b);
	}
}

void
VboManager::drawBbs(int r, int g, int b)
{
	for (mIterator it=mPriVboMap->begin(); it!=mPriVboMap->end(); ++it){
		it->second->getBb().draw(r,g,b);
	}
}

void
VboManager::drawBb(std::string id, float r, float g, float b)
{
	mPriVboMap->at(id)->getBb().draw(r, g, b);
}

void
VboManager::drawBb(std::string id, int r, int g, int b)
{
	mPriVboMap->at(id)->getBb().draw(r, g, b);
}

void
VboManager::makeVbos(Model* _model)
{
	mPriCt = _model->getColorTable();
	std::map<std::string, MetaGroup*>::iterator it;
	for (it = _model->getGrpStart(); it != _model->getGrpEnd(); ++it) {
		VertexArray<float>* va = _model->getVArrayPtr(it);
		VertexArray<char>* na = _model->getNArrayPtr(it);
		Vbo *vbo = new Vbo();
		vbo->setVData(va);
		vbo->setNData(na);
		cout << "passedMat: " << it->second->getMat().kdR << ", "
				<< it->second->getMat().kdG << ", "
				<< it->second->getMat().kdB << endl;

		vbo->setColor(it->second->getMat().kdR, it->second->getMat().kdG,
				it->second->getMat().kdB);
		addVbo(it->first, vbo);
	}
	for (mIterator mit = mPriVboMap->begin(); mit != mPriVboMap->end(); ++mit) {
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
	if (mPriVboMap->find(id) != mPriVboMap->end())
		mPriVboMap->find(id)->second->setOffline();
}

void
VboManager::switchOnVBO(std::string id)
{
	if (mPriVboMap->find(id) != mPriVboMap->end())
		mPriVboMap->find(id)->second->setOnline();
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

} // end of namespace OOCTools


