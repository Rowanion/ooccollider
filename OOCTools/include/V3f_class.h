/*
 * V3f.h
 *
 *  Created on: 30.01.2009
 *      Author: ava
 */

#ifndef V3F_H_CLASS_
#define V3F_H_CLASS_

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>

namespace ooctools_ {

class V3f_class {
public:
	V3f_class();
	V3f_class(float _x, float _y, float _z);
	V3f_class(float *tempVal);
	V3f_class(V3f_class *tempVal);
	virtual ~V3f_class();

	void add(float b[]);
	void add(V3f_class *b);
	void add(float x,float y, float z);
	void add(float scalar);
	static V3f_class *sub(V3f_class *a, V3f_class *b);
	void mul(float *b);
	void mul(V3f_class *b);
	void mul(float x,float y, float z);
	void mul(float scalar);
	static V3f_class *mul(V3f_class *a, V3f_class *b);
	static V3f_class *mul(V3f_class *a, float b);
	static float dot(float *a, float *b);
	static float dot(V3f_class *a, V3f_class *b);
	float dot(V3f_class *a);
	static float dot(V3f_class *a, float *b);
	V3f_class *cross (V3f_class *b);
	static V3f_class *cross (V3f_class *a, V3f_class *b);
	const float *asArray();
	std::vector<GLfloat> *asVector();
	void setV3f(V3f_class *tempVal);
	void setV3f(float *tempVal);
	void setV3f(float x, float y, float z);
	void normalize();
	V3f_class *getNormalized();
	float getLength();
	float getSquaredLength();
	void LoadZero();
	void LoadOne();
	//linear interpolate
	static V3f_class *lerp(V3f_class *p1, V3f_class *p2, float percent);
	V3f_class *lerp(V3f_class *v2, float factor);
	V3f_class *quadraticInterpolate(V3f_class *v2, V3f_class *v3, float factor);
	std::string toString();
	void packTo01();
	V3f_class *getPackedTo01();
	void unPackTo11();
	V3f_class *getUnPackedTo11();
	V3f_class *getRotatedX(double angle);
	void rotateX(double angle);
	V3f_class *getRotatedY(double angle);
	void rotateY(double angle);
	V3f_class *getRotatedZ(double angle);
	void rotateZ(double angle);
	V3f_class *getRotatedAxis(double angle, V3f_class *axis);
	void rotateAxis(double angle, V3f_class *axis);

	void operator+=(float scalar);
	void operator+=(V3f_class * v);

	float x;
    float y;
    float z;

private:
    float data[3];
    void updateArray();


};

} // OOCTools_
#endif /* V3F_H_ */
