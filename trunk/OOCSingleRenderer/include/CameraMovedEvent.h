/**
 * @file	CameraMovedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	CameraMovedEvent class declaration.
 */

#ifndef CAMERAMOVEDEVENT_H_
#define CAMERAMOVEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class CameraMovedEvent
 *
 * @brief This event is thrown whenever the camera was moved. Unlike most other events,
 * this will probably not sent over the net to other nodes.
 */
class CameraMovedEvent : public oocframework::IEvent{
public:
	CameraMovedEvent();
	virtual ~CameraMovedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* CAMERAMOVEDEVENT_H_ */
