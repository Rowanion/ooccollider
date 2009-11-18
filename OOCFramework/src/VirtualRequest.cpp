/**
 * @file	VirtualRequest.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 06.11.2009
 * @brief 	VirtualRequest class definition.
 */

#include "VirtualRequest.h"
#include "MpiControl.h"

using namespace std;
namespace oocframework {

VirtualRequest::VirtualRequest() {
	mPriQuint = 0;

}

VirtualRequest::~VirtualRequest() {
	// TODO Auto-generated destructor stub
}

void VirtualRequest::unregisterAllButThis(VirtualNode* _vNode)
{
	set<VirtualNode*>::iterator nodeIt = mPriNodeSet.begin();
	while (nodeIt!=mPriNodeSet.end()){
		VirtualNode* vNode = *nodeIt;
		if (vNode != _vNode){
			vNode->unregisterRequest(this);
			mPriNodeSet.erase(nodeIt++);
		}
		else {
			++nodeIt;
		}
	}
}

void VirtualRequest::unregisterAll()
{
	set<VirtualNode*>::iterator nodeIt = mPriNodeSet.begin();
	for (; nodeIt!=mPriNodeSet.end(); ++nodeIt){
			(*nodeIt)->unregisterRequest(this);
	}
	mPriNodeSet.clear();
}

unsigned VirtualRequest::imHappyToHelp(VirtualNode* _vNode)
{
	mPriServiceNodeRank = _vNode->getRank();
	unregisterAll();

	return mPriTriCount;
}

//void VirtualRequest::reset(uint64_t _id, int _destRank, unsigned _triCount, std::set<int>* _relevantNodes)
//{
//	mPriTag = false;
//	if (_relevantNodes == 0){
//		mPriId = 0;
//		mPriDestRank = 0;
//		mPriTriCount = 1000000;
//	}
//	else {
//		mPriId = _id;
//		mPriDestRank = _destRank;
//		mPriTriCount = _triCount;
//		set<int>::iterator rankIt = _relevantNodes->begin();
//		for (; rankIt != _relevantNodes->end(); ++rankIt){
//			VirtualNode* vNode = VirtualNode::getNode(*rankIt);
//			vNode->registerRequest(this);
//			mPriNodeSet.insert(vNode);
//		}
//	}
//}

void VirtualRequest::reset(ooctools::Quintuple* _quint, unsigned int _triCount, std::set<int>* _relevantNodes)
{
	// --------------------
	mPriQuint = _quint;
	mPriServiceNodeRank = 0;
	mPriQuint = _quint;
	mPriNodeSet.clear();
	if (mPriQuint == 0){
		mPriTag = true;
		mPriId = 0;
		mPriDestRank = 0;
		mPriTriCount = 1000000;
	}
	else {
		mPriTag = false;
		mPriId = _quint->id;
		mPriDestRank = _quint->destId;
		mPriTriCount = _triCount;
//		cerr << "Request " << mPriId << " resetted with nodes ";
		set<int>::iterator rankIt = _relevantNodes->begin();
		for (; rankIt != _relevantNodes->end(); ++rankIt){
			VirtualNode* vNode = VirtualNode::getNode(*rankIt);
			vNode->registerRequest(this);
			mPriNodeSet.insert(vNode);
//			cerr << vNode->getRank() << ", ";
		}
//		cerr << " and total " << mPriNodeSet.size() << " nodes." << endl;
	}
}

bool VirtualRequest::operator<(const VirtualRequest& _rhs) const
{
	if (mPriDestRank == mPriDestRank){
		return mPriDestRank < _rhs.getDestRank();
	}
	else return mPriId < _rhs.getId();
}

void VirtualRequest::debug()
{
	// to test locally
}
} // oocframework
