/**
 * @file	VirtualNode.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 06.11.2009
 * @brief 	VirtualNode class declaration.
 */

#ifndef VIRTUALNODE_H_
#define VIRTUALNODE_H_

#include "declarations.h"

#include <map>
#include <set>

#include "VirtualRequest.h"

namespace oocframework {

/**
 * @class VirtualNode
 * @brief This class represents a virtual data-node for the c-Collision Protocol.
 */
class VirtualNode {
public:
	VirtualNode(int _rank);
	virtual ~VirtualNode();

	/**
	 * @brief This method registers given request with this node, thus creating an edge of the c-Collision-graph.
	 */
	void registerRequest(VirtualRequest* _vReq);

	/**
	 * @brief This method un-registers given request from this this node, thus removing an edge from the c-Collision-graph.
	 */
	void unregisterRequest(VirtualRequest* _vReq);
	int getRank() const {return mPriRank;};
	unsigned getTriCount() const {return mPriTriCount;};

	/**
	 * @brief This method returns the numerator of the inverse load; ie. totalTris-triCount.
	 */
	unsigned getInverseTriCount() const {return VirtualNode::priTotalTriCount-mPriTriCount;};

	/**
	 * @brief This resets the tag on this node but no the tri-count.
	 */
	void newSubTurn();
	void newTurn();

	/**
	 * @brief This method actually promises each request that this node will take care of it.
	 * Internally it calls each request's imHappyToHelp() method, which will result in purging
	 * all links to and fro it. This effectively increases the local and global triCount.
	 * @return the number of jobs taken care of.
	 */
	unsigned int professService();

	/**
	 * @brief Tags this node as being resolved.
	 * This will be reset each sub-turn.
	 */
	void tag(){mPriTag = true;};
	bool isTagged() const {return mPriTag;};
	void tag4Service(){mPriServiceTag = true;};
	bool isTagged4Service() const {return mPriServiceTag;};
	unsigned int getRequestCount() const {return mPriReqCount;};
	bool compRequests(const VirtualNode* _rhs) const;
	bool operator<(const VirtualNode& _rhs) const;
	void debug(unsigned _i);

	/**
	 * @brief Returns a pointer to the node identified by _rank.
	 */
	static VirtualNode* getNode(int _rank);
	static unsigned getNodeCount();
	static unsigned getTotalTriCount();

	/**
	 * @brief This method resets all nodes prior to the next turn (not sub-turn).
	 * This includes the tag, the triCount, the totalTriCount and all links to requests.
	 */
	static void hardReset();
	static void registerNode(VirtualNode* _vNode);

private:
	int mPriRank;
	unsigned mPriTriCount;
	bool mPriTag;
	bool mPriServiceTag;
	unsigned mPriReqCount;
	std::set<VirtualRequest*> mPriReqSet;
	std::map<int, std::set<VirtualRequest>* > mPriReqMap;

	static std::map<int, VirtualNode*>* priNodeMap;
	static unsigned priNodeCount;
	static unsigned priTotalTriCount;
};

}

#endif /* VIRTUALNODE_H_ */
