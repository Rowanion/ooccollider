/**
 * @file	V3ub.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 * @brief 	V3ub class declaration.
 */

#ifndef V3UB_H_
#define V3UB_H_

#include <string>

#include "declarations.h"

namespace ooctools {

/**
 * @class V3ub
 * @brief This is a vector-class with 3 unsigned byte(or uchar)-components.
 * It's got only a very limited number of operations and methods defined, because I use
 * it only to store colors in it, which don't do very much. For more operations
 * have a look at the other vector classes.
 * @see V3b, V3f, V4f,
 */
class V3ub
{
public:
	V3ub();
	V3ub(unsigned char _x, unsigned char _y, unsigned char _z);
	V3ub(float _x, float _y, float _z);
	V3ub(const unsigned char* _data);
	V3ub(unsigned char c);
	V3ub(const V3ub& _v);
	V3ub(const V3f& _v);
	virtual ~V3ub();
	unsigned char* x;
	unsigned char* y;
	unsigned char* z;
	const unsigned char getX()const {return mPriData[0];};
	const unsigned char getY()const {return mPriData[1];};
	const unsigned char getZ()const {return mPriData[2];};
	const unsigned char* getData();
	std::string toString() const;

	bool operator==(V3ub& a);
	bool operator!=(V3ub& a);
	V3ub& operator=(const V3ub& a);

private:
	unsigned char mPriData[3];
	int mPriCount;
};

} // NameSpace OOCTools

#endif /* V3UB_H_ */
