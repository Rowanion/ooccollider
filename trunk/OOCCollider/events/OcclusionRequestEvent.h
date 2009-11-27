/**
 * @file	OcclusionRequestEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	OcclusionRequestEvent class declaration.
 */

#ifndef OCCLUSIONREQUESTEVENT_H_
#define OCCLUSIONREQUESTEVENT_H_

#include <vector>
#include <map>
#include <set>

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"
#include "StructDefs.h"

/**
 * @class OcclusionRequestEvent
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
 * Data:
 * |unsigned  |Quintuple  |Quintuple  |..|Quintuple  |float[16]|
 * |quintCount|Quintuple_0|Quintuple_1|..|Quintuple_n|mvpMatrix|
 *
 */
class OcclusionRequestEvent : public oocframework::IEvent{
public:
//	OcclusionRequestEvent();
	OcclusionRequestEvent(const OcclusionRequestEvent& _nre);
	OcclusionRequestEvent(const std::set<ooctools::Quintuple>& quintSet);
	OcclusionRequestEvent(const std::set<ooctools::Quintuple>& _quintSet, const float* _matrix);
	OcclusionRequestEvent(std::set<ooctools::Quintuple>::iterator _begin, std::set<ooctools::Quintuple>::iterator _end, unsigned int _count);

	OcclusionRequestEvent(const oocframework::Message* msg);
	virtual ~OcclusionRequestEvent();
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

	/**
	 * @brief Returns the ModelViewProjection Matrix for these requests.
	 */
	const float* getMatrix() const {
		return (((float*)(mProData + sizeof(unsigned) + sizeof(ooctools::Quintuple)*getIdxCount())));

	};

	const OcclusionRequestEvent& operator=(const OcclusionRequestEvent& _rhs);
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

#endif /* OCCLUSIONREQUESTEVENT_H_ */
