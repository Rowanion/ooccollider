/**
 * @file	KeyPressedEvent.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	KeyPressedEvent class declaration.
 */

#ifndef KEYPRESSEDEVENT_H_
#define KEYPRESSEDEVENT_H_

#include "declarations.h"
#include "IEvent.h"
#include "ClassId.h"
#include "Message.h"

/**
 * @class KeyPressedEvent
 *
 * @brief This event is thrown whenever the Window-Class registers that a key has been pressed.
 *
 * This event originates from the Master-Node.
 *
 */
class KeyPressedEvent : public oocframework::IEvent{
public:
	KeyPressedEvent();
	KeyPressedEvent(int key, char _lShift, char _rShift, char _lCtrl, char _rCtrl, char _lAlt, char _rAlt);
	KeyPressedEvent(const oocframework::Message* msg);
	virtual ~KeyPressedEvent();
	static const oocframework::ClassId* classid();
	virtual const oocframework::ClassId* getClassId(){return mClassId;};
	virtual bool instanceOf(const oocframework::ClassId* cId) const;

	/**
	 * @brief Simply returns the key which has been pressed.
	 * For which key is which have a look at the GLFW-Documentation. There's a bunch of predefined
	 * key-defines. But raw chars are transmitted as well.
	 * @note I haven't found out yet how to tell lower-case and upper-case letters apart, because
	 * GLFW seems always to submit the upper-case versions.
	 */
	int getKey() const {return ((int*)mProData)[0];};

	bool isLShift();
	bool isRShift();
	bool isShift();
	bool isLCtrl();
	bool isRCtrl();
	bool isCtrl();
	bool isLAlt();
	bool isRAlt();
	bool isAlt();

protected:
	static oocframework::ClassId* mClassId;
	virtual void init();
private:
};

#endif /* KEYPRESSEDEVENT_H_ */
