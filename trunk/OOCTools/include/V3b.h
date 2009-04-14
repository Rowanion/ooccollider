/**
 * @file	V3b.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 * @brief 	V3b class declaration.
 *
 */

#ifndef V3B_H_
#define V3B_H_

#include <string>

#include "declarations.h"

namespace ooctools {

/**
 * @class V3b
 * @brief This is a vector-class with 3 byte(or char)-components.
 * It's got only a very limited number of operations and methods defined, because I use
 * it only to store vertex-normals in it, which don't do very much, except pointing
 * away from something. For more operations have a look at the other vector classes.
 * Important notice: Because of the GPU's craving for PowerOf4-Data, we changed our normal-
 * vectors to char4. This is not reflected in this library's vertex-classes, because
 * we don't draw directly from these vertices (well...maybe during development or when
 * debugging). When the vertices and normals from a model are being converted into
 * VertexArrays, the 4th component will be filled with fake-data. Keep on reading
 * there, if you like know more.
 *
 * @see V3f, V3ub, V4f, VertexArray
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
