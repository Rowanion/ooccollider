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
#include <set>

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"
#include "StructDefs.h"

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
//	NodeRequestEvent();
	NodeRequestEvent(const NodeRequestEvent& _nre);
	NodeRequestEvent(const std::set<ooctools::Quintuple>& quintSet);
	NodeRequestEvent(const oocframework::Message* msg);
	virtual ~NodeRequestEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Returns a const pointer to the requested Quadruple at index idx.
	 */
	ooctools::Quintuple* getQuintuple(unsigned idx) const {
		return &(((ooctools::Quintuple*)(mProData+sizeof(unsigned)))[idx]);
	};

	/**
	 * @brief Returns a const pointer to all Quadruples.
	 */
	const ooctools::Quintuple* getQuintupleArray() const {
		return ((const ooctools::Quintuple*)(mProData+sizeof(unsigned)));
	};

	/**
	 * @brief Returns the number of nodes contained in this event.
	 */
	unsigned getIdxCount() const {
		return ((unsigned*)mProData)[0];
	};

	const NodeRequestEvent& operator=(const NodeRequestEvent& _rhs);
//	/**
//	 * @brief Returns the id of the mpi-node which requested these vbos.
//	 */
//	int getRecepient() const {
//		return 	((int*)(mProData+sizeof(unsigned)))[0];
//	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* NodeRequestEVENT_H_ */
