/**
 * @file	LooseOctree.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 17.05.2009
 * @file 	LooseOctree class definition.
 *
 */

#include "LooseOctree.h"

#include <string>
#include <iostream>
#include <sstream>

#include "declarations.h"
#include "BoundingBox.h"
#include "StructDefs.h"
#include "Triangle.h"
#include "GeometricOps.h"

using namespace std;
using namespace ooctools;
namespace oocformats {

unsigned LooseOctree::descendantCount[8] = {0,0,0,0,0,0,0,0};
unsigned LooseOctree::maxTriCount = 0;
unsigned LooseOctree::totalTriCount = 0;
unsigned LooseOctree::maxLevel = 0;

LooseOctree::LooseOctree(LooseOctree* _father, const BoundingBox& _bb, int64_t _id) :
	mTriList(std::list<Triangle>()), mBb(_bb), mExtBb(extendBb(_bb)), mPriRoot(this),
			mPriLevel(0), mPriId(_id), mPriTriCount(0), mPriAreaSum(0.0), mPriDataLoaded(false), mFather(_father)
{
	if (mFather != 0) {
		mPriLevel = mFather->getLevel() + 1;
		mPriRoot = mFather->getRoot();
	}
	for(unsigned i=0; i< 8; ++i){
		mChildren[i] = 0;
	}

}

LooseOctree::LooseOctree(void* nodeSkel) :
	mTriList(std::list<Triangle>()), mPriLevel(0), mPriId(0), mPriTriCount(0), mPriAreaSum(0.0), mFather(0)
{
	char* count = (char*)nodeSkel;
	mPriId = *((int64_t*)count);
	count += sizeof(int64_t);
	V3f min((float*)count);
	count += sizeof(float)*3;
	V3f max((float*)count);
	count += sizeof(float)*3;
	mBb = BoundingBox(min, max);
	mExtBb = extendBb(mBb);
	mPriAreaSum = *((float*)count);
	count += sizeof(float);
	mPriTriCount = *((unsigned*)count);
	for(unsigned i=0; i< 8; ++i){
		mChildren[i] = 0;
	}

}

LooseOctree::~LooseOctree()
{
	unsigned size = mTriList.size();
	std::list<Triangle>::iterator startIt;
	for (unsigned i=0; i< size; ++i){
		startIt = mTriList.begin();
//		delete *startIt;
		mTriList.erase(startIt);
	}
	mTriList.clear();

	std::vector<ProcessingObject*> mObjectList;
	if (isLeaf()){

	}
	else {
		for (unsigned i=0; i<8; ++i){
			delete mChildren[i];
			mChildren[i] = 0;
		}
	}
}

bool
LooseOctree::isRoot() const
{
	if (mFather == 0)
		return true;
	else
		return false;
}

bool
LooseOctree::isLeaf() const
{
	for(unsigned i =0; i<8; ++i){
		if (mChildren[i]!=0)
			return false;
	}
	return true;
}

bool
LooseOctree::isEmpty() const
{
	if (isLeaf() && !hasData())
		return true;
	else
		return false;
}

bool
LooseOctree::hasData() const
{
	if (mTriList.size()==0  && mPriTriCount == 0)
		return false;
	else
		return true;
}

void LooseOctree::insertTriList(std::list<Triangle>& triList)
{
	//DEBUG
//	cout << getIdString() << ": TriList before insertion: " << mTriList.size() << endl;
	//DEBUG

	mTriList.insert(mTriList.end(), triList.begin(), triList.end());
//	std::list<Triangle> stayList = std::list<Triangle>();
	//DEBUG
//	cout << getIdString() << ": TriList after insertion: " << mTriList.size() << endl;
	//DEBUG

	if (!conditionHolds()){
		// list to save newly created children for instant bubbling.
		vector<unsigned int> newChildren;

		//DEBUG
		unsigned triCount = 0;
		unsigned triSize = mTriList.size();
		//DEBUG
		// try for each triangle to fit into a child
		TriIter it = mTriList.begin();
		while(it != mTriList.end()){
			triCount++;

			if (mPriId == 1005 && triCount%1000 == 0){
//				cout << getIdString() << ": TriCount: " << triCount << "/" << mTriList.size() << endl;
			}
			for(unsigned i=0; i<8; ++i){
				// fill list of new children - check later if anything went into them
				if(makeChild(i)){
					newChildren.push_back(i);
				}
				if(mChildren[i]->fitsInto(*it)){
					mChildren[i]->pushTriangle(*it);
					// delete tri
					it = mTriList.erase(it);
//					++it;
					break;
				}
//				else{
//					++it;
//				}
				if (i == 7){
					++it;
					if (mPriId == 7){
						//					stayList.push_back(*it);
						//DEBUG
						V3f triCenter;
						cout << "TriCount: " << triCount << endl;
						cout << "eval of fits into 1: " << mChildren[1]->fitsInto(*it) << endl;
						it->getBb().computeCenter(triCenter);
						cout << "Tri-BB:  " << it->getBb().toString() << endl;
						cout << "Tri-BBCenter:  " << triCenter.toString() << endl;
						cout << "boxes of children (check for center-accordance): " << endl;
						for(unsigned i=0; i<8; ++i){
							cout << "child " << i << " BB: " << mChildren[i]->getBb().toString() << endl;
						}
						cout << "-------------------------------------------------------" << endl;
						cout << "ExBB von CHild 1: " << mChildren[1]->getExtBb().toString() << endl;
						cout << "triangle stays in node - it doesn't fit anywhere." << endl;
						cout << "Node-BoundingBox:  " << mBb.toString() << endl;
						cout << "Node-ExtBoundingBox:  " << mExtBb.toString() << endl;
						exit(0);
					}
					//DEBUG

				}
			}
		}

		// now we have fitted everything that fits - now clean up!
//		mTriList.clear();
//		mTriList.insert(mTriList.end(), stayList.begin(), stayList.end());
		//remove empty children - slim is beautiful
		vector<unsigned int>::iterator cIt = newChildren.begin();
		while(cIt!= newChildren.end()){
			if (mChildren[*cIt]->isEmpty()){
				delete mChildren[*cIt];
				mChildren[*cIt] = 0;
				cIt = newChildren.erase(cIt);
			}
			else {
				mChildren[*cIt]->subdivideData();
				++cIt;
			}
		}
	}
	calcAreaSum();
}

void LooseOctree::insertTriangle(Triangle& tri)
{
	mTriList.push_back(tri);
	mPriAreaSum += tri.getArea();


	//DEBUG
//	cout << "inserting. currentsize: " << mTriList.size() << endl;
	//DEBUG
	if (!conditionHolds()){
		// list so safe triangle iterators for later deletion
		unsigned nTris = mTriList.size();
		int64_t nodeId = mPriId;
		vector<TriIter> deleteList;
		// make children if needed
		makeChildren();
		// try for each triangle to fit into a child
		for(TriIter it = mTriList.begin(); it != mTriList.end(); ++it ){
			for(unsigned i=0; i<8; ++i){
				if(mChildren[i]->fitsInto(*it)){
					mChildren[i]->insertTriangle(*it);
					// mark tri for deletion
					deleteList.push_back(it);
					break;
				}
			}
		}

		// now we have fitted everything that fits - now clean up!
		for(vector<TriIter>::iterator it = deleteList.begin(); it != deleteList.end(); ++it ){
			mTriList.erase(*it);
		}
		deleteList.clear();

		//remove empty children - slim is beautiful
		for(unsigned i=0; i<8; ++i){
			if (mChildren[i]->isEmpty()){
				delete mChildren[i];
				mChildren[i] = 0;
			}
		}
		calcAreaSum();
	}
}

bool LooseOctree::conditionHolds() const
{
	unsigned size;
	(mTriList.size() > mPriTriCount) ? size =  mTriList.size() : size= mPriTriCount;
	if (size > MAX_OCTREE_TRIANGLES && mPriLevel < MAX_OCTREE_LEVEL){
		return false;
	}
	else return true;
}

void LooseOctree::trimSubTreeData()
{
	mPriTriCount = mTriList.size();
	mTriList.clear();
	for(unsigned i=0; i<8; ++i){
		if (mChildren[i]!=0)
			mChildren[i]->trimSubTreeData();
	}

}

void LooseOctree::makeChildren()
{
	V3f center;
	mBb.computeCenter(center);

	for(unsigned i=0; i< 8; ++i){
		if (mChildren[i]==0) {
			mChildren[i]= new LooseOctree(this, subdivideBB(i), (mPriId*8)+1+i);
		}
	}
}

bool LooseOctree::fitsInto(const Triangle& tri) const
{
	// if center is in orig bb and bb is inside ext bb return true;
	V3f center;
	tri.getBb().computeCenter(center);

	//DEBUG
//	V3f exBBEd;
//	V3f bbEd;
//	V3f triBBEd;
//	mBb.computeEdgeSizes(bbEd);
//	mExtBb.computeEdgeSizes(exBBEd);
//	tri.getBb().computeEdgeSizes(triBBEd);
//	cout << "tribb: " << tri.getBb().toString() << endl;
//	cout << "nodebb: " << mBb.toString() << endl;
//	cout << "exnodebb: " << mExtBb.toString() << endl;
//	cout << "center: " << center.toString() << endl;
//	cout << "eval1: " << (mBb.hasInside(center)) << endl;
	//DEBUG


	if (mBb.hasInside(center)){
		if (mExtBb.hasInside(tri.getBb()))
			return true;
	}
	return false;
}

const BoundingBox
LooseOctree::extendBb(const BoundingBox& _bb) const
{
	V3f edges;
	_bb.computeEdgeSizes(edges);
	edges *= 0.5f;
	return BoundingBox(_bb.getMin()-edges, _bb.getMax()+edges);
}

std::string LooseOctree::getIdString()const
{
	stringstream ss;
	ss << mPriId;
	return ss.str();
}

void LooseOctree::calcAreaSum()
{
	mPriAreaSum = 0.0;
	for(CTriIter it=mTriList.begin(); it != mTriList.end(); ++it){
		mPriAreaSum+=it->getArea();
	}
}

std::size_t LooseOctree::getTriangleCount() const
{
	return (mTriList.size() > mPriTriCount) ? mTriList.size() : mPriTriCount;
}

void LooseOctree::setTriList(std::list<Triangle> triList)
{
	if(mTriList.size() < 1){
		mTriList.clear();
	}
	mTriList = triList;
	calcAreaSum();
}

void LooseOctree::setChildren(LooseOctree* children[])
{
	if (!isLeaf()){
		for (unsigned i =0; i<8; ++i){
			delete mChildren[i];
			mChildren[i] = children[i];
		}
	}
}

void LooseOctree::insertNode(LooseOctree* node)
{
	if ((mPriId+1)*8>= node->getId()){
		mChildren[(node->getId()-(mPriId*8))-1] = node;
		node->setFather(this);
		node->setLevel(mPriLevel+1);
		node->setRoot(mPriRoot);
		LooseOctree::maxLevel = (LooseOctree::maxLevel < node->getLevel()) ? node->getLevel() : LooseOctree::maxLevel;
		LooseOctree::maxTriCount = (LooseOctree::maxTriCount < node->getTriangleCount()) ? node->getTriangleCount() : LooseOctree::maxTriCount;
		LooseOctree::totalTriCount += node->getTriangleCount();
		LooseOctree::descendantCount[(node->getId()-(mPriId*8))-1] ++;
	}
	else{
		V3f bbCenter = V3f();
		node->getBb().computeCenter(bbCenter);
		for(unsigned i=0; i<8; ++i){
			if (makeChild(i)){
				if(mChildren[i]->getBb().hasInside(bbCenter)){
					mChildren[i]->insertNode(node);
					break;
				}
				else {
					delete mChildren[i];
					mChildren[i] = 0;
				}

			}
			else if(mChildren[i]->getBb().hasInside(bbCenter)){
				mChildren[i]->insertNode(node);
				break;
			}
		}
	}
}

void LooseOctree::getNodeSkeleton(void* nodeSkel)
{
	char* counter = (char*)nodeSkel;
//	for(int i= -1; i<8; ++i){
//		if (i<0){
			*((int64_t*)counter) = mPriId;
//		}
//		else {
//			*((int64_t*)counter) = mChildren[i]->getId();
//		}
		counter+=sizeof(int64_t);
//	}
	*((float*)counter) = mBb.getMin().getX();
	counter+=sizeof(float);
	*((float*)counter) = mBb.getMin().getY();
	counter+=sizeof(float);
	*((float*)counter) = mBb.getMin().getZ();
	counter+=sizeof(float);
	*((float*)counter) = mBb.getMax().getX();
	counter+=sizeof(float);
	*((float*)counter) = mBb.getMax().getY();
	counter+=sizeof(float);
	*((float*)counter) = mBb.getMax().getZ();
	counter+=sizeof(float);
	*((float*)counter) = mPriAreaSum;
	counter+=sizeof(float);
	*((unsigned*)counter) = getTriangleCount();
	counter+=sizeof(unsigned);

}

unsigned LooseOctree::getSkeletonSize()
{
	// id + 8*children ids, BBm areasum, triCount
//	cout << "Skel-Size: " << (sizeof(int64_t)*1) + (6*sizeof(float)) + sizeof(float) + sizeof(unsigned) << endl;
	return (sizeof(int64_t)*1) + (6*sizeof(float)) + sizeof(float) + sizeof(unsigned);
}

BoundingBox
LooseOctree::subdivideBB(unsigned idx)
{
	V3f center;
	mBb.computeCenter(center);
	BoundingBox newBB;
	switch (idx){
	case 0: // fne
		newBB = BoundingBox(center, V3f(mBb.getMax().getX(),
					mBb.getMax().getY(), mBb.getMin().getZ()));
		break;
	case 1: // fnw
		newBB = BoundingBox(center, V3f(mBb.getMin().getX(),
				mBb.getMax().getY(), mBb.getMin().getZ()));
		break;
	case 2: // fse
		newBB = BoundingBox(center, V3f(mBb.getMax().getX(),
				mBb.getMin().getY(), mBb.getMin().getZ()));
		break;
	case 3: // fsw
		newBB = BoundingBox(center, V3f(mBb.getMin().getX(),
				mBb.getMin().getY(), mBb.getMin().getZ()));
		break;
	case 4: // bne
		newBB = BoundingBox(center, V3f(mBb.getMax().getX(),
				mBb.getMax().getY(), mBb.getMax().getZ()));
		break;
	case 5: // bnw
		newBB = BoundingBox(center, V3f(mBb.getMin().getX(),
				mBb.getMax().getY(), mBb.getMax().getZ()));
		break;
	case 6: // bse
		newBB = BoundingBox(center, V3f(mBb.getMax().getX(),
				mBb.getMin().getY(), mBb.getMax().getZ()));
		break;
	case 7: // bsw
		newBB = BoundingBox(center, V3f(mBb.getMin().getX(),
				mBb.getMin().getY(), mBb.getMax().getZ()));
		break;
	}
	return newBB;
}

void
LooseOctree::pushTriList(std::vector<Triangle>& triList)
{
	mTriList.insert(mTriList.end(), triList.begin(), triList.end());
	calcAreaSum();
}

void LooseOctree::pushTriangle(Triangle& tri)
{
	mTriList.push_back(tri);
	mPriAreaSum += tri.getArea();
}

bool LooseOctree::makeChild(unsigned int idx)
{
	if (mChildren[idx] == 0){
		mChildren[idx] = new LooseOctree(this, subdivideBB(idx),(mPriId*8)+1+idx);
		return true;
	}
	else return false;
}

void LooseOctree::subdivideData()
{
	if (!conditionHolds()){
		makeChildren();
		TriIter it = mTriList.begin();
		while (it != mTriList.end()){
			for (unsigned i=0; i< 8; ++i){
				if (mChildren[i]->fitsInto(*it)){
					mChildren[i]->pushTriangle(*it);
					it = mTriList.erase(it);
					break;
				}
				else {
					++it;
				}
			}
		}

		// remove empty children and further subdivision of others
		for (unsigned i=0; i< 8; ++i){
			if (mChildren[i]->isEmpty()){
				delete mChildren[i];
				mChildren[i] = 0;
			}
			else {
				mChildren[i]->subdivideData();
			}
		}
	}
}

void LooseOctree::addTriangleArea(double area)
{
	mPriAreaSum+= area;
}

void LooseOctree::incTriCount(unsigned count)
{
	mPriTriCount += count;
}

void LooseOctree::drawLeafBBs()
{
	if(isRoot()){
		cout << "Total number of tris: " << LooseOctree::totalTriCount << endl;
		LooseOctree::totalTriCount = 0;
	}
	LooseOctree::totalTriCount += getTriangleCount();

	if (isLeaf()){
		mBb.draw();
	}
//	else if ( !isEmpty()){
//		mBb.draw();
//		for (unsigned i=0; i<8; ++i){
//			if (mChildren[i]!=0){
//				mChildren[i]->drawLeafBBs();
//			}
//		}
//	}
	else {
		for (unsigned i=0; i<8; ++i){
			if (mChildren[i]!=0){
				mChildren[i]->drawLeafBBs();
			}
		}
	}
}

LooseOctree*
LooseOctree::getChild(unsigned i)
{
	LooseOctree* retVal;
	(i>7) ? retVal= this : retVal= mChildren[i];
	return retVal;
}

const LooseOctree*
LooseOctree::getChild(unsigned i) const
{
	const LooseOctree* retVal;
	(i>7) ? retVal= this : retVal= mChildren[i];
	return retVal;
}

unsigned
LooseOctree::findInsertionPoint(const Triangle& tri)
{
	makeChildren();
	unsigned returnId=8;
	for(unsigned i=0; i<8; ++i){
		if(mChildren[i]->fitsInto(tri)){
			returnId = i;
			mChildren[i]->incTriCount();
			break;
		}
	}

	//remove empty children - slim is beautiful
	for(unsigned i=0; i<8; ++i){
		if (mChildren[i]->isEmpty()){
			delete mChildren[i];
			mChildren[i] = 0;
		}
	}

	return returnId;
}

std::string LooseOctree::getDirPrefix(unsigned level) const
{
	if (isRoot())
		return "";
	else if (level>0)
		return getChildType()+"/";
	else
		return getFather()->getDirPrefix(level+1) + getChildType()+"/";
}

std::string LooseOctree::getChildType() const
{
	if (isRoot()){
		return "";
	}
	else {
		stringstream ss;
		ss << (mPriId - (getFather()->getId()*8)-1);
		return ss.str();
	}
}

void LooseOctree::printNodePath(int64_t id) const
{
	if (mPriId == id){
		cout << getChildType() << "/";
		LooseOctree* node = mFather;
		while(!node->isRoot()){
			cout << node->getChildType() << "/";
			node = node->getFather();
		}
		cout << endl << "back to front" << endl;
	}
	else {
		for (unsigned i=0; i<8; ++i){
			if (mChildren[i]!=0){
				mChildren[i]->printNodePath(id);
			}
		}

	}
}




void LooseOctree::isInFrustum_orig(float** _frustum, std::set<uint64_t>* _ids) {
	int aPosCounter = 0, aTotalIn = 0, aIPtIn = 0;

	for (unsigned int p = 0; p < 6; ++p) {
		aPosCounter = 8;
		aIPtIn = 1;
		if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
				this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
				this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
				this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
				this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
				this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
				this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
				this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
				this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
				_frustum[p][3] < -0) {
			aPosCounter--;
			aIPtIn = 0;
		}
		if (aPosCounter == 0) //exclude
		return;
		else
			aTotalIn += aIPtIn;
	}

	if (this->hasData()) {
		_ids->insert(this->mPriId);
	}


	if (aTotalIn == 6) { //include
		//vollständig drin
	}
	// kinder weiter testen

	for (unsigned i = 0; i < 8; i++) {  //intersect
		if (this->mChildren[i] != 0) {
			(this->mChildren[i])->isInFrustum_orig(_frustum, _ids);
		}
	}
}

void LooseOctree::isInFrustum(float** _frustum, std::set<uint64_t>*
		_ids, bool _showOctree, unsigned* _threshold, bool debug) {
	int aPosCounter = 0, aTotalIn = 0, aIPtIn = 0;

	if (!debug || mPriLevel>3){
		for (unsigned int p = 0; p < 6; ++p) {
			aPosCounter = 8;
			aIPtIn = 1;
			if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
					this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
					this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
					this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
					this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMin().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
					this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
					this->mExtBb.getMin().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMin().getX() + _frustum[p][1] *
					this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (_frustum[p][0] * this->mExtBb.getMax().getX() + _frustum[p][1] *
					this->mExtBb.getMax().getY() + _frustum[p][2] * this->mExtBb.getMax().getZ() +
					_frustum[p][3] < -0) {
				aPosCounter--;
				aIPtIn = 0;
			}
			if (aPosCounter == 0) //exclude
				return;
			else
				aTotalIn += aIPtIn;
		}
	}

	if (this->hasData()) {
		_ids->insert(mPriId);
	}


	if (aTotalIn == 6) { //include
		debug = true;
		//completely inside
	}
	// further tesing of children

	for (unsigned i = 0; i < 8; i++) {  //intersect
		if (this->mChildren[i] != 0) {
			(this->mChildren[i])->isInFrustum(_frustum, _ids, _showOctree,
					_threshold, debug);
		}
	}
}

/*
 * OctreeNode written to disk:
 * [0;1,2,3,4,5,6,7,8,(x_min, y_min, z_min),(x_max, y_max, z_max)]
 * [1;9,10,11,12,13,14,15,16,(x_min, y_min, z_min),(x_max, y_max, z_max)]
 * [2;65,66,67,68,69,70,71,72,(x_min, y_min, z_min),(x_max, y_max, z_max)]
 * if a child idx is negative, there is no child....? right?!
 * data will be loaded if an existence-check holds true on the node-id
 * at filesystem level.
 *
 */

} // ooctools
