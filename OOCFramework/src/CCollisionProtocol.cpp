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
//		mPriIdToNodeMap[_lo->getId()].insert();
		unsigned int rnd = mPriLowestNodeId + mPriMTwister.randInt(mPriLowestNodeId + mPriHighesNodeId);
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

} // oocframework
