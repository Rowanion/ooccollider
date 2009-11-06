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

namespace oocframework {

class VirtualRequest {
public:
	VirtualRequest();
	virtual ~VirtualRequest();
	void registerNode(VirtualNode* _vNode);
	void unregisterNode(VirtualNode* _vNode);
	void reset(uint64_t _id, int _destRank, std::set<int>* _relevantNodes);
	uint64_t getId() const {return mPriId;};
	int getDestRank() const {return mPriDestRank;};
	unsigned getTriCount()const {return mPriTriCount;};
	void tag() {mPriTag = true;};
	bool operator<(const VirtualRequest& _rhs) const;
private:
	uint64_t mPriId;
	unsigned mPriTriCount;
	bool mPriTag;
	int mPriDestRank;
	std::set<VirtualNode*> mPriNodeSet;
	std::map<int, std::set<VirtualRequest>* > mPriNodeMap;
};

}

#endif /* VIRTUALREQUEST_H_ */
