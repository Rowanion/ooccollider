/**
 * @file	VirtualNode.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 06.11.2009
 * @brief 	VirtualNode class definition.
 */

#include "VirtualNode.h"

using namespace std;
namespace oocframework {

std::map<int, VirtualNode*> VirtualNode::mPriNodeMap = std::map<int, VirtualNode*>();

VirtualNode::VirtualNode(int _rank) {
	mPriRank = _rank;
	VirtualNode::mPriNodeMap.insert(make_pair(_rank, this));
	// TODO Auto-generated constructor stub

}

VirtualNode::~VirtualNode() {
	// TODO Auto-generated destructor stub
	std::map<int, VirtualNode*>::iterator nodeMapIt;
	nodeMapIt = VirtualNode::mPriNodeMap.find(mPriRank);
	if (nodeMapIt!=VirtualNode::mPriNodeMap.end()){
		VirtualNode::mPriNodeMap.erase(nodeMapIt);
	}
}

void VirtualNode::registerRequest(VirtualRequest* _vReq)
{
	mPriReqSet.insert(*_vReq);
}

void VirtualNode::unregisterRequest(VirtualRequest* _vReq)
{
	set<VirtualRequest>::iterator reqSetIt;
	reqSetIt = mPriReqSet.find(*_vReq);
	if (reqSetIt != mPriReqSet.end()){
		mPriReqSet.erase(reqSetIt);
	}
}

VirtualNode* VirtualNode::getNode(int _rank)
{
	std::map<int, VirtualNode*>::iterator nodeMapIt;
	nodeMapIt = VirtualNode::mPriNodeMap.find(_rank);
	if (nodeMapIt!=VirtualNode::mPriNodeMap.end()){
		return nodeMapIt->second;
	}
	return 0;
}

bool VirtualNode::operator<(const VirtualNode* _rhs) const
{
	return mPriRank < _rhs->getRank();
}

} // oocframework
