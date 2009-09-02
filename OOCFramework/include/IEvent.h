/**
 * @file	IEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2009
 * @brief 	IEvent class declaration.
 */

#ifndef IEVENT_H_
#define IEVENT_H_

#include <string>

#include "declarations.h"
#include "ClassId.h"

namespace oocframework {

/**
 * @class IEvent
 * @brief The interface for any event.
 */
class IEvent {
public:
	IEvent();
	virtual ~IEvent();

	/**
	 * @brief Checks whether this object is an instance of the given ClassId.
	 * This method must be implemented by all Events, because its virtuality.
	 * The code should be exactly the same as in this interface.
	 */
	virtual bool instanceOf(const ClassId* cId) const;

	/**
	 * @brief Returns the ClassId of the calling object. Watch it, though.
	 * This is not virtual. so if you call this on a downcasted object,
	 * it will return the id of the downcast class.
	 * For validation always use instanceOf()!
	 */
	static const ClassId* classid();

	/**
	 * @brief Returns the correct ClassId of an event. No matter how the current
	 * object is casted.
	 * @return the ClassId
	 */
	virtual const oocframework::ClassId* getClassId(){return mClassId;};

	/**
	 * @brief Returns the accumulated size in bytes this particular event has.
	 * Due to virtuality every heir of this class has to implement the correct size
	 * on its own. This method was introduced to make conversions between events an messages
	 * easier. For the same reason the method getData() was introduced. getByteSize() informs you
	 * how many bytes you'll get via getData().
	 * @return the size of the sum of all event's fields in bytes.
	 */
	virtual unsigned getByteSize();

	/**
	 * @brief Returns a raw byte-pointer to all the events' data fields.
	 * This has to be implemented by all heirs. Ensures simplicity when converting
	 * to and fro events and messages.
	 * @return a const pointer to all data field of the particular event.
	 */
	const char* getData() const;
protected:
	/**
	 * @brief Static field that holds the ClassId of this object.
	 */
	static ClassId* mClassId;

	/**
	 * @brief Simple shortcut for setting the static ClassId in case of multiple
	 * constructors.
	 * @todo Needs verification that this method is still needed after changing the
	 * classid mechanism a little bit.
	 */
	virtual void init();

	char* mProData;
	static unsigned mProByteSize;

};

}

#endif /* IEVENT_H_ */
