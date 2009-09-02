/**
 * @file	LooseProcessingOctree.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 23.04.2009
 *
 */

#include "LooseProcessingOctree.h"

#include <string>

#include "declarations.h"
#include "BoundingBox.h"
#include "StructDefs.h"

using namespace std;
using namespace ooctools;
namespace oocformats {

LooseProcessingOctree::LooseProcessingOctree(LooseProcessingOctree* _father,
		const BoundingBox& _bb, string _path) :
	mBb(_bb), mExtBb(extendBb(_bb)), path(_path), nTriangles(0), mPriRoot(this),
			mPriLevel(0), father(_father), fne(0), fnw(0), fse(0), fsw(0), bne(0),
			bnw(0), bse(0), bsw(0)
{
	if (father != 0) {
		mPriLevel = father->getLevel() + 1;
		mPriRoot = father->getRoot();
	}

}

LooseProcessingOctree::~LooseProcessingOctree()
{
	unsigned size = mObjectList.size();
	std::vector<ProcessingObject*>::iterator startIt;
	for (unsigned i=0; i< size; ++i){
		startIt = mObjectList.begin();
		delete *startIt;
		mObjectList.erase(startIt);
	}
	mObjectList.clear();

	std::vector<ProcessingObject*> mObjectList;
	if (isLeaf()){

	}
	else {
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

bool
LooseProcessingOctree::isRoot()
{
	if (father == 0)
		return true;
	else
		return false;
}

unsigned char
LooseProcessingOctree::calculateLevel(unsigned char counter)
{
	if (isRoot())
		return counter;
	else
		return father->calculateLevel(counter + 1);
}

bool
LooseProcessingOctree::isLeaf()
{
	return !(fne || fnw || fse || fsw || bne || bnw || bse || bsw);
}

bool
LooseProcessingOctree::isEmpty()
{
	return (fne || fnw || fse || fsw || bne || bnw || bse || bsw);
}

BoundingBox
LooseProcessingOctree::subdivideFne()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMax().getX(),
			mBb.getMax().getY(), mBb.getMin().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideFnw()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMin().getX(),
			mBb.getMax().getY(), mBb.getMin().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideFse()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMax().getX(),
			mBb.getMin().getY(), mBb.getMin().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideFsw()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMin().getX(),
			mBb.getMin().getY(), mBb.getMin().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideBne()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMax().getX(),
			mBb.getMax().getY(), mBb.getMax().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideBnw()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMin().getX(),
			mBb.getMax().getY(), mBb.getMax().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideBse()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMax().getX(),
			mBb.getMin().getY(), mBb.getMax().getZ()));
}

BoundingBox
LooseProcessingOctree::subdivideBsw()
{
	V3f center;
	mBb.computeCenter(center);
	return BoundingBox(center, V3f(mBb.getMin().getX(),
			mBb.getMin().getY(), mBb.getMax().getZ()));
}

/**
 * @todo test
 */
bool
LooseProcessingOctree::insertObject(ProcessingObject* _po)
{
	if (mPriLevel >= MAX_PROCESSING_OCTREE_LEVEL){ // <- write down anyway
		mObjectList.push_back(_po);
		nTriangles+=_po->triangleCount;
		return true;
	}
	else if (!isLeaf()){ // <- there are children - only big objects stay here
		if (fne->fitsInto(_po)){
			fne->insertObject(_po);
		}
		else if (fnw->fitsInto(_po)){
			fnw->insertObject(_po);
		}
		else if (fse->fitsInto(_po)){
			fse->insertObject(_po);
		}
		else if (fsw->fitsInto(_po)){
			fsw->insertObject(_po);
		}
		else if (bne->fitsInto(_po)){
			bne->insertObject(_po);
		}
		else if (bnw->fitsInto(_po)){
			bnw->insertObject(_po);
		}
		else if (bse->fitsInto(_po)){
			bse->insertObject(_po);
		}
		else if (bsw->fitsInto(_po)){
			bsw->insertObject(_po);
		}
		else{	// <- object is too big - it has to stay
			mObjectList.push_back(_po);
			nTriangles+=_po->triangleCount;
		}
		return true;
	}
	else if (conditionHolds(_po->triangleCount)){
		mObjectList.push_back(_po);
		nTriangles+=_po->triangleCount;
		return true;
	}
	else { // <- we're a leaf
		makeChildren();
		mObjectList.push_back(_po);
		unsigned size = mObjectList.size();
		vector<ProcessingObject*>::iterator startIt;
		for(unsigned i=0; i<size; ++i){
			startIt = mObjectList.begin();

			if (fne->fitsInto(*startIt)) {
				fne->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (fnw->fitsInto(*startIt)) {
				fnw->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (fse->fitsInto(*startIt)) {
				fse->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (fsw->fitsInto(*startIt)) {
				fsw->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (bne->fitsInto(*startIt)) {
				bne->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (bnw->fitsInto(*startIt)) {
				bnw->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (bse->fitsInto(*startIt)) {
				bse->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}
			else if (bsw->fitsInto(*startIt)) {
				bsw->insertObject(*startIt);
				nTriangles -= (*startIt)->triangleCount;
				mObjectList.erase(startIt);
			}

		}
		return true;
		//insertObject(_po); // <-- we're not a leaf anymore - this saves us code.
	}

}

bool
LooseProcessingOctree::conditionHolds(unsigned int addOn)
{
	if (nTriangles+addOn > MAX_PROCESSING_OCTREE_VERTICES)
		return false;
	else return true;
}

/**
 * @todo test
 */
const BoundingBox
LooseProcessingOctree::extendBb(const BoundingBox& _bb) const
{
	V3f edges;
	_bb.computeEdgeSizes(edges);
	edges *= 0.5f;
	return BoundingBox(_bb.getMin()-edges, _bb.getMax()+edges);
}

/**
 * @todo test!
 */
bool
LooseProcessingOctree::fitsInto(ProcessingObject* _po) const
{
	if (mBb.hasInside(_po->bb)){
		return true;
	}
	return false;
}

//bool
//LooseProcessingOctree::fitsInto(ProcessingObject* _po) const
//{
//	// if center is in orig bb and bb is inside ext bb return true;
//	V3f center;
//	_po->bb.computeCenter(center);
//	if (mBb.hasInside(center)){
//		if (mExtBb.hasInside(_po->bb))
//			return true;
//	}
//	return false;
//}
/**
 * @todo TEST!
 */
void LooseProcessingOctree::makeChildren()
{
	V3f center;
	mBb.computeCenter(center);

	fne = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMax().getX(), mBb.getMax().getY(), mBb.getMin().getZ())),
			string(path + "/fne"));
	fnw = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMin().getX(), mBb.getMax().getY(), mBb.getMin().getZ())),
			string(path + "/fnw"));
	fse = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMax().getX(), mBb.getMin().getY(), mBb.getMin().getZ())),
			string(path + "/fse"));
	fsw = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMin().getX(), mBb.getMin().getY(), mBb.getMin().getZ())),
			string(path + "/fsw"));
	bne = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMax().getX(), mBb.getMax().getY(), mBb.getMax().getZ())),
			string(path + "/bne"));
	bnw = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMin().getX(), mBb.getMax().getY(), mBb.getMax().getZ())),
			string(path + "/bnw"));
	bse = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMax().getX(), mBb.getMin().getY(), mBb.getMax().getZ())),
			string(path + "/bse"));
	bsw = new LooseProcessingOctree(this, BoundingBox(center, V3f(
			mBb.getMin().getX(), mBb.getMin().getY(), mBb.getMax().getZ())),
			string(path + "/bsw"));
}

/**
 * @todo TEST
 */
void
LooseProcessingOctree::setBasePath(const string& _path, int oldSize)
{
	if (oldSize == -1)
		oldSize = path.size();
	path = _path + path.substr(oldSize);
	if (!isLeaf()){
		fne->setBasePath(_path, oldSize);
		fnw->setBasePath(_path, oldSize);
		fse->setBasePath(_path, oldSize);
		fsw->setBasePath(_path, oldSize);
		bne->setBasePath(_path, oldSize);
		bnw->setBasePath(_path, oldSize);
		bse->setBasePath(_path, oldSize);
		bsw->setBasePath(_path, oldSize);
	}
}

void
LooseProcessingOctree::generateEmptyTree(unsigned int maxLevel)
{
	if (mPriLevel < maxLevel){
		makeChildren();
		fnw->generateEmptyTree(maxLevel);
		fne->generateEmptyTree(maxLevel);
		fsw->generateEmptyTree(maxLevel);
		fse->generateEmptyTree(maxLevel);
		bnw->generateEmptyTree(maxLevel);
		bne->generateEmptyTree(maxLevel);
		bsw->generateEmptyTree(maxLevel);
		bse->generateEmptyTree(maxLevel);
	}
}

} // ooctools
