/**
 * @file	RandomSeedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	RandomSeedEvent class declaration.
 */

#ifndef RANDOMSEEDEVENT_H_
#define RANDOMSEEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class RandomSeedEvent
 *
 * @brief This event is thrown whenever the Master determines that the whole application
 * should be shut down.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other data-nodes.
 *
 */
class RandomSeedEvent : public oocframework::IEvent{
public:
	RandomSeedEvent();
	RandomSeedEvent(unsigned int _seed);
	RandomSeedEvent(const oocframework::Message* msg);
	virtual ~RandomSeedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	unsigned int getSeed() const {return ((unsigned int*)mProData)[0];};
protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* RANDOMSEEDEVENT_H_ */
