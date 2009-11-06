/**
 * @file	VirtualRequest.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 06.11.2009
 * @brief 	VirtualRequest class definition.
 */

#include "VirtualRequest.h"

using namespace std;
namespace oocframework {

VirtualRequest::VirtualRequest() {
	// TODO Auto-generated constructor stub

}

VirtualRequest::~VirtualRequest() {
	// TODO Auto-generated destructor stub
}

void VirtualRequest::registerNode(VirtualNode* _vNode)
{
	mPriNodeSet.insert(_vNode);
}

void VirtualRequest::unregisterNode(VirtualNode* _vNode)
{
	set<VirtualNode>::iterator nodeSetIt;
	nodeSetIt = mPriNodeSet.find(_vNode);
	if (nodeSetIt != mPriNodeSet.end()){
		mPriNodeSet.erase(nodeSetIt);
	}
}

void VirtualRequest::reset(uint64_t _id, int _destRank, std::set<int>* _relevantNodes)
{
	mPriTag = false;
	mPriId = _id;
	mPriDestRank = _destRank;
	set<int>::iterator rankIt = _relevantNodes->begin();
	for (; rankIt != _relevantNodes->end(); ++rankIt){
		mPriNodeSet.insert(*(VirtualNode::getNode(*rankIt)));
		VirtualNode::getNode(*rankIt);
	}

}

bool VirtualRequest::operator<(const VirtualRequest& _rhs) const
{
	if (mPriDestRank == mPriDestRank){
		return mPriDestRank < _rhs.getDestRank();
	}
	else return mPriId < _rhs.getId();
}

} // oocframework
