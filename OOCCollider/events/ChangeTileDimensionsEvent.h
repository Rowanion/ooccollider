/**
 * @file	ChangeTileDimensionsEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ChangeTileDimensionsEvent class declaration.
 */

#ifndef CHANGETILEDIMENSIONSEVENT_H_
#define CHANGETILEDIMENSIONSEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"

/**
 * @class ChangeTileDimensionsEvent
 *
 * @brief This event is thrown whenever the Master has requested a DepthBufferRequestEvent and the
 * render-nodes have sent their accumulated render-times. It contains the new dimensions for the
 * receiving render-node.
 *
 * This event originates from the Master-Node and will be submitted to all render-nodes.
 *
 */
class ChangeTileDimensionsEvent : public oocframework::IEvent{
public:
	ChangeTileDimensionsEvent();
	ChangeTileDimensionsEvent(int xPos, int yPos, int width, int height);
	ChangeTileDimensionsEvent(Tile& t);
	ChangeTileDimensionsEvent(const oocframework::Message* msg);
	virtual ~ChangeTileDimensionsEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;
	virtual unsigned getByteSize(){return ChangeTileDimensionsEvent::mProByteSize;};

	int getTileXPos() const{return ((int*)(mProData))[0];};
	int getTileYPos() const{return ((int*)(mProData))[1];};
	int getTileWidth() const{return ((int*)(mProData))[2];};
	int getTileHeight() const{return ((int*)(mProData))[3];};

	void setTileDimension(Tile& t);

protected:
	static oocframework::ClassId* mClassId;
	static unsigned mProByteSize;
	virtual void init();
private:
};

#endif /* CHANGETILEDIMENSIONSEVENT_H_ */
