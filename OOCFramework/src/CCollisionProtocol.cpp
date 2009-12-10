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

#include <boost/lambda/lambda.hpp>

using namespace std;
namespace oocframework{

CCollisionProtocol::CCollisionProtocol(unsigned int _seed, int _lvlOfRedundancy) :
	mPriMpiCon(MpiControl::getSingleton()), mPriRndNodeSet(std::set<int>()), mPriMTwister(),
	mPriLvlOfRedundancy(_lvlOfRedundancy), mPriDataNodeCount(mPriMpiCon->getGroupSize(MpiControl::DATA)),
	mPriVirtualRequests(mPriDataNodeCount, VirtualRequest())

{
	mPriRandomSeed = mPriMTwister.randInt();
	mPriMTwister.seed(_seed);
	mPriVirtualNodes.resize(mPriDataNodeCount);
	mPriSeed = _seed;
	mPriLowestNodeId = mPriMpiCon->getDataGroup()[0];
	mPriHighestNodeId = mPriMpiCon->getDataGroup()[mPriDataNodeCount-1];
	int i, idx;
	for (i=mPriLowestNodeId, idx = 0; i<= mPriHighestNodeId; ++i, ++idx){
		mPriVirtualNodes[idx] = new VirtualNode(i);
		VirtualNode::registerNode(mPriVirtualNodes[idx]);
		mPriVirtualRequests[idx] = (VirtualRequest());
	}

	mPriCConst = 2;

}

CCollisionProtocol::~CCollisionProtocol()
{
}

void
CCollisionProtocol::generateDistribution(const oocformats::LooseRenderOctree* _lo)
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
//	cerr << "number of incoming requests: " << _quintVec->size() << endl;
	//ensure randomness of _quintVec
	random_shuffle(_quintVec->begin(), _quintVec->end());

	//reset all nodes to start-values
	VirtualNode::hardReset();

	// iterator over all requested vbos
	vector<ooctools::Quintuple>::iterator quintIt = _quintVec->begin();
	while (quintIt != _quintVec->end() ){
		// iterate over each data-node to pick exactly these number of vbos
		resetAllRequests();
//		cerr << "performing cCollision with this distribution:" << endl;
		for (unsigned i=0; (i < mPriDataNodeCount) && (quintIt != _quintVec->end()); ++i){
			// iterate over all nodes which are in possession of this vbo and inc request-count
			// ------------------------------------
//			set<int>::iterator setIntIt = mPriIdToNodeMap[quintIt->id].begin();
//			cerr << "request-id " << quintIt->id << endl;
//			for(; setIntIt != mPriIdToNodeMap[quintIt->id].end(); ++setIntIt){
//				cerr << "is distributed to node " << *setIntIt << endl;
//
//			}
			// ------------------------------------
			mPriVirtualRequests[i].reset(&(*quintIt), mPriLoTriMap[quintIt->id], &mPriIdToNodeMap[quintIt->id]);
			quintIt++;
		}

//		cerr << "FINAL DISTRIBUTION: " << endl;
//		cerr << "R -> N" << endl;
//		for (unsigned i =0; i< mPriDataNodeCount; ++i){
//			cerr << "Req " << mPriVirtualRequests[i].getId() << " (" << (uint64_t) &mPriVirtualRequests[i] << ") -> ";
//			std::set<VirtualNode*>::iterator aIt = mPriVirtualRequests[i].getNodeSet()->begin();
//			for (; aIt != mPriVirtualRequests[i].getNodeSet()->end(); aIt++){
//				cerr << (*aIt)->getRank() << " (" << uint64_t(*aIt) << ") , ";
//			}
//			cerr << endl;
//		}
//		cerr << "N -> R" << endl;
//		for (unsigned i =0; i< mPriDataNodeCount; ++i){
//			cerr << "Node " << mPriVirtualNodes[i]->getRank() << " (" << (uint64_t) mPriVirtualNodes[i] << ") -> ";
//			std::set<VirtualRequest*>::iterator aIt = mPriVirtualNodes[i]->getReqSet()->begin();
//			for (; aIt != mPriVirtualNodes[i]->getReqSet()->end(); aIt++){
//				cerr << (*aIt)->getId() << " (" << uint64_t(*aIt) << ") , ";
//			}
//			cerr << endl;
//		}

		if (quintIt != _quintVec->end() || _quintVec->size() % mPriVirtualRequests.size() == 0){
			solveCCollision(mPriCConst);
		}
		else {
//			cerr << "using modulo: " << mPriVirtualNodes.size() << "- (" << _quintVec->size() << " % " << mPriVirtualRequests.size() << ")" << endl;
			solveCCollision(mPriCConst, mPriVirtualNodes.size() - (_quintVec->size() % mPriVirtualRequests.size()));
		}

		//extract assignments from VirtualRequests

		for (unsigned int i=0; i< mPriVirtualRequests.size(); ++i){
			if (mPriVirtualRequests[i].getQuintuple()!=0){
				(*_nodeReqMap)[mPriVirtualRequests[i].getServiceNodeRank()].insert(*mPriVirtualRequests[i].getQuintuple());
			}
		}
		// clean up the nodes.
		for (unsigned int i=0; i< mPriVirtualNodes.size(); ++i){
			mPriVirtualNodes[i]->newTurn();
		}

	}

}

const std::set<uint64_t>& CCollisionProtocol::getMyNodeSet()
{
	return mPriNodeToIdMap[mPriMpiCon->getRank()];
}

void CCollisionProtocol::solveCCollision(unsigned _cConst, unsigned int _assignedValue)
{
	unsigned requestCount = mPriVirtualRequests.size();
	unsigned requestsAssigned = _assignedValue;
//	cerr << "started solving with c=" << _cConst << ": req. status: " << requestsAssigned << "/" << requestCount << endl;
	vector<VirtualNode*>::reverse_iterator nodeIt;
	// using reverse_iterator here because it's sorted by inverse triCount, meaning
	// largest number = lowest load = largest probability to be picked
	bool taggedANode = true;
	while(taggedANode && (requestCount > requestsAssigned)) {
		sort(mPriVirtualNodes.begin(), mPriVirtualNodes.end(), *boost::lambda::_1 < *boost::lambda::_2 );
		taggedANode = false;
		for (nodeIt = mPriVirtualNodes.rbegin(); nodeIt!=mPriVirtualNodes.rend(); ++nodeIt){
			if (!(*nodeIt)->isTagged() && (*nodeIt)->getRequestCount() <= _cConst){
				// choose a random node among nodes with equal requests and probability
				selectRandomNode(*nodeIt)->tag4Service();
				taggedANode = true;
			}
		}
//		cerr << "tagged a node? " << taggedANode << endl;
		//if no node has been tagged until here -> recurse!
		if(!taggedANode)
			break;

		// sub-turn
		for (nodeIt = mPriVirtualNodes.rbegin(); nodeIt!=mPriVirtualNodes.rend(); ++nodeIt){
			if ((*nodeIt)->isTagged4Service() && (*nodeIt)->getRequestCount() > 0){
				unsigned tris = (*nodeIt)->getTriCount();
				unsigned jobs = (*nodeIt)->getRequestCount();
				requestsAssigned += (*nodeIt)->professService();
//				cerr << "assigning " << jobs << " jobs to node " << (*nodeIt)->getRank() << " with "<< (*nodeIt)->getTriCount()-tris << " tris." << endl;
			}
//			else if (nodeIt->getRequestCount() == 0){
//				cout << "7.11" << endl;
//				nodeIt->tag();
//				taggedANode = true;
//			}
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

VirtualNode* CCollisionProtocol::selectRandomNode(VirtualNode* _candidate)
{
	std::set<VirtualNode*> nodeSet = std::set<VirtualNode*>();
	// adding start-node to set
	nodeSet.insert(_candidate);
	// fetches request-list of this node
	std::set<VirtualRequest*>* candidateRequests = _candidate->getReqSet();
	std::set<VirtualRequest*>::iterator reqIt = candidateRequests->begin();
	std::set<VirtualNode*>::iterator nodeIt;
	// fetches all nodes of all those requests...
	for (reqIt = candidateRequests->begin(); reqIt != candidateRequests->end(); reqIt++){
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
//	cerr << "these nodes have been selected at random:" << endl;
//	for (nodeIt = nodeSet.begin(); nodeIt != nodeSet.end(); ++nodeIt){
//		cerr << (*nodeIt)->getRank() << ": " << (*nodeIt)->getRequestCount() << ", " << (*nodeIt)->getTriCount() << endl;
//	}
	// ----------------------------

	// reset MersenneTwister with spontaneous randomness
	mPriMTwister.seed(mPriRandomSeed);

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

void CCollisionProtocol::debug()
{
		cerr << "mPriIdToNodeMap: " << mPriIdToNodeMap.size() << endl;
		cerr << "mPriNodeToIdMap: " << mPriNodeToIdMap.size() << endl;
		cerr << "mPriRndNodeSet: " << mPriRndNodeSet.size() << endl;
		cerr << "mPriNodeLoad: " << mPriNodeLoad.size() << endl;
		cerr << "mPriTriCount: " << mPriTriCount.size() << endl;
		cerr << "mPriRequestCount: " << mPriRequestCount.size() << endl;
		cerr << "mPriVirtualNodes: " << mPriVirtualNodes.size() << endl;
		cerr << "mPriVirtualRequests: " << mPriVirtualRequests.size() << endl;
		cerr << "mPriLoTriMap: " << mPriLoTriMap.size() << endl;


}

} // oocframework
