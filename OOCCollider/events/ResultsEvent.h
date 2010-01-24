/**
 * @file	ResultsEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ResultsEvent class declaration.
 */

#ifndef RESULTSEVENT_H_
#define RESULTSEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class ResultsEvent
 *
 * @brief This event is thrown whenever the Master determines that the whole application
 * should be shut down.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class ResultsEvent : public oocframework::IEvent{
public:
	ResultsEvent(unsigned _count, double* _array);
	ResultsEvent(const oocframework::Message* msg);
	virtual ~ResultsEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	int getCount() const {return ((unsigned*)mProData)[0];};
	const double* getResults() const {return ((double*)(mProData+sizeof(unsigned)));};
	double getResult(unsigned _idx) const {return ((double*)(mProData+sizeof(unsigned)))[_idx];};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* RESULTSEVENT_H_ */
