/**
 * @file	EventManager.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2009
 * @brief 	EventManager class definition.
 */

#include "EventManager.h"

#include <map>
#include <vector>
#include <iostream>

#include "declarations.h"
#include "AbstractEventListener.h"

using namespace std;

namespace oocframework {

EventManager* EventManager::instance = 0;

EventManager* EventManager::getSingleton()
{
	if 	(EventManager::instance == 0){
		EventManager::instance = new EventManager();
	}
	return EventManager::instance;
}

EventManager::EventManager() {
	// TODO Auto-generated constructor stub

}

EventManager::~EventManager() {
	// TODO Auto-generated destructor stub
}

void EventManager::addListener(AbstractEventListener* listener, const ClassId* eventClass)
{
	mListenerMap[eventClass].insert(listener);
}

void EventManager::removeListener(AbstractEventListener* listener, const ClassId* eventClass)
{
	LMap::iterator mIter = mListenerMap.find(eventClass);
	if (mIter != mListenerMap.end()){
		mIter->second.erase(listener);
	}
}

void EventManager::removeListener(AbstractEventListener* listener)
{
    for(LMap::iterator mIter = mListenerMap.begin(); mIter != mListenerMap.end(); ++mIter)
    	mIter->second.erase(listener);
}

void EventManager::fire(IEvent& event) {
	/*
	 * for each listener registered for this type.....
	 * notify.
	 * map<ClassId*, vector<AbtractEventListener*> >
	 */

	for (LMap::const_iterator mcIter = mListenerMap.begin(); mcIter!= mListenerMap.end(); ++mcIter){
		if (event.instanceOf(mcIter->first)){
			for (LSet::const_iterator csIter = mcIter->second.begin(); csIter!= mcIter->second.end(); ++csIter) {
				(*csIter)->notify(event);
			}
		}
	}
}

} //oocframework
