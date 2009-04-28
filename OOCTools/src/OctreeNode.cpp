/**
 * @file	OctreeNode.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 13.04.2009
 * @brief   Class definition of OctreeNode
 */

#include "OctreeNode.h"

#include <string>

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "../../OOCFormats/include/FileIO.h"
#include "VboManager.h"
#include "../../OOCFormats/include/OctreeHandler.h"

using namespace std;
using namespace oocformats;
namespace fs = boost::filesystem;
namespace ooctools {

OctreeNode::OctreeNode(OctreeNode* _father, const BoundingBox& _bb,
		string _path) :
	mPriBb(_bb), path(_path), father(_father), fne(0), fnw(0), fse(0), fsw(
			0), bne(0), bnw(0), bse(0), bsw(0)
{
	// TODO Auto-generated constructor stub
	mPriLevel = calculateLevel();

}

OctreeNode::~OctreeNode()
{
	if (!isLeaf()) {
		delete fne;
		delete fnw;
		delete fse;
		delete fsw;
		delete bne;
		delete bnw;
		delete bse;
		delete bsw;
	}
	// TODO Auto-generated destructor stub
}

bool OctreeNode::isRoot()
{
	if (father == 0)
		return true;
	else
		return false;
}

unsigned char OctreeNode::calculateLevel(unsigned char counter)
{
	if (isRoot())
		return counter;
	else
		return father->calculateLevel(counter + 1);
}

bool OctreeNode::isLeaf()
{
	return !(fne || fnw || fse || fsw || bne || bnw || bse || bsw);
}

BoundingBox OctreeNode::subdivideFne()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMax().getX(),
			mPriBb.getMax().getY(), mPriBb.getMin().getZ()));
}

BoundingBox OctreeNode::subdivideFnw()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMin().getX(),
			mPriBb.getMax().getY(), mPriBb.getMin().getZ()));
}

BoundingBox OctreeNode::subdivideFse()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMax().getX(),
			mPriBb.getMin().getY(), mPriBb.getMin().getZ()));
}

BoundingBox OctreeNode::subdivideFsw()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMin().getX(),
			mPriBb.getMin().getY(), mPriBb.getMin().getZ()));
}

BoundingBox OctreeNode::subdivideBne()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMax().getX(),
			mPriBb.getMax().getY(), mPriBb.getMax().getZ()));
}

BoundingBox OctreeNode::subdivideBnw()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMin().getX(),
			mPriBb.getMax().getY(), mPriBb.getMax().getZ()));
}

BoundingBox OctreeNode::subdivideBse()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMax().getX(),
			mPriBb.getMin().getY(), mPriBb.getMax().getZ()));
}

BoundingBox OctreeNode::subdivideBsw()
{
	V3f center;
	mPriBb.computeCenter(center);
	return BoundingBox(center, V3f(mPriBb.getMin().getX(),
			mPriBb.getMin().getY(), mPriBb.getMax().getZ()));
}

bool
OctreeNode::insertVbo(ooctools::Vbo& _vbo)
{
	// always divide at root
	if (isRoot()) {
		subdivide(_vbo);
	} // are we already at the deepest level?
	else if (mPriLevel == MAX_OCTREE_LEVEL) {
		Vbo* old = 0;
		old = OctreeHandler::readOctreeVbo(fs::path(path));
		// is there already a written file present?
		if (old == 0){
			FileIO::recursiveTestAndSet(path);
			OctreeHandler::writeOctreeVbo(_vbo, fs::path(path));
		} // if so - merge it with this one and save it
		else {
			Vbo* originalVbo = new Vbo(_vbo);
			VboManager::getSingleton()->addVbo("1", originalVbo);
			VboManager::getSingleton()->addVbo("2", old);
			VboManager::getSingleton()->mergeDown();
			OctreeHandler::writeOctreeVbo(*VboManager::getSingleton()->getVbo("1"), fs::path(path));
			VboManager::getSingleton()->clear();
		}
	}	// are we at a nonMaxLevel leaf?
	else if (isLeaf()) {
		// merge, test and write or subdivide
		Vbo* old = 0;
		old = OctreeHandler::readOctreeVbo(fs::path(path));
		// if there is already a saved object present.
		if (old == 0){
			FileIO::recursiveTestAndSet(path);
			OctreeHandler::writeOctreeVbo(_vbo, fs::path(path));
		}
		else {	// if so, load it, merge it and test the new object on condition
			Vbo* originalVbo = new Vbo(_vbo);
			VboManager::getSingleton()->addVbo("1", originalVbo);
			VboManager::getSingleton()->addVbo("2", old);
			VboManager::getSingleton()->mergeDown();
			if (testCondition(*VboManager::getSingleton()->getVbo("1"))){
				OctreeHandler::writeOctreeVbo(*VboManager::getSingleton()->getVbo("1"), fs::path(path));
				VboManager::getSingleton()->clear();
			}
			else {	// if the condition does not hold - subdivide and delete old file.
				Vbo temp(*VboManager::getSingleton()->getVbo("1"));
				VboManager::getSingleton()->clear();
				subdivide(temp);
				fs::path p(path);
				p /= "vArray.oct";
				fs::remove(p);
				p = fs::path(path);
				p /= "nArray.oct";
				fs::remove(p);
			}
		}
	}
	else {	// if it's not a leaf, we don't have any objects here and have to subdivide anyway.
		subdivide(_vbo);

	}
	return false;
}

bool
OctreeNode::subdivide(ooctools::Vbo& _vbo)
{
	OctreeNode* temp;

	// Front North East //////////////////////////////////////////
	// for each child....
	if (fne == 0) {
		fne = new OctreeNode(this, subdivideFne(), path + "/fne");
	}
	temp = fne;
	cout << temp->getBb().toString()  << endl;

	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
//		cout << temp->getBb().toString() << "  ----->   " << _vbo.getBb().toString() << endl;
//		exit(0);
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Front North West //////////////////////////////////////////
	if (fnw == 0) {
		fnw = new OctreeNode(this, subdivideFnw(), path + "/fnw");
	}
	temp = fnw;
	cout << temp->getBb().toString()  << endl;

	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Front South East //////////////////////////////////////////
	if (fse == 0) {
		fse = new OctreeNode(this, subdivideFse(), path + "/fse");
	}
	temp = fse;
	cout << temp->getBb().toString()  << endl;

	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Front South West //////////////////////////////////////////
	if (fsw == 0) {
		fsw = new OctreeNode(this, subdivideFsw(), path + "/fsw");
	}
	temp = fsw;
	cout << temp->getBb().toString()  << endl;
	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Back North East //////////////////////////////////////////
	// for each child....
	if (bne == 0) {
		bne = new OctreeNode(this, subdivideBne(), path + "/bne");
	}
	temp = bne;
	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Back North West //////////////////////////////////////////
	if (bnw == 0) {
		bnw = new OctreeNode(this, subdivideBnw(), path + "/bnw");
	}
	temp = bnw;
	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Back South East //////////////////////////////////////////
	if (bse == 0) {
		bse = new OctreeNode(this, subdivideBse(), path + "/bse");
	}
	temp = bse;
	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}

	// Back South West //////////////////////////////////////////
	if (bsw == 0) {
		bsw = new OctreeNode(this, subdivideBsw(), path + "/bsw");
	}
	temp = bsw;
	if (temp->getBb().hasInside(_vbo.getBb())) {

		// do something
		temp->insertVbo(_vbo);
		return true;
		// break up
	}
	else if (temp->getBb().intersects(_vbo.getBb())) {
		Vbo in, out;
		_vbo.split(temp->getBb(), in, out);
		_vbo = out;
		if (in.getVData()->size > 0)
			temp->insertVbo(in);
	}
	return true;
}

//TODO implement
/**
 * @brief This will check for the node-criteria (ie. are there >= 100 faces in one node).
 * I'm using a VBO because all the merging and splitting is done in the VboManager. Hmmm...
 * maybe not an optimal solution...... need to think about this more.
 * @param _vbo The Vbo which will be checked.
 * @return Whether the condition holds or not.
 * @todo this method has been implemented yet. It returns always false.
 */
bool
OctreeNode::testCondition(const Vbo& _vbo)
{
	return false;
}

void
OctreeNode::drawBbs()
{
	if(isLeaf()){
		mPriBb.draw(0,0,255);
	}
	else {
		if (fne !=0)
			fne->drawBbs();
		if (fnw !=0)
			fnw->drawBbs();
		if (fse !=0)
			fse->drawBbs();
		if (fsw !=0)
			fsw->drawBbs();
		if (bne !=0)
			bne->drawBbs();
		if (bnw !=0)
			bnw->drawBbs();
		if (bse !=0)
			bse->drawBbs();
		if (bsw !=0)
			bsw->drawBbs();
	}
}

} // ooctools
