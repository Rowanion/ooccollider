/**
 * @file	ModelViewMatrixEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ModelViewMatrixEvent class declaration.
 */

#ifndef MODELVIEWMATRIXEVENT_H_
#define MODELVIEWMATRIXEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"


/**
 * @class ModelViewMatrixEvent
 *
 * @brief This event serves as container-event for the ModelviewMatrix and adjustments to tile-dimensions from the master-Node.
 * It is transmitted round about every frame to all other nodes. Because all input happens
 * at the Master-Node, the input is applied to the local Transformation/Rotation Matrix, which is
 * then sent to the other nodes. There it is multiplied on the identity matrix.
 * Due to the regular recurrence of this event, I took the liberty to add the adjustments of the tile-dimensions
 * also into this event.
 * Serves as frame for equivalent messages.
 *
 * This event originates from the Master-Node and will be submitted to all the other nodes.
 *
 */
class ModelViewMatrixEvent : public oocframework::IEvent{
public:
	ModelViewMatrixEvent();
	ModelViewMatrixEvent(const float* matrix);
	ModelViewMatrixEvent(const oocframework::Message* msg);
	virtual ~ModelViewMatrixEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	const float* getMatrix() const {return (float*)mProData;};

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* MODELVIEWMATRIXEVENT_H_ */
