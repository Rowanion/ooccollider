/**
 * @file	KillApplicationEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	KillApplicationEvent class declaration.
 */

#ifndef KILLAPPLICATIONEVENT_H_
#define KILLAPPLICATIONEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class KillApplicationEvent
 *
 * @brief This event is thrown whenever the Master determines that the whole application
 * should be shut down.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class KillApplicationEvent : public oocframework::IEvent{
public:
	KillApplicationEvent();
	virtual ~KillApplicationEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	virtual unsigned getByteSize(){return KillApplicationEvent::mProByteSize;};

protected:
	static oocframework::ClassId* mClassId;
	static unsigned mProByteSize;
	virtual void init();
private:
};

#endif /* KILLAPPLICATIONEVENT_H_ */
