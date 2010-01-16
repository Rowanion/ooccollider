/**
 * @file	VboDistributionEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	VboDistributionEvent class declaration.
 */

#ifndef VBODISTRIBUTIONEVENT_H_
#define VBODISTRIBUTION_H_

#include <GL/gl.h>

#include <set>

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"

/**
 * @class VboDistributionEvent
 *
 * @brief This event serves as container-event for a colorbuffer from a Render-Node.
 *
 * This event originates from a Render-Node and is sent to the Master-Node.
 *
 */
class VboDistributionEvent : public oocframework::IEvent{
public:
	VboDistributionEvent();
	VboDistributionEvent(const std::set<uint64_t>& _idSet);
	VboDistributionEvent(const oocframework::Message* msg);
	virtual ~VboDistributionEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Returns the x-rasterposition of the framebuffer-section.
	 */
	uint64_t getIdCount() const {return ((uint64_t*)mProData)[0];};
	const uint64_t* getIdArray() const {return ((uint64_t*)(mProData+sizeof(uint64_t)));};

	/**
	 * @brief Returns the y-rasterposition of the framebuffer-section.
	 */
	uint64_t getId(uint64_t _id) const {return ((uint64_t*)mProData)[_id+1];};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* VBODISTRIBUTIONEVENT_H_ */
