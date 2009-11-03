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

void CCollisionProtocol::doCCollision(set<ooctools::Quintuple>& _quintSet, map<int, set<ooctools::Quintuple> >& _nodeReqMap)
{
	// iterator over all requested vbos
	set<ooctools::Quintuple>::iterator quintIt = _quintSet.begin();
	for (; quintIt != _quintSet.end(); ){
		// iterate over each data-node to pick exactly these number of vbos
		for (unsigned i=0; (i < mPriMpiCon->getGroupSize(MpiControl::DATA)) && (quintIt != _quintSet.end()); ++i){
			int minLoadNode = mPriMpiCon->getDataGroup()[0];
			set<int>& nodeSet = mPriIdToNodeMap[quintIt->id];
			set<int>::iterator nodeIt = nodeSet.begin();
			// iterate over all nodes which are in possession of this vbo and find minLoad
			for (; nodeIt != nodeSet.end(); ++nodeIt){
				if (mPriNodeLoad[*nodeIt] < mPriNodeLoad[minLoadNode]){
					minLoadNode = *nodeIt;
				}
			}
			if (mPriNodeLoad[minLoadNode] >= mPriCConst){
				cerr << "c-collision collided!!!!" << endl;
			}
			else {
				_nodeReqMap[minLoadNode].insert(*quintIt);
				mPriNodeLoad[minLoadNode]++;
			}
		}
	}
}

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
