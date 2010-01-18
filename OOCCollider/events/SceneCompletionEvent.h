/**
 * @file	SceneCompletionEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	SceneCompletionEvent class declaration.
 */

#ifndef SCENECOMPLETIONEVENT_H_
#define SCENECOMPLETIONEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class SceneCompletionEvent
 *
 * @brief This event is thrown whenever the Master determines that the whole application
 * should be shut down.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class SceneCompletionEvent : public oocframework::IEvent{
public:
	SceneCompletionEvent();
	virtual ~SceneCompletionEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* SCENECOMPLETIONEVENT_H_ */
