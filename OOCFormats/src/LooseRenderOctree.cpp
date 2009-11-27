/**
 * @file	LooseRenderOctree.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 17.05.2009
 * @file 	LooseRenderOctree class definition.
 *
 */

#include "LooseRenderOctree.h"

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

unsigned LooseRenderOctree::descendantCount[8] = {0,0,0,0,0,0,0,0};
unsigned LooseRenderOctree::maxTriCount = 0;
unsigned LooseRenderOctree::totalTriCount = 0;
unsigned LooseRenderOctree::maxLevel = 0;
unsigned LooseRenderOctree::orderLUT[26][8] = {{0, 1, 2, 3, 4, 5, 6, 7}, {4, 5, 6, 7, 0, 1, 2, 3}, {0, 1, 4, 5, 2, 3, 6, 7},
		{2, 3, 6, 7, 0, 1, 4, 5}, {1, 3, 5, 7, 0, 2, 4, 6}, {0, 2, 4, 6, 1, 3, 5, 7},

		{3, 7, 2, 1, 6, 5, 0, 4}, {2, 3, 6, 0, 7, 1, 4, 5}, {0, 4, 1, 2, 5, 6, 3, 7}, {1, 0, 5, 3, 4, 2, 7, 6},
		{4, 5, 0, 6, 1, 7, 2, 3}, {5, 1, 4, 7, 0, 3, 6, 2}, {7, 6, 3, 5, 2, 4, 1, 0}, {6, 2, 7, 4, 3, 0, 5, 1},

		{2, 3, 0, 1, 6, 7, 4, 5}, {0, 2, 1, 3, 4, 6, 5, 7}, {0, 1, 4, 5, 2, 3, 6, 7}, {1, 3, 0, 2, 5, 7, 4, 6},
		{4, 5, 0, 1, 6, 7, 2, 3}, {5, 7, 1, 3, 4, 6, 0, 2}, {6, 7, 2, 3, 4, 5, 0, 1}, {4, 6, 5, 7, 0, 2, 1, 3},
		{1, 5, 0, 4, 3, 7, 2, 6}, {3, 7, 1, 5, 2, 6, 0, 4}, {0, 4, 1, 5, 2, 6, 3, 7}, {2, 6, 0, 4, 3, 7, 1, 5}};

unsigned LooseRenderOctree::detailLUT[26][8] = {{14, 14, 14, 14, 7, 7, 7, 7}, {14, 14, 14, 14, 7, 7, 7, 7}, {14, 14, 14, 14, 7, 7, 7, 7},
		{14, 14, 14, 14, 7, 7, 7, 7}, {14, 14, 14, 14, 7, 7, 7, 7}, {14, 14, 14, 14, 7, 7, 7, 7},

		{14, 10, 10, 10, 7, 7, 7, 5}, {14, 10, 10, 10, 7, 7, 7, 5}, {14, 10, 10, 10, 7, 7, 7, 5}, {14, 10, 10, 10, 7, 7, 7, 5},
		{14, 10, 10, 10, 7, 7, 7, 5}, {14, 10, 10, 10, 7, 7, 7, 5}, {14, 10, 10, 10, 7, 7, 7, 5}, {14, 10, 10, 10, 7, 7, 7, 5},

		{14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5},
		{14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5},
		{14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}, {14, 14, 9, 9, 9, 9, 5, 5}};

unsigned LooseRenderOctree::pubTick = 0;
unsigned LooseRenderOctree::pubTickLimit = 0;
unsigned LooseRenderOctree::pubNodeCount = 0;
LooseRenderOctree* LooseRenderOctree::pubRoot = 0;

LooseRenderOctree::LooseRenderOctree(LooseRenderOctree* _father, const BoundingBox& _bb, int64_t _id) :
	mBb(_bb), mExtBb(extendBb(_bb)),
	mPriId(_id), mPriLevel(0), mFather(_father), mPriTriCount(0)
{
	if (mFather != 0) {
		mPriLevel = mFather->getLevel() + 1;
	}
	if (LooseRenderOctree::pubRoot == 0) {
		LooseRenderOctree::pubRoot = this;
	}
	for(unsigned i=0; i< 8; ++i){
		mChildren[i] = 0;
	}
	pubNodeCount++;

}

LooseRenderOctree::LooseRenderOctree(const char* nodeSkel) :
	mBb(0.0), mPriId(0), mPriWrapper(this), mPriDistanceUpdateKey(-1), mPriLevel(0),
	mFather(0), mPriTriCount(0)
{
//	cerr << "constructing LooseRenderOctree" << endl;
	const char* count = nodeSkel;
	mPriId = *((int64_t*)count);
	count += sizeof(int64_t);
	V3f min = V3f((float*)count);
	count += sizeof(float)*3;
	V3f max = V3f((float*)count);
	count += sizeof(float)*3;
//	cerr << "---------------------------------------------------------------" << endl;
//	std::cerr << "Ptr OUT before: " << (uint64_t)mBb.mPriCenter << std::endl;
	mBb = BoundingBox(min, max);
//	std::cerr << "Ptr OUT after: " << (uint64_t)mBb.mPriCenter << std::endl;

//	cerr << "1center: " << mBb.getCenter().toString() << endl;
//	V3f test = V3f(mBb.getCenter());
//	cerr << "2center: " << mBb.getFritz() << endl;
//	cerr << "Testcenter: " << test.toString() << endl;
//	cerr << "extBB from here" << endl;
	mExtBb = extendBb(mBb);
	count += sizeof(float);
	mPriTriCount = *((unsigned*)count);
	for(unsigned i=0; i< 8; ++i){
		mChildren[i] = 0;
	}
	if (LooseRenderOctree::pubRoot == 0) {
		LooseRenderOctree::pubRoot = this;
	}

	pubNodeCount++;

}

LooseRenderOctree::~LooseRenderOctree()
{
	if (!isLeaf()){
		for (unsigned i=0; i<8; ++i){
			delete mChildren[i];
			mChildren[i] = 0;
		}
	}
	delete mPriWrapper.iVbo;
	mPriWrapper.iVbo = 0;
}

bool
LooseRenderOctree::isRoot() const
{
	if (this == LooseRenderOctree::pubRoot)
		return true;
	else
		return false;
}

std::size_t LooseRenderOctree::getTriangleCount() const
{
	return mPriTriCount;
}

bool
LooseRenderOctree::isLeaf() const
{
	for(unsigned i =0; i<8; ++i){
		if (mChildren[i]!=0)
			return false;
	}
	return true;
}

bool
LooseRenderOctree::isEmpty() const
{
	if (isLeaf() && !hasData())
		return true;
	else
		return false;
}

bool
LooseRenderOctree::hasData() const
{
	if (mPriTriCount == 0)
		return false;
	else
		return true;
}

void LooseRenderOctree::makeChildren()
{
	V3f center;
	mBb.computeCenter(center);

	for(unsigned i=0; i< 8; ++i){
		if (mChildren[i]==0) {
			mChildren[i]= new LooseRenderOctree(this, subdivideBB(i), (mPriId*8)+1+i);
		}
	}
}

const BoundingBox
LooseRenderOctree::extendBb(const BoundingBox& _bb) const
{
	V3f edges;
	_bb.computeEdgeSizes(edges);
	edges *= 0.5f;
	return BoundingBox(_bb.getMin()-edges, _bb.getMax()+edges);
}

std::string LooseRenderOctree::getIdString()const
{
	stringstream ss;
	ss << mPriId;
	return ss.str();
}

void LooseRenderOctree::setChildren(LooseRenderOctree* children[])
{
	if (!isLeaf()){
		for (unsigned i =0; i<8; ++i){
			delete mChildren[i];
			mChildren[i] = children[i];
		}
	}
}

void LooseRenderOctree::insertNode(LooseRenderOctree* node)
{
	if ((mPriId+1)*8>= node->getId()){
		mChildren[(node->getId()-(mPriId*8))-1] = node;
		node->setFather(this);
		node->setLevel(mPriLevel+1);
		LooseRenderOctree::maxLevel = (LooseRenderOctree::maxLevel < node->getLevel()) ? node->getLevel() : LooseRenderOctree::maxLevel;
		LooseRenderOctree::maxTriCount = (LooseRenderOctree::maxTriCount < node->getTriangleCount()) ? node->getTriangleCount() : LooseRenderOctree::maxTriCount;
		LooseRenderOctree::totalTriCount += node->getTriangleCount();
		LooseRenderOctree::descendantCount[(node->getId()-(mPriId*8))-1] ++;
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

void LooseRenderOctree::getNodeSkeleton(void* nodeSkel)
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
	counter+=sizeof(float);
	*((unsigned*)counter) = getTriangleCount();
	counter+=sizeof(unsigned);

}

unsigned LooseRenderOctree::getSkeletonSize()
{
	// id + 8*children ids, BBm areasum, triCount
//	cout << "Skel-Size: " << (sizeof(int64_t)*1) + (6*sizeof(float)) + sizeof(float) + sizeof(unsigned) << endl;
	return (sizeof(int64_t)*1) + (6*sizeof(float)) + sizeof(float) + sizeof(unsigned);
}

BoundingBox
LooseRenderOctree::subdivideBB(unsigned idx)
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

bool LooseRenderOctree::makeChild(unsigned int idx)
{
	if (mChildren[idx] == 0){
		mChildren[idx] = new LooseRenderOctree(this, subdivideBB(idx),(mPriId*8)+1+idx);
		return true;
	}
	else return false;
}

void LooseRenderOctree::incTriCount(unsigned count)
{
	mPriTriCount += count;
}

void LooseRenderOctree::drawLeafBBs()
{
	if(isRoot()){
		cout << "Total number of tris: " << LooseRenderOctree::totalTriCount << endl;
		LooseRenderOctree::totalTriCount = 0;
	}
	LooseRenderOctree::totalTriCount += getTriangleCount();

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

LooseRenderOctree*
LooseRenderOctree::getChild(unsigned i)
{
	LooseRenderOctree* retVal;
	(i>7) ? retVal= this : retVal= mChildren[i];
	return retVal;
}

const LooseRenderOctree*
LooseRenderOctree::getChild(unsigned i) const
{
	const LooseRenderOctree* retVal;
	(i>7) ? retVal= this : retVal= mChildren[i];
	return retVal;
}

std::string LooseRenderOctree::getDirPrefix(unsigned level) const
{
	if (isRoot())
		return "";
	else if (level>0)
		return getChildType()+"/";
	else
		return getFather()->getDirPrefix(level+1) + getChildType()+"/";
}

std::string LooseRenderOctree::getChildType() const
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

void LooseRenderOctree::printNodePath(int64_t id) const
{
	if (mPriId == id){
		cout << getChildType() << "/";
		LooseRenderOctree* node = mFather;
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

void
LooseRenderOctree::isInFrustum_bfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, unsigned _limit)
{
	// should be called in root only, or at least from subroot
	std::queue<LooseRenderOctree*> toDoQueue = std::queue<LooseRenderOctree*>();
	frustumSelfTest_bfs(_frustum, _ids, toDoQueue, orderIdx);
	unsigned iterCount = 0;
	bool state;
	while(!toDoQueue.empty() && iterCount<_limit){
		state = toDoQueue.front()->frustumSelfTest_bfs(_frustum, _ids, toDoQueue, orderIdx);
		toDoQueue.pop();
		if (state)
			iterCount++;
	}
}

void
LooseRenderOctree::isInFrustum_dfs(float** _frustum, std::set<uint64_t>* _ids, unsigned orderIdx, unsigned _limit)
{
	// should be called in root only, or at least from subroot
	std::queue<LooseRenderOctree*> toDoQueue = std::queue<LooseRenderOctree*>();
	frustumSelfTest_bfs(_frustum, _ids, toDoQueue, orderIdx);
	unsigned iterCount = 0;
	bool state;

	while(!toDoQueue.empty() && iterCount<_limit){
		state = toDoQueue.front()->frustumSelfTest_bfs(_frustum, _ids, toDoQueue, orderIdx);
		toDoQueue.pop();
		if (state)
			iterCount++;
	}
}

bool
LooseRenderOctree::frustumSelfTest_bfs(float** _frustum, std::set<uint64_t>* _ids, std::queue<LooseRenderOctree*>& _toDoQueue, unsigned orderIdx)
{
	int aPosCounter = 0, aTotalIn = 0, aIPtIn = 0;
	bool added = false;

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
		return false;
		else
			aTotalIn += aIPtIn;
	}

	if (this->hasData()) {
		_ids->insert(this->mPriId);
		added = true;
	}

	// insert non-null children into queue
	for (unsigned i = 0; i < 8; i++) {
		if (this->mChildren[LooseRenderOctree::orderLUT[orderIdx][i]] != 0) {
			_toDoQueue.push(this->mChildren[LooseRenderOctree::orderLUT[orderIdx][i]]);
		}
	}
	return added;
}

void
LooseRenderOctree::isInFrustum_orig(float** _frustum, std::list<WrappedOcNode*>* _nodes, unsigned orderIdx, const V3f& eyePos, const float* distArray, bool _isExt) {
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
		if (aPosCounter == 0){ //exclude

			return;
		}
		else
			aTotalIn += aIPtIn;
	}

	if (this->hasData()) {
		this->mPriWrapper.timeStamp = pubTick;
		if (_isExt && mPriWrapper.state == WrappedOcNode::MISSING){
			_nodes->push_back(&this->mPriWrapper);
		}
		else if (!_isExt && mPriWrapper.state == WrappedOcNode::OFFLINE){
			this->mPriWrapper.state = WrappedOcNode::SET_ONLINE;
		}
		else if (!_isExt && this->mPriWrapper.state == WrappedOcNode::ONLINE) {
			this->mPriWrapper.usageCount++;
		}
	}

	LooseRenderOctree* child = 0;

	if (aTotalIn == 6) { //include
		//completely inside
		for (unsigned i = 0; i < 8; i++) {  //intersect
			child = this->mChildren[LooseRenderOctree::orderLUT[orderIdx][i]];
			if (child != 0) {
				if (child->mPriDistanceUpdateKey<0){
					child->mPriDistanceUpdateKey = child->mPriId % pubTickLimit;
				}
				if (child->mPriWrapper.dist == 0.0 || pubTick % pubTickLimit == mPriDistanceUpdateKey){
					child->mPriWrapper.dist = eyePos.calcSimpleDistance(child->getBb().getCenter());
				}
				if (child->mPriWrapper.dist < distArray[child->getLevel()]){
					child->getAllSubtreeIds(_nodes, orderIdx, eyePos, distArray, _isExt);
				}
				else{
					return;
				}
			}
		}
		return;
	}
	// kinder weiter testen

	for (unsigned i = 0; i < 8; i++) {  //intersect
		child = this->mChildren[LooseRenderOctree::orderLUT[orderIdx][i]];
		if (child != 0) {
//			cerr << "simpledist ";
//			cerr << child->getBb().getCenter().calcSimpleDistance(eyePos) << endl;
//			cout << "child lvl " << child->getLevel() << endl;
//			cout << "dist at level " << distArray[child->getLevel()] << endl;
//			cerr << "leveldist ";
////			exit(0);
//			cout << distArray[child->getLevel()] << endl;
//			V3f blub = eyePos;
//			V3f bla = child->getBb().getCenter();
//			bla -= blub;
//			cout << "magnitude" << bla.calculateMagnitude() << endl;
//			cout << "simple magnitude " << (bla.getX()*bla.getX())+(bla.getY()*bla.getY())+(bla.getZ()*bla.getZ())<< endl;
//			cout << eyePos.calcSimpleDistance(child->getBb().getCenter()) << " vs " << distArray[child->getLevel()] << endl;
			if (child->mPriDistanceUpdateKey<0){
				child->mPriDistanceUpdateKey = child->mPriId % pubTickLimit;
			}
			if (child->mPriWrapper.dist == 0.0 || pubTick % pubTickLimit == mPriDistanceUpdateKey){
				child->mPriWrapper.dist = eyePos.calcSimpleDistance(child->getBb().getCenter());
			}
			if (child->mPriWrapper.dist < distArray[child->getLevel()]){
				child->isInFrustum_orig(_frustum, _nodes, orderIdx, eyePos, distArray, _isExt);
			}
			else{
				return;
			}
		}
	}

}

void LooseRenderOctree::getAllSubtreeIds(std::list<WrappedOcNode*>* _nodes, bool _isExt){
	if (this->hasData()){
		this->mPriWrapper.timeStamp = pubTick;
		if (_isExt && this->mPriWrapper.state == WrappedOcNode::MISSING) {
			_nodes->push_back(&this->mPriWrapper);
		}
		else if (!_isExt && this->mPriWrapper.state == WrappedOcNode::OFFLINE) {
			this->mPriWrapper.state = WrappedOcNode::SET_ONLINE;
		}
		else if (!_isExt && this->mPriWrapper.state == WrappedOcNode::ONLINE) {
			this->mPriWrapper.usageCount++;
		}
	}

	for (unsigned i = 0; i < 8; i++) {
		if (this->mChildren[i] != 0) {
			(this->mChildren[i])->getAllSubtreeIds(_nodes, _isExt);
		}
	}
}

void LooseRenderOctree::getAllSubtreeIds(std::list<WrappedOcNode*>* _nodes, unsigned orderIdx, const V3f& eyePos, const float* distArray, bool _isExt){
	if (this->hasData()){
		this->mPriWrapper.timeStamp = pubTick;
		if (_isExt && this->mPriWrapper.state == WrappedOcNode::MISSING) {
			_nodes->push_back(&this->mPriWrapper);
		}
		else if (!_isExt && this->mPriWrapper.state == WrappedOcNode::OFFLINE) {
			this->mPriWrapper.state = WrappedOcNode::SET_ONLINE;
		}
		else if (!_isExt && this->mPriWrapper.state == WrappedOcNode::ONLINE) {
			this->mPriWrapper.usageCount++;
		}
	}
	LooseRenderOctree* child = 0;
	for (unsigned i = 0; i < 8; i++) {
		child = this->mChildren[LooseRenderOctree::orderLUT[orderIdx][i]];
		if (child != 0) {
			//TODO abhängigkeit von tick to calc dist
			if (child->mPriDistanceUpdateKey<0){
				child->mPriDistanceUpdateKey = child->mPriId % pubTickLimit;
			}
			if (child->mPriWrapper.dist == 0.0 || pubTick % pubTickLimit == mPriDistanceUpdateKey){
				child->mPriWrapper.dist = eyePos.calcSimpleDistance(child->getBb().getCenter());
			}
			if (this->mPriWrapper.dist < distArray[child->getLevel()]){
				child->getAllSubtreeIds(_nodes, _isExt);
			}
			else{
				return;
			}
		}
	}
}

} // oocformats
