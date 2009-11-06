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

class VirtualNode {
public:
	VirtualNode(int _rank);
	virtual ~VirtualNode();
	void registerRequest(VirtualRequest* _vReq);
	void unregisterRequest(VirtualRequest* _vReq);
	int getRank() const {return mPriRank;};
//	bool operator<(const VirtualNode& _rhs) const;
	bool operator<(const VirtualNode* _rhs) const;
	static VirtualNode* getNode(int _rank);
private:
	int mPriRank;
	std::set<VirtualRequest> mPriReqSet;
	std::map<int, std::set<VirtualRequest>* > mPriReqMap;
	static std::map<int, VirtualNode*> mPriNodeMap;
};

}

#endif /* VIRTUALNODE_H_ */
