/**
 * @file	ClassId.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.05.2009
 * @brief 	ClassId class definition.
 */

#include "ClassId.h"

#include <string>
#include <iostream>

#include "EventManager.h"

using namespace std;

namespace oocframework {

unsigned int ClassId::mCrctab[256];
bool ClassId::initiated = false;;

ClassId::ClassId(std::string name) : mClassName(name) {
	if (!ClassId::isInited()){
		ClassId::crc32_init();
	}
	mCrcId = encode(name);
}

ClassId::~ClassId() {
	// TODO Auto-generated destructor stub
}

void ClassId::crc32_init(void) {
	int i, j;
	unsigned int crc;

	for (i = 0; i < 256; i++) {
		crc = i << 24;
		for (j = 0; j < 8; j++) {
			if (crc & 0x80000000)
				crc = (crc << 1) ^ QUOTIENT;
			else
				crc = crc << 1;
		}
		ClassId::mCrctab[i] = crc;
	}
	ClassId::initiated = true;
}

unsigned int ClassId::encode(const unsigned char *data, int len) const
{
    unsigned int        result;
    int                 i;

    if (!len < 4){
    	result = *data++ << 24;
		result |= *data++ << 16;
		result |= *data++ << 8;
		result |= *data++;
		result = ~result;
		len -= 4;
	}
    for (i=0; i<len; i++)
    {
        result = (result << 8 | *data++) ^ ClassId::mCrctab[result >> 24];
    }
    return ~result;
}

unsigned int ClassId::encode(const std::string& name) const
{
	return encode((unsigned char*)name.c_str(), name.size());
}

bool ClassId::operator==(const ClassId& rhs)
{
	return (mCrcId == rhs.getId());
}

bool ClassId::operator!=(const ClassId& rhs)
{
	return (mCrcId != rhs.getId());
}

} // oocframework
