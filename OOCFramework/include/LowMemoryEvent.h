/**
 * @file	LowMemoryEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	LowMemoryEvent class declaration.
 */

#ifndef LOWMEMORYEVENT_H_
#define LOWMEMORYEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

namespace oocframework{

/**
 * @class LowMemoryEvent
 *
 * @brief This event is thrown whenever the Master determines that the whole application
 * should be shut down.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class LowMemoryEvent : public IEvent{
public:
	LowMemoryEvent();
	virtual ~LowMemoryEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

}
#endif /* LOWMEMORYEVENT_H_ */
