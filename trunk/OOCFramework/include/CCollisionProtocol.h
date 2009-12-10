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
#include "LooseRenderOctree.h"
#include "VirtualNode.h"
#include "VirtualRequest.h"

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
	void generateDistribution(const oocformats::LooseRenderOctree* _lo);
	const std::set<uint64_t>& getMyNodeSet();
	void doCCollision(std::vector<ooctools::Quintuple>* _quintVec, std::map<int, std::set<ooctools::Quintuple> >* _nodeReqMap);

	/**
	 * @brief Tries to solve one round of the c-collision protocol.
	 * Assumptions:
	 * \f$ \texttt{Let n be the number of nodes possessing a single object. Then}\f$ <br>
	 * \f$ \frac{\sum_{i=0}^{n} \left(Triangles_i\right)-Triangles_k}{\sum_{j=0}^{n} \left(\sum_{i=0}^{n} \left(Triangles_i\right)-Triangles_j\right)}\f$ <br>
	 * \f$ \texttt{is the probability of choosing a single Node } N_k.\f$ <br>
	 * \f$ \texttt{Example: 3 Nodes } N_0, N_1 \texttt{ and }N_2 \texttt{ with 10, 1 and 89 triangles.}\f$ <br>
	 * \f$ \texttt{So the probabilities would be}\f$ <br>
	 * \f$ \frac{100-10}{200}, \frac{100-1}{200}\texttt{ and }\frac{100-89}{200} = \frac{90}{200}, \frac{99}{200}\texttt{ and }\frac{11}{200}\f$ <br>
	 *
	 */
	void solveCCollision(unsigned _cConst, unsigned int _assignedValue = 0);
	void debug();
private:
	oocframework::MpiControl* mPriMpiCon;
	std::map<uint64_t, std::set<int> > mPriIdToNodeMap;
	std::map<int, std::set<uint64_t> > mPriNodeToIdMap;
	std::set<int> mPriRndNodeSet;
	ooctools::MTRand mPriMTwister;
	int mPriLvlOfRedundancy;
	unsigned int mPriDataNodeCount;
	int mPriLowestNodeId;
	int mPriHighestNodeId;
	unsigned int mPriCConst;
	unsigned int mPriSeed;
	std::map<int, unsigned int> mPriNodeLoad;
	std::map<int, unsigned int> mPriTriCount;
	std::map<int, unsigned int> mPriRequestCount;
	std::vector<VirtualNode*> mPriVirtualNodes;
	std::vector<VirtualRequest> mPriVirtualRequests;
	std::map<uint64_t, unsigned int> mPriLoTriMap;
	unsigned int mPriRandomSeed;


	typedef std::map<uint64_t, std::set<int> >::iterator IdMapSetIter;
	typedef std::map<int, std::set<uint64_t> >::iterator NodeMapSetIter;
	typedef std::set<int>::iterator NodeSetIter;

	void genRndNodes(std::set<int>& _nodeSet);
	void resetLoad();
	void resetAllRequests();
	VirtualNode* selectRandomNode(VirtualNode* _candidate);
	void searchEqualNodes(std::vector<VirtualNode>::reverse_iterator _from, std::vector<VirtualNode>::reverse_iterator _to, std::list<VirtualNode*>* _equalList);
};

} //oocframework

#endif /* CCOLLISIONPROTOCOL_H_ */
