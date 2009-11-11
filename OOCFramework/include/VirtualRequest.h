/**
 * @file	VirtualRequest.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 06.11.2009
 * @brief 	VirtualRequest class declaration.
 */

#ifndef VIRTUALREQUEST_H_
#define VIRTUALREQUEST_H_

#include "declarations.h"

#include <set>
#include <map>
#include "stdint.h"

#include "VirtualNode.h"
#include "StructDefs.h"

namespace oocframework {

/**
 * @class VirtualRequest
 * @brief This class represents a virtual request for the c-Collision Protocol.
 */
class VirtualRequest {
public:
	VirtualRequest();
	virtual ~VirtualRequest();

	/**
	 * @brief As the name suggests all but the given node will be unregistered and removed from
	 * this request.
	 * @note The usefulness of this method remains to be seen, because I think rather all links are removed then just
	 * a few.
	 * @param _vNode
	 */
	void unregisterAllButThis(VirtualNode* _vNode);

	void unregisterAll();

	/**
	 * @brief With this method a node professes his intention to service this
	 * request. It returns the triangle count for this request.
	 */
	unsigned imHappyToHelp(VirtualNode* _vNode);

	/**
	 * @brief This actually sets this request with new content, aka. rewrites the members.
	 * @param _relevantNodes This is a pointer to a set with all node-ranks in possession of this
	 * request's data.
	 */
//	void reset(uint64_t _id, int _destRank, unsigned _triCount, std::set<int>* _relevantNodes);

	void reset(ooctools::Quintuple* _quint = 0, unsigned int _triCount = 0, std::set<int>* _relevantNodes = 0);

	/**
	 * @brief This returns the OctreeNodeId for this request.
	 */
	uint64_t getId() const {return mPriId;};

	/**
	 * @brief Returns the rank of the requesting RenderNode.
	 */
	int getDestRank() const {return mPriDestRank;};

	/**
	 * @brief Returns the number of triangles that is associated with this request.
	 */
	unsigned getTriCount()const {return mPriTriCount;};
	void tag() {mPriTag = true;};
	bool isTagged() const {return mPriTag;};

	/**
	 * @brief This method returns the final registered node, which promised to take care
	 * of this request's needs.
	 */
	int getServiceNodeRank() const {return mPriServiceNodeRank;};
	const ooctools::Quintuple* getQuintuple() const {return mPriQuint;};
	bool operator<(const VirtualRequest& _rhs) const;

private:
	uint64_t mPriId;
	unsigned mPriTriCount;
	bool mPriTag;
	int mPriDestRank;
	int mPriServiceNodeRank;
	const ooctools::Quintuple* mPriQuint;
	std::set<VirtualNode*> mPriNodeSet;
	std::map<int, std::set<VirtualRequest>* > mPriNodeMap;
};

}

#endif /* VIRTUALREQUEST_H_ */
