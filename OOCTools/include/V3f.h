/**
 * @file	V3f.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 19.02.2009
 * @brief 	V3f class declaration.
 */

#ifndef V3F_H_
#define V3F_H_
#include <string>
#include <vector>

#include "declarations.h"

namespace ooctools {

/**
 * @class V3f
 * @brief This is a vector-class with 3 float-components.
 * It's a pretty large class because of all the operators and vector-style methods defined here.
 * Together with V4f it's used the most, because 3 floats are the most usual way to move around
 * in 3D-space.
 * @see V3b, V3ub, V4f
 */
class V3f
{
public:
	V3f();
	V3f(float, float, float);
	V3f(const float* _data);
	V3f(float);
	V3f(const V3f &_v);
	V3f(const V4f &_v);
//	V3f(const V3f *_v);
	virtual ~V3f();
	float *x;
	float *y;
	float *z;
	static const V3f max(const V3f &_a, const V3f &_b);
	static const V3f min(const V3f &_a, const V3f &_b);
	void minMe(const V3f &_a);
	void maxMe(const V3f &_a);
	void minMe(float _x, float _y, float _z);
	void maxMe(float _x, float _y, float _z);
	const float getX() const {return mPriData[0];};
	const float getY() const {return mPriData[1];};
	const float getZ() const {return mPriData[2];};
	const float getR() const {return mPriData[0];};
	const float getG() const {return mPriData[1];};
	const float getB() const {return mPriData[2];};
	const float* getData() const {return (const float*) mPriData;};
	void setX(float f){mPriData[0]=f;};
	void setY(float f){mPriData[1]=f;};
	void setZ(float f){mPriData[2]=f;};
	const std::string toString() const;
	bool hasSharedComponent(const V3f &_v) const;
	float getMaxComponent()const;
	float getMinComponent()const;

	/**
	 * @brief Performs normalization on this vector and returns a reference to itself.
	 * A vector is normalized by dividing it through its magnitude, ie.
	 * @f[ \frac{\left( \begin{array}{c}
	 * x \\
	 * y \\
	 * z
	 * \end{array}\right)}
	 * {\sqrt{(x^2 + y^2 + z^2}} @f]
	 *
	 * @return normalized self
	 */
	V3f& normalize();

	void addFaceRef(Face *face);
	V3f getAbs() const;

	/**
	 * @brief Calculates the magnitude of a vector.
	 * @return magnitude, ie. @f$ \sqrt{(x^2 + y^2 + z^2} @f$
	 *
	 */
	float calculateMagnitude();
	float dot(const V3f& rhs)const;

	bool operator>(const V3f &a) const;
//	bool operator>(V3f *a);
	bool operator>=(const V3f& a) const;
//	bool operator>=(V3f *a);
	bool operator<(const V3f &a) const;
//	bool operator<(const V3f *a);
	bool operator<=(const V3f& a) const;
//	bool operator<=(V3f *a);
	bool operator==(const V3f &a);
//	bool operator==(V3f *a);
	bool operator!=(V3f &a);
	V3f& operator=(const V3f &a);
	V3f& operator=(const V4f &a);
	V3f& operator/=(const float f);
	V3f& operator+=(const V3f &a);
	V3f& operator+=(const float f);
	V3f& operator*=(float f);
	V3f& operator*=(const V3f& v);
	V3f& operator-=(const V3f &a);
	V3f operator/(float f) const;
	V3f operator+(const V3f &a) const;
	V3f operator-(const V3f &a) const;
	V3f operator*(float f) const;

	/**
	 * @brief Returns the cross-product of the two input-vectors.
	 * It's a static function.
	 * @param a reference of the first vector
	 * @param b reference of the second vector
	 * @return V3f object containing the cross product, ie.
	 * \f$ \left( \begin{array}{c}
	 * y_a*z_b - z_a*y_b \\
	 * z_a*x_b - x_a*z_b \\
	 * x_a*y_b - y_a*x_b
	 * \end{array}\right) \f$ .
	 */

	static V3f cross(const V3f &a, const V3f &b);
	static void calcFNormal(Face *face);
	static bool hasSharedComponent(const V3f& _v1, const V3f& _v2);
	static std::string toString(const float* _array);
	float calcDistance(const V3f& point) const;
	float calcSimpleDistance(const V3f& point) const;
	static float calcSimpleDistance(const V3f& p1, const V3f& p2);

	// back-reference for vertex-normal-calculation. DO NOT DELETE, DUH!
	std::vector<Face*> *mPriFaceList;

private:
	float mPriData[3];
	int mPriCount;
};

} // Namespace ModelConverter

#endif /* V3F_H_ */
