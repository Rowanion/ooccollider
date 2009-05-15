/**
 * @file	EventManager.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2009
 * @brief 	EventManager class declaration.
 */

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <map>
#include <vector>
#include <set>

#include "declarations.h"
#include "AbstractEventListener.h"
#include "IEvent.h"
#include "ClassId.h"

namespace oocframework {

/**
 * @class EventManager
 * @brief This is the place where you submit new Events and register and unregister
 * yourself as a event listener. It's a singleton.
 */
class EventManager {
public:
	virtual ~EventManager();

	/**
	 * @brief This adds a new listener to the set for the given ClassId.
	 */
	virtual void addListener(AbstractEventListener* listener, const ClassId* eventClass);

	/**
	 * @brief This removes the given listener for the specified EventClassId.
	 */
	virtual void removeListener(AbstractEventListener* listener, const ClassId* eventClass);

	/**
	 * @brief This removes the given listener for all registered events.
	 */
	virtual void removeListener(AbstractEventListener* listener);

	/**
	 * @brief Fires the given event into all registered recipients.
	 */
	void fire(IEvent& event);

	/**
	 * @brief Returns the single instance of this manager.
	 */
	static EventManager* getSingleton();

protected:
	typedef std::set<AbstractEventListener*> LSet;
	typedef std::map<const ClassId*, LSet > LMap;
	LMap mListenerMap;


private:
	static EventManager* instance;
	EventManager();
};

}

#endif /* EVENTMANAGER_H_ */
