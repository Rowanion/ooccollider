/**
 * @file	VirtualNode.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 06.11.2009
 * @brief 	VirtualNode class definition.
 */

#include "VirtualNode.h"

#include <iostream>

using namespace std;
namespace oocframework {

unsigned VirtualNode::priNodeCount = 0;
unsigned VirtualNode::priTotalTriCount = 0;
std::map<int, VirtualNode*>* VirtualNode::priNodeMap = new std::map<int, VirtualNode*>();
unsigned VirtualNode::getNodeCount()
{
	return VirtualNode::priNodeCount;
}

void VirtualNode::registerNode(VirtualNode* _vNode)
{
	VirtualNode::priNodeMap->insert(make_pair(_vNode->mPriRank, _vNode));
	VirtualNode::priTotalTriCount += _vNode->mPriTriCount;
	VirtualNode::priNodeCount++;
}


unsigned VirtualNode::getTotalTriCount()
{
	return VirtualNode::priTotalTriCount;
}

VirtualNode* VirtualNode::getNode(int _rank)
{
	std::map<int, VirtualNode*>::iterator nodeMapIt;
	nodeMapIt = VirtualNode::priNodeMap->find(_rank);
	if (nodeMapIt != VirtualNode::priNodeMap->end()){
		return nodeMapIt->second;
	}
	return 0;
}


void VirtualNode::hardReset()
{
	std::map<int, VirtualNode*>::iterator nodeIt = VirtualNode::priNodeMap->begin();
	for (; nodeIt != VirtualNode::priNodeMap->end(); ++nodeIt){
		nodeIt->second->newTurn();
		nodeIt->second->mPriTriCount = 0;
	}
	VirtualNode::priTotalTriCount = 0;
}

VirtualNode::VirtualNode(int _rank) : mPriTag(false), mPriServiceTag(false)

{
	mPriRank = _rank;
	mPriTriCount = 1;
	mPriReqSet = set<VirtualRequest*>();
	VirtualNode::priNodeCount = VirtualNode::priNodeMap->size();

}

VirtualNode::~VirtualNode()
{
}

void VirtualNode::registerRequest(VirtualRequest* _vReq)
{
	pair<set<VirtualRequest*>::iterator, bool> result;
	result = mPriReqSet.insert(_vReq);
	if (result.second){
		mPriReqCount++;
	}
}

void VirtualNode::unregisterRequest(VirtualRequest* _vReq)
{
	set<VirtualRequest*>::iterator reqSetIt;
	reqSetIt = mPriReqSet.find(_vReq);
	if (reqSetIt != mPriReqSet.end()){
		mPriReqSet.erase(reqSetIt);
		mPriReqCount--;
	}
}

unsigned int VirtualNode::professService()
{
	unsigned requestCount = mPriReqSet.size();
	set<VirtualRequest*>::iterator reqSetIt = mPriReqSet.begin();
	for (; reqSetIt != mPriReqSet.end(); ++reqSetIt){
		unsigned int triCount = (*reqSetIt)->imHappyToHelp(this);
		mPriTriCount+= triCount;
		VirtualNode::priTotalTriCount+= triCount;
	}

	mPriReqSet.clear();
	return requestCount;
}

void VirtualNode::newSubTurn()
{
	mPriTag = false;
	mPriServiceTag = false;
}

void VirtualNode::newTurn()
{
	mPriTag = false;
	mPriServiceTag = false;
	mPriReqSet.clear();
	mPriReqCount = 0;
}

bool VirtualNode::operator<(const VirtualNode& _rhs) const
{

	if (getInverseTriCount() == _rhs.getInverseTriCount()){
		return mPriRank < _rhs.getRank();
	}
	else {
		return getInverseTriCount() < _rhs.getInverseTriCount();
	}
}

bool VirtualNode::compNodeStats(const VirtualNode* _rhs) const
{
	if (getInverseTriCount() != _rhs->getInverseTriCount()){
		return false;
	}
	if (mPriReqSet.size() != _rhs->mPriReqSet.size()){
		return false;
	}
	set<VirtualRequest*>::iterator lhsSetIt = mPriReqSet.begin();
	set<VirtualRequest*>::iterator rhsSetIt = _rhs->mPriReqSet.begin();
	for (; lhsSetIt != mPriReqSet.end() && rhsSetIt != _rhs->mPriReqSet.end(); ++lhsSetIt, ++rhsSetIt){
		if ((*lhsSetIt)->getId() != (*rhsSetIt)->getId()){
			return false;
		}
	}
	return true;
}

void VirtualNode::debug()
{
	// to test locally
	VirtualNode::priNodeMap->clear();
}

} // oocframework
