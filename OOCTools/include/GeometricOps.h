/**
 * @file	GeometricOps.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 10.05.2009
 * @brief 	GeometricOps class declaration.
 */

#ifndef GEOMETRICOPS_H_
#define GEOMETRICOPS_H_

#include "declarations.h"

namespace ooctools{

class GeometricOps {
public:
	GeometricOps();
	virtual ~GeometricOps();
	static void calcTriangleCenter(const V3f& a, const V3f& b, const V3f& c, V3f& center);
	/**
	 * @brief Calculates the center of given triangle.
	 * @warning Remember though, it truncates the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static void calcTriangleCenter(const V4f& a, const V4f& b, const V4f& c, V3f& center);
	static void calcTriangleCenter3(const float* triangle, V3f& center);
	/**
	 * @brief Calculates the center of given triangle.
	 * @warning Remember though, it truncates the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static void calcTriangleCenter4(const float* triangle, V3f& center);

	static float calcMaxTriangleEdge(const V3f& a, const V3f& b, const V3f& c);
	/**
	 * @brief Calculates the maximal edge of given Triangle.
	 * @warning Remember though, it truncates the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static float calcMaxTriangleEdge(const V4f& a, const V4f& b, const V4f& c);
	static double calcMaxTriangleEdge3(const float* triangle);
	/**
	 * @brief Calculates the maximal edge of given Triangle.
	 * @warning Remember though, it truncates the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static double calcMaxTriangleEdge4(const float* triangle);

	static float calcMinTriangleEdge(const V3f& a, const V3f& b, const V3f& c);
	/**
	 * @brief Calculates the minimal edge of given Triangle.
	 * @warning Remember though, it truncates the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static float calcMinTriangleEdge(const V4f& a, const V4f& b, const V4f& c);
	static long double calcMinTriangleEdge3(const float* triangle);
	/**
	 * @brief Calculates the minimal edge of given Triangle.
	 * @warning Remember though, it truncates the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static long double calcMinTriangleEdge4(const float* triangle);

	static float calcLineLength(const V3f& a, const V3f& b);
	static float calcLineLength(const V4f& a, const V4f& b);
	/**
	 * @brief Calculates the distance between given points.
	 * @warning Remember though, it ignores the 4th component. In our case
	 * the 4th component will only contain a bogus number of no relevance.
	 */
	static long double calcLineLength3(const float* a, const float* b);

	/**
	 * @brief Calculates the area of the given triangle, which consists
	 * of 4-component vectors, but the 4th component is being ignored.
	 * @f$ \frac{1}{4}\sqrt{(a+b-c)(a-b+c)(b+c-a)(a+b+c)} @f$
	 */
	static long double calcTriangleArea4(const float* triangle);

	/**
	 * @brief Calculates the edgelengths of a triangle and writes them into the edges
	 * V3f.
	 */
	static void calcTriangleEdges4(const float* triangle, V3f& edges);

	static bool calcEyePosition( float* _mat , V3f& eye);
};

} // ooctools
#endif /* GEOMETRICOPS_H_ */
