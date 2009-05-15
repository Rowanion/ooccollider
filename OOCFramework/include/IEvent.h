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
protected:
	/**
	 * @brief Static field that holds the ClassId of this object.
	 */
	static ClassId* mClassId;

	/**
	 * @brief Simple shortcut for setting the static ClassId in case of multiple
	 * constructors.
	 */
	virtual void init();

};

}

#endif /* IEVENT_H_ */
