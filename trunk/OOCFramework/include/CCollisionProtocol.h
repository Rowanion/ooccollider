/**
 * @file	CCollisionProtocol.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.11.2009
 * @brief 	CCollisionProtocol class declaration.
 */

#ifndef CCOLLISIONPROTOCOL_H_
#define CCOLLISIONPROTOCOL_H_

#include <map>
#include <set>
#include <list>

#include "declarations.h"
#include "MpiControl.h"
#include "MersenneTwister.h"
#include "LooseOctree.h"

namespace oocframework{

/**
 * @class CCollisionProtocol
 * @brief This represents the actual implementation of the c-Collision-Protocol.
 * What this class has to do:
 * - maintain list/maps/sets with which object is located at which node.
 * - maintain lists/maps/sets of the loadlevel of particular nodes
 * - resolve a single request-cycle?
 * - resolve all request-cycles of a round?
 * - either return results or pass the requests itself
 */
class CCollisionProtocol {
public:
	CCollisionProtocol(unsigned int _seed, int _lvlOfRedundancy = 2);
	virtual ~CCollisionProtocol();
	void generateDistribution(const oocformats::LooseOctree* _lo);
private:
	oocframework::MpiControl* mPriMpiCon;
	std::map<uint64_t, std::set<int> > mPriIdToNodeMap;
	std::map<int, std::set<uint64_t> > mPriNodeToIdMap;
	std::set<int> mPriRndNodeSet;
	ooctools::MTRand mPriMTwister;
	int mPriLvlOfRedundancy;
	unsigned int mPriDataNodeCount;
	int mPriLowestNodeId;
	int mPriHighesNodeId;

	void genRndNodes(std::set<int>& _nodeSet);
};

} //oocframework

#endif /* CCOLLISIONPROTOCOL_H_ */
