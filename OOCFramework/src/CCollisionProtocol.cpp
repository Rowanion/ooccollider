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
	mPriVirtualNodes(mPriDataNodeCount, VirtualNode(0)), mPriVirtualRequests(mPriDataNodeCount, VirtualRequest())

{
	mPriSeed = _seed;
	mPriLowestNodeId = mPriMpiCon->getDataGroup()[0];
	mPriHighestNodeId = mPriMpiCon->getDataGroup()[mPriDataNodeCount-1];
	int i, idx;
	for (i=mPriLowestNodeId, idx = 0; i<= mPriHighestNodeId; ++i, ++idx){
		mPriVirtualNodes[idx] = (VirtualNode(i));
		VirtualNode::registerNode(&mPriVirtualNodes[idx]);
		mPriVirtualRequests[idx] = (VirtualRequest());
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

	//reset all nodes to start-values
	VirtualNode::hardReset();

	// iterator over all requested vbos
	vector<ooctools::Quintuple>::iterator quintIt = _quintVec->begin();
	while (quintIt != _quintVec->end() ){
		// iterate over each data-node to pick exactly these number of vbos
		resetAllRequests();
		for (unsigned i=0; (i < mPriDataNodeCount) && (quintIt != _quintVec->end()); ++i){
			// iterate over all nodes which are in possession of this vbo and inc request-count
			mPriVirtualRequests[i].reset(&(*quintIt), mPriLoTriMap[quintIt->id], &mPriIdToNodeMap[quintIt->id]);
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
	//TODO make sure no assigning and tagging occurs twice
	//TODO somehow the _cConst grows to infinity.....?
	//TODO with request-ids 4 and 8 it terminates - problem seems to be odd ratio between request-objects and actual requests.
	//TODO like when i have 2 nodes but just one request. The requestCount==requestsAssigned constraint never gets satisfied.
	cout << "7.1" << endl;
	unsigned requestCount = mPriVirtualRequests.size();
	unsigned requestsAssigned = _assignedValue;
	cout << _cConst << ": req. status: " << requestsAssigned << "/" << requestCount << endl;
	cout << "7.2" << endl;
	vector<VirtualNode>::reverse_iterator nodeIt;
	// using reverse_iterator here because it's sorted by inverse triCount, meaning
	// largest number = lowest load = largest probability to be picked
	bool taggedANode = true;
	cout << "7.3" << endl;
	while(taggedANode && (requestCount > requestsAssigned)) {
		sort(mPriVirtualNodes.begin(), mPriVirtualNodes.end());
		taggedANode = false;
		cout << "7.4" << endl;
		for (nodeIt = mPriVirtualNodes.rbegin(); nodeIt!=mPriVirtualNodes.rend(); ++nodeIt){
			cout << "7.5" << endl;
			if (!nodeIt->isTagged() && nodeIt->getRequestCount() <= _cConst){
				// choose a random node among nodes with equal requests and probability
				selectRandomNode(&(*nodeIt))->tag4Service();
				taggedANode = true;
				cout << "7.6" << endl;

			}
		}
		cout << "tagged a node? " << taggedANode << endl;
		cout << "7.7" << endl;
		//TODO if no node has been tagged until here -> recurse!
		if(!taggedANode)
			break;

		// sub-turn
		for (nodeIt = mPriVirtualNodes.rbegin(); nodeIt!=mPriVirtualNodes.rend(); ++nodeIt){
			cout << "7.8" << endl;
			if (nodeIt->isTagged4Service() && nodeIt->getRequestCount() > 0){
				cout << "7.10" << endl;
				cout << "assigning " << nodeIt->getRequestCount() << " jobs to node " << nodeIt->getRank() << endl;
				requestsAssigned += nodeIt->professService();
			}
//			else if (nodeIt->getRequestCount() == 0){
//				cout << "7.11" << endl;
//				nodeIt->tag();
//				taggedANode = true;
//			}
		}
	}
	cout << "7.12" << endl;
//	exit(0);

	if (requestCount > requestsAssigned){
		cout << "7.13" << endl;

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

VirtualNode* CCollisionProtocol::selectRandomNode(VirtualNode* _candidate)
{
	std::set<VirtualNode*> nodeSet = std::set<VirtualNode*>();
	// fetches request-list of this node
	std::set<VirtualRequest*>* candidateRequests = _candidate->getReqSet();
	std::set<VirtualRequest*>::iterator reqIt = candidateRequests->begin();
	std::set<VirtualNode*>::iterator nodeIt;
	// fetches all nodes of all those requests...
	for (; reqIt != candidateRequests->end(); reqIt++){
//		nodeSet.insert((*reqIt)->getNodeSet()->begin(), (*reqIt)->getNodeSet()->end());
		// ... and tests them against state-equality of _candiate
		for (nodeIt = (*reqIt)->getNodeSet()->begin(); nodeIt != (*reqIt)->getNodeSet()->end(); ++nodeIt){
			if (_candidate->compNodeStats(*nodeIt)){
				// tag node because equality is given and to prevent re-assignment
				(*nodeIt)->tag();
				nodeSet.insert(*nodeIt);
			}
		}
	}

	// ----------------------------

	// reset MersenneTwister with spontaneous randomness
	mPriMTwister.seed();

	VirtualNode* currentPick = *nodeSet.begin();
	unsigned int winningNumber = mPriMTwister.randInt();

	nodeIt = ++nodeSet.begin();
	for (; nodeIt != nodeSet.end(); ++nodeIt){
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
		if (_from->compNodeStats(&(*tmpIt))){
			_equalList->push_back(&(*tmpIt));
		}
	}
}

void CCollisionProtocol::debug(unsigned nodeCount)
{
	// to test this locally
	mPriVirtualNodes.clear();
	mPriVirtualRequests.clear();
	VirtualNode::debug();
	mPriDataNodeCount = nodeCount;
	mPriVirtualNodes.resize(nodeCount, VirtualNode(0));
	mPriVirtualRequests.resize(nodeCount, VirtualRequest());
	mPriLowestNodeId = 0;
	mPriHighestNodeId = nodeCount-1;
	int i, idx;
	for (i=mPriLowestNodeId, idx = 0; i<= mPriHighestNodeId; ++i, ++idx){
		mPriVirtualNodes[idx] = (VirtualNode(i));
		VirtualNode::registerNode(&mPriVirtualNodes[idx]);
		mPriVirtualRequests[idx] = (VirtualRequest());
	}

}

} // oocframework
