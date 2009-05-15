/**
 * @file	IEvent.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 11.05.2009
 * @brief 	IEvent class definition.
 */

#include "IEvent.h"

#include <string>
#include <iostream>

#include "declarations.h"
#include "ClassId.h"

namespace oocframework {

oocframework::ClassId* IEvent::mClassId = 0;

IEvent::IEvent()
{
	init();
}

IEvent::~IEvent()
{
}

void
IEvent::init()
{
	if (mClassId == 0){
		mClassId = new oocframework::ClassId("oocframework::IEvent");
	}
}

bool
IEvent::instanceOf(const ClassId* cId) const
{
	return (mClassId->getId() == cId->getId());
}

const oocframework::ClassId* IEvent::classid()
{
	return mClassId;
}

} // oocframework
