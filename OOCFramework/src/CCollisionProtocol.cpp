/**
 * @file	CCollisionProtocol.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.11.2009
 * @brief 	CCollisionProtocol class definition.
 */

#include "CCollisionProtocol.h"

#include <iostream>
#include <algorithm>

using namespace std;
namespace oocframework{

CCollisionProtocol::CCollisionProtocol(unsigned int _seed, int _lvlOfRedundancy) :
	mPriMpiCon(MpiControl::getSingleton()), mPriRndNodeSet(std::set<int>()), mPriMTwister(_seed),
	mPriLvlOfRedundancy(_lvlOfRedundancy), mPriDataNodeCount(mPriMpiCon->getGroupSize(MpiControl::DATA)),
	mPriVirtualNodes(mPriDataNodeCount, VirtualNode(0))

{
	mPriSeed = _seed;
	mPriLowestNodeId = mPriMpiCon->getDataGroup()[0];
	mPriHighestNodeId = mPriMpiCon->getDataGroup()[mPriDataNodeCount-1];
	int i, idx;
	for (i=mPriLowestNodeId, idx = 0; i<= mPriHighestNodeId; ++i, ++idx){
		mPriVirtualNodes[idx] = (VirtualNode(i));
		VirtualNode::registerNode(&mPriVirtualNodes[idx]);
		mPriVirtualRequests.push_back(VirtualRequest());
	}

	mPriCConst = 2;

}

CCollisionProtocol::~CCollisionProtocol()
{
}

void
CCollisionProtocol::generateDistribution(const oocformats::LooseOctree* _lo)
{
	// save triangleCount of OctreeNode in map
	mPriLoTriMap.insert(make_pair(_lo->getId(), _lo->getTriangleCount()));

	// parse octree
	if (_lo->hasData()){
		genRndNodes(mPriIdToNodeMap[_lo->getId()]);
		NodeSetIter nsi = mPriIdToNodeMap[_lo->getId()].begin();
		for (; nsi != mPriIdToNodeMap[_lo->getId()].end(); ++nsi){
			mPriNodeToIdMap[*nsi].insert(_lo->getId());
		}

//		mPriIdToNodeMap[_lo->getId()].insert();
	}
	for (unsigned i=0; i<8; ++i){
		if (_lo->getChild(i)!=0){
			generateDistribution(_lo->getChild(i));
		}
	}
}

void CCollisionProtocol::genRndNodes(std::set<int>& _nodeSet)
{
	pair<set<int>::iterator, bool> verify;
	for (int i = 0; i < mPriLvlOfRedundancy; ++i){
		do{
			verify = _nodeSet.insert(mPriLowestNodeId + mPriMTwister.randInt(mPriDataNodeCount-1));
		}while(!verify.second);
	}

}

void CCollisionProtocol::doCCollision(vector<ooctools::Quintuple>* _quintVec, map<int, set<ooctools::Quintuple> >* _nodeReqMap)
{
	//ensure randomness of _quintVec
	random_shuffle(_quintVec->begin(), _quintVec->end());

	cout << "1" << endl;
	//reset all nodes to start-values
	VirtualNode::hardReset();
	cout << "2" << endl;

	// iterator over all requested vbos
	vector<ooctools::Quintuple>::iterator quintIt = _quintVec->begin();
	while (quintIt != _quintVec->end() ){
		cout << "3" << endl;
		// iterate over each data-node to pick exactly these number of vbos
		resetAllRequests();
		cout << "4" << endl;
		for (unsigned i=0; (i < mPriMpiCon->getGroupSize(MpiControl::DATA)) && (quintIt != _quintVec->end()); ++i){
			cout << "5" << endl;
			set<int>& nodeSet = mPriIdToNodeMap[quintIt->id];
			set<int>::iterator nodeIt = nodeSet.begin();
			// iterate over all nodes which are in possession of this vbo and inc request-count
			mPriVirtualRequests[i].reset(&(*quintIt), mPriLoTriMap[quintIt->id], &mPriIdToNodeMap[quintIt->id]);
			cout << "6" << endl;
			quintIt++;
		}
		cout << "7" << endl;

		solveCCollision(2);
		cout << "8" << endl;

		//extract assignments from VirtualRequests
		cout << "9" << endl;

		for (unsigned int i=0; i< mPriVirtualRequests.size(); ++i){
			(*_nodeReqMap)[mPriVirtualRequests[i].getServiceNodeRank()].insert(*mPriVirtualRequests[i].getQuintuple());
		}
		cout << "10" << endl;

	}

}

/*
- randomize request-order; reorder on Data-Node side
-

for each #DataNodes requests
  for each DataNode in Request[i]  // that has given Object
    increase LoadCount of the DataNode by one
    save in local map<int, set<uint64_t> > each object assigned to DataNode
  rof
rof

new method -> bool solveCCollision(map<int, set<uint64_t> >&, unsigned cConst)
  for each DataNode
    check load against cConst
    if (OK)
      // attention here: randomization needs to be implemented
      get all other DataNodes having this Object and remove the requests from NodeMap
      and decrease loadCount accordingly
      finally mark DataNode as done // no idea yet how to do that
    else
      for each Object assigned to this node
        check if this object is assigned to another DataNode with load < cConst
        check cConst again -> if true -> break;
        // question: really break here or try to distribute further?
      rof
      check cConst again -> if false -> return false; // cConst is too small
      // another thought: can it happen that this situation has a solution if other nodes can be resolved?
    fi

  rof

 *
 */
const std::set<uint64_t>& CCollisionProtocol::getMyNodeSet()
{
	return mPriNodeToIdMap[mPriMpiCon->getRank()];
}

void CCollisionProtocol::solveCCollision(unsigned _cConst, unsigned int _assignedValue)
{
	//TODO coin-flip
	//TODO make sure no assigning and tagging occurs twice
	unsigned tagCount = 0;

	unsigned requestCount = mPriVirtualRequests.size();
	unsigned requestsAssigned = _assignedValue;
	sort(mPriVirtualNodes.begin(), mPriVirtualNodes.end());
	vector<VirtualNode>::reverse_iterator nodeIt;
	// using reverse_iterator here because it's sorted by inverse triCount, meaning
	// largest number = lowest load = largest probability to be picked
	bool taggedANode = true;
	while(taggedANode && (requestCount > requestsAssigned)) {
		taggedANode = false;
		for (nodeIt = mPriVirtualNodes.rbegin(); nodeIt!=mPriVirtualNodes.rend(); ++nodeIt){
			if (!nodeIt->isTagged() && nodeIt->getRequestCount() <= _cConst){
				// choose a random node among nodes with equal
				//TODO pick random node and tag4service
				nodeIt->tag();
				tagCount++;
				taggedANode = true;
			}
		}

		// sub-turn
		for (nodeIt = mPriVirtualNodes.rbegin(); nodeIt!=mPriVirtualNodes.rend(); ++nodeIt){
			if (nodeIt->isTagged()){
				if (nodeIt->isTagged4Service()){
					requestsAssigned += nodeIt->professService();
				}
			}
			else if (nodeIt->getRequestCount() == 0){
				nodeIt->tag();
				tagCount++;
				taggedANode = true;
			}
		}
	}

	if (requestCount > requestsAssigned){
		//recurse
		solveCCollision(++_cConst, requestsAssigned);
	}

}

void CCollisionProtocol::resetAllRequests()
{
	for (unsigned int i = 0; i< mPriVirtualNodes.size(); ++i){
		mPriVirtualRequests[i].reset();
	}
}

VirtualNode* CCollisionProtocol::selectRandomNode(std::list<VirtualNode*>* _candidateList)
{
	// reset MersenneTwister with spontaneous randomness
	mPriMTwister.seed();

	VirtualNode* currentPick = *_candidateList->begin();
	unsigned int winningNumber = mPriMTwister.randInt();

	std::list<VirtualNode*>::iterator nodeIt = ++_candidateList->begin();
	for (; nodeIt != _candidateList->end(); ++nodeIt){
		unsigned tmp = mPriMTwister.randInt();
		if (tmp > winningNumber){
			winningNumber = tmp;
			currentPick = *nodeIt;
		}
	}

	// reset MersenneTwister with constant randomness
	mPriMTwister.seed(mPriSeed);

	return currentPick;
}

void CCollisionProtocol::searchEqualNodes(vector<VirtualNode>::reverse_iterator _from, vector<VirtualNode>::reverse_iterator _to, std::list<VirtualNode*>* _equalList)
{
	vector<VirtualNode>::reverse_iterator tmpIt = _from;
	for (;(tmpIt != _to && tmpIt->getInverseTriCount() == _from->getInverseTriCount()); ++tmpIt){
		if (_from->compRequests(&(*tmpIt))){
			_equalList->push_back(&(*tmpIt));
		}
	}
}

void CCollisionProtocol::debug()
{
	mPriVirtualNodes[0].debug(4);
	mPriVirtualNodes[1].debug(9);
	mPriVirtualNodes[2].debug(123);
	mPriVirtualNodes[3].debug(17);
	sort(mPriVirtualNodes.begin(), mPriVirtualNodes.end());
	vector<VirtualNode>::reverse_iterator it;
	cout << "total tris: " << VirtualNode::getTotalTriCount() << endl;
	for (it = mPriVirtualNodes.rbegin(); it!=mPriVirtualNodes.rend(); ++it){
		cout << "tricount vs inv tricount " << (*it).getRank() << ": " << (*it).getTriCount() << " vs. " << (*it).getInverseTriCount() << endl;
	}

}

} // oocframework
