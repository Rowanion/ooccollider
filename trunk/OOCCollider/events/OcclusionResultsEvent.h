/**
 * @file	OcclusionResultsEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	OcclusionResultsEvent class declaration.
 */

#ifndef OCCLUSIONRESULTSEVENT_H_
#define OCCLUSIONRESULTSEVENT_H_

#include <vector>
#include <map>
#include <set>
#include <list>

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"
#include "StructDefs.h"

/**
 * @class OcclusionResultsEvent
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
class OcclusionResultsEvent : public oocframework::IEvent{
public:
//	OcclusionResultsEvent();
	OcclusionResultsEvent(const OcclusionResultsEvent& _nre);
	OcclusionResultsEvent(const std::list<ooctools::Visibility>& _pairList);

	OcclusionResultsEvent(const oocframework::Message* msg);
	virtual ~OcclusionResultsEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Returns the requested ID at index idx.
	 */
	uint64_t getId(unsigned idx) const {
		return ((uint64_t*)(mProData + sizeof(unsigned) + sizeof(uint64_t)*idx + sizeof(char)*idx))[0];
	};

	/**
	 * @brief Returns the requested byte at index idx.
	 */
	char getVisibility(unsigned idx) const {
		return ((char*)(mProData + sizeof(unsigned) + sizeof(uint64_t)*(idx+1) + sizeof(char)*idx))[0];
	};

	/**
	 * @brief Returns the number of results contained in this event.
	 */
	unsigned getIdxCount() const {
		return ((unsigned*)mProData)[0];
	};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* OCCLUSIONRESULTSEVENT_H_ */
