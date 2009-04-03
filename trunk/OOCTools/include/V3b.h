/**
 * @file	V3b.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 *
 */

#ifndef V3B_H_
#define V3B_H_

#include <string>

#include "declarations.h"

namespace ooctools {

/**
 * @class C
 * @brief B
 * L
 */
class V3b
{
public:
	V3b();
	V3b(char _x, char _y, char _z);
	V3b(char *_data);
	V3b(char);
	virtual ~V3b();
	char *x;
	char *y;
	char *z;
	char getX(){return mPriData[0];};
	char getY(){return mPriData[1];};
	char getZ(){return mPriData[2];};
	void setX(char b){mPriData[0]=b;};
	void setY(char b){mPriData[1]=b;};
	void setZ(char b){mPriData[2]=b;};
	const char *getData();
	std::string toString();


private:
	char *mPriData;
	int mPriCount;
};

} // NameSpace OOCTools

#endif /* V3B_H_ */
