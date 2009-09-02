/**
 * @file	Triangle.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 16.05.2009
 * @brief 	Triangle class declaration.
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <string>

#include "declarations.h"
#include "BoundingBox.h"
#include "StructDefs.h"

namespace ooctools {

/**
 * @class Triangle
 * @brief Container-Class for a single triangle.
 * This is used while constructing the final octree.
 */
class Triangle {
public:
	Triangle();
	Triangle(const float* vertices, const char* normals);
	Triangle(const Triangle& rhs);
	Triangle(const V4N4& a, const V4N4& b, const V4N4& c);
	virtual ~Triangle();
	const float* getVertices() const;
	const char* getNormals() const;
	void setData(const float* vertices, const char* normals);
	void setVertices(const float* vertices);
	void setNormals(const char* normals);
	float getArea() const {return mArea;};
	const BoundingBox& getBb() const;
	const std::string toString() const;
	void sort();
	float calcArea();

	Triangle& operator=(const Triangle& rhs);
	bool operator==(const Triangle& rhs) const;
	bool operator!=(const Triangle& rhs) const ;
	bool operator<(const Triangle& rhs) const;

private:
	float calcDistance(const float* a, const float* b);
	void calcBb();

	float* mVertices;
	char* mNormals;
	float mArea;
	BoundingBox mBb;

};

}

#endif /* TRIANGLE_H_ */
