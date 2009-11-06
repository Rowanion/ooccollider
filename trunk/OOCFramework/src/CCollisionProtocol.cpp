/**
 * @file	CCollisionProtocol.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.11.2009
 * @brief 	CCollisionProtocol class definition.
 */

#include "CCollisionProtocol.h"

#include <iostream>

using namespace std;
namespace oocframework{

CCollisionProtocol::CCollisionProtocol(unsigned int _seed, int _lvlOfRedundancy) :
	mPriMpiCon(MpiControl::getSingleton()), mPriRndNodeSet(std::set<int>()), mPriMTwister(_seed),
	mPriLvlOfRedundancy(_lvlOfRedundancy)
{
	mPriDataNodeCount = mPriMpiCon->getGroupSize(MpiControl::DATA);
	mPriLowestNodeId = mPriMpiCon->getDataGroup()[0];
	mPriHighesNodeId = mPriMpiCon->getDataGroup()[mPriDataNodeCount-1];

	mPriCConst = 2;

	resetLoad();
}

CCollisionProtocol::~CCollisionProtocol()
{
}

void
CCollisionProtocol::generateDistribution(const oocformats::LooseOctree* _lo)
{
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

void CCollisionProtocol::doCCollision(set<ooctools::Quintuple>* _quintSet, map<int, set<ooctools::Quintuple> >* _nodeReqMap)
{
	//TODO ensure randomness of _quintSet
	// iterator over all requested vbos

	map<int, set<uint64_t> > initialDistribution = map<int, set<uint64_t> >();
	set<ooctools::Quintuple>::iterator quintIt = _quintSet->begin();
	for (; quintIt != _quintSet->end(); ){
		// iterate over each data-node to pick exactly these number of vbos
		for (unsigned i=0; (i < mPriMpiCon->getGroupSize(MpiControl::DATA)) && (quintIt != _quintSet->end()); ++i){
			set<int>& nodeSet = mPriIdToNodeMap[quintIt->id];
			set<int>::iterator nodeIt = nodeSet.begin();
			// iterate over all nodes which are in possession of this vbo and inc request-count
			for (; nodeIt != nodeSet.end(); ++nodeIt){
				initialDistribution[*nodeIt].insert(quintIt->id);
				mPriRequestCount[*nodeIt]++;
			}
			quintIt++;
		}
	}
	solveCCollision(&initialDistribution, 2);

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

void CCollisionProtocol::resetLoad()
{
	for (unsigned i=0; i< mPriMpiCon->getGroupSize(MpiControl::DATA); ++i){
		mPriNodeLoad[mPriMpiCon->getDataGroup()[i]] = 0;
	}

}

bool CCollisionProtocol::solveCCollision(map<int, set<uint64_t> >* _initialDistribution, unsigned _cConst)
{
	/*
	 * two iterations per cConst
	 * first check requestCount of each node and assign if fits
	 *   if more then 1 node with
	 * second check if nodes can now be solved
	 * if so return true
	 * else return false
	 *
	 * need requestCounter / node
	 * need triCounter / node
	 * need requestAssignmentContainer
	 */

	map<int, set<uint64_t> >::iterator mapIntSetIt = _initialDistribution->begin();
	for (; mapIntSetIt != _initialDistribution->end(); ++mapIntSetIt){ // for each node
		if (mPriRequestCount[mapIntSetIt->first] <= _cConst){
			set<uint64_t>& idSet = mapIntSetIt->second;
			set<uint64_t>::iterator uintIt = idSet.begin();
			for(; uintIt != idSet.end(); ++uintIt){
				// check each node having this baby for reqLoad
//				if (/* reqCount[mPriIdToNodeMap[*uintIt]]<=_cConst*/)
			}
		}
	}

//	NodeMapSetIter nmsi = _roundResult->begin();
//	for (; nmsi != _roundResult->end(); ++nmsi){
//		//check load against cConst
//		if (mPriNodeLoad[nmsi->first] <= mPriCConst){ // if load is within cConst
//		      // attention here: randomization needs to be implemented
////		      get all other DataNodes having this Object and remove the requests from NodeMap
////		      and decrease loadCount accordingly
////		      finally mark DataNode as done // no idea yet how to do that
//		}
//		else{ // if load is out of bounds
////		      for each Object assigned to this node{
////		        check if this object is assigned to another DataNode with load < cConst
////		        check cConst again -> if true -> break;
//		        // question: really break here or try to distribute further?
//		}
		/*
      check cConst again -> if false -> return false; // cConst is too small
      // another thought: can it happen that this situation has a solution if other nodes can be resolved?
    }
		 */
//	}
}


void CCollisionProtocol::debug()
{
	IdMapSetIter it = mPriIdToNodeMap.begin();
	NodeSetIter nsi;
	for (; it!=mPriIdToNodeMap.end(); it++){
		nsi = it->second.begin();
		cout << it->first << ": ";
		for(; nsi != it->second.end(); ++nsi){
			cout << *nsi << ", ";
		}
		cout << endl;
	}
}

} // oocframework
