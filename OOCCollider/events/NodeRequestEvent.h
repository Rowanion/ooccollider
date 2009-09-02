/**
 * @file	NodeRequestEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	NodeRequestEvent class declaration.
 */

#ifndef NODEREQUESTEVENT_H_
#define NODEREQUESTEVENT_H_

#include <vector>
#include <map>

#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"

/**
 * @class NodeRequestEvent
 *
 * @brief This event is generated when a Render-Node has determined, which nodes are visible this frame
 * and which of these need to be loaded by the Render-Node. Happens right before the actual
 * rendering. So this could be a bottleneck, because the Render-Node has to wait for the VBOs
 * from the Data-Nodes before continuing its work.
 *
 * This event originates from a Render-Node and is sent to a Data-Node.
 * @note This communication-structure is likely to to be changed when using the c-collision protocol.
 * That's because a Render-Node doesen't have sufficient information on where the node are stored.
 * So one node has to take the manager-role and resubmit the requests to the relevant nodes.
 *
 */
class NodeRequestEvent : public oocframework::IEvent{
public:
	NodeRequestEvent();
	NodeRequestEvent(const std::multimap<float, uint64_t>& idMap, int reciepient);
	NodeRequestEvent(const Message* msg);
	virtual ~NodeRequestEvent();
	static const oocframework::ClassId* classid();
	inline virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Switching to member for byteSize because the size may differ per event.
	 * The resolution is the key to size determination.
	 * @return the size in bytes stored in superclass oocframework::IEvent::mProData
	 */
	inline virtual unsigned getByteSize(){return mPriByteSize;};

	/**
	 * @brief Returns the requested distance to the eye of the node at index idx.
	 */
	inline float getDistance(unsigned idx) const {
		return ((float*)(mProData+sizeof(unsigned)+sizeof(int)))[idx];
	};

	/**
	 * @brief Returns a const pointer to all distances to the eye.
	 */
	inline const float* getDistanceArray() const {
		return ((const float*)(mProData+sizeof(unsigned)+sizeof(int)));
	};

	/**
	 * @brief Returns the requested node-id at index idx.
	 * @note Not sure if we need this. I was just in completeness-mood....
	 */
	inline uint64_t getId(unsigned idx) const {
		return ((uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)+(sizeof(float)*getIdxCount())))[idx];
	};

	/**
	 * @brief Returns a const pointer to all requested node-ids.
	 */
	inline const uint64_t* getIdArray() const {
		return ((const uint64_t*)(mProData+sizeof(unsigned)+sizeof(int)+(sizeof(float)*getIdxCount())));
	};

	/**
	 * @brief Returns the number of nodes contained in this event.
	 */
	inline unsigned getIdxCount() const {
		return ((unsigned*)mProData)[0];
	};

	/**
	 * @brief Returns the number of nodes contained in this event.
	 */
	inline int getRecepient() const {
		return 	((int*)(mProData+sizeof(unsigned)))[0];
	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
	unsigned mPriByteSize;
};

#endif /* NodeRequestEVENT_H_ */
