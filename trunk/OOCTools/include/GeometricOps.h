/**
 * @file	GeometricOps.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 10.05.2009
 * @brief 	GeometricOps class declaration.
 */

#ifndef GEOMETRICOPS_H_
#define GEOMETRICOPS_H_

#include <GL/glew.h>

#include "declarations.h"

namespace ooctools{

/**
 * @class GeometricOps
 * @brief Collection class for static geometric operations such as triangle-center calculations or matrix inversions.
 */
class GeometricOps {
public:
	GeometricOps();
	virtual ~GeometricOps();

	static GLdouble PI;

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

	/**
	 * @brief Efficiently transposes a 4x4 matrix bye swapping the entries above the diagonal with
	 * the entries below the diagonal.
	 * @param _mat The matrix which will be transposed. Will contain the transposed matrix as well.
	 * @note This code was taken from the source of the Doom3-Engine, which has been released under the GPL. (http://www.iddevnet.com/doom3)
	 */
	static void transposeMat4( float* _mat );

	/**
	 * @brief Calculates current eye-position by transposing and inverting the given ModelView matrix.
	 * Transpose is neccessary because OpenGL matrices are column-major, whereas DirectX- and Doom3-Matrices are row-major.
	 * Code performs in 84+4+16 = 104 multiplications and 1 division.
	 * @param _mat The current ModelView matrix.
	 * @param eye A reference to the container where the eye-position will be stored.
	 * @note This code is a modified version from the source of the Doom3-Engine, which has been released under the GPL. (http://www.iddevnet.com/doom3)
	 */
	static bool calcEyePosition( float* _mat , V3f& eye, V3f& view);

	/**
	 * @brief Calculates current eye-position by transposing and fast-inverting the given ModelView matrix.
	 * Transpose is neccessary because OpenGL matrices are column-major, whereas DirectX- and Doom3-Matrices are row-major.
	 * Code performs in 6*8+2*6 = 60 multiplications and 2*1 =  2 divisions.
	 * @param _mat The current ModelView matrix.
	 * @param eye A reference to the container where the eye-position will be stored.
	 * @note This code is a modified version from the source of the Doom3-Engine, which has been released under the GPL. (http://www.iddevnet.com/doom3)
	 */
	static bool calcEyePosFast( float* _mat, V3f& eye, V3f& view );

	/**
	 * @brief Calculates absolute value of given float.
	 * @note This code was taken from the source of the Doom3-Engine, which has been released under the GPL. (http://www.iddevnet.com/doom3)
	 */
	static float Fabs( float f );
};

} // ooctools
#endif /* GEOMETRICOPS_H_ */
