/**
 * @file	ClearCacheEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ClearCacheEvent class declaration.
 */

#ifndef CLEARCACHEEVENT_H_
#define CLEARCACHEEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class ClearCacheEvent
 *
 * @brief This event is thrown whenever the Master determines that the whole application
 * should be shut down.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class ClearCacheEvent : public oocframework::IEvent{
public:
	ClearCacheEvent();
	virtual ~ClearCacheEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* CLEARCACHEEVENT_H_ */
