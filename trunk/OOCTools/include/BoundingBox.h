/**
 * @file	BoundingBox.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.02.2009
 * @brief 	BoundingBox class declaration.
 */


#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "V3f.h"

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "declarations.h"
#include "TriBoxTest.h"

namespace fs = boost::filesystem;
namespace ooctools {

/**
 * @class BoundingBox
 *
 * @brief As the name suggests, this represents a bounding-box of an object.
 *
 * This boundingbox is represented by the minimum and maximum V3f vertices. Class contains
 * elemental functionality to draw it via opengl; either as GL_LINES or GL_QUAD.
 * Some intersection tests are included as well.
 */
class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(float _value);
	BoundingBox(float _valA, float _valB);
	BoundingBox(const V3f& _vA, const V3f& _vB);
	BoundingBox(const V4f& _vA, const V4f& _vB);
	BoundingBox(fs::path bbFile);
	BoundingBox(std::string bbFile);
	BoundingBox(const BoundingBox& _bb);
	virtual ~BoundingBox();
	void draw() const;
	void draw(float _r, float _g, float _b) const;
	void draw(int _r, int _g, int _b) const;
	void draw(float* _rgb) const;
	void expand(const V3f& _v);
	void expand(const V4f& _v);
	void expand(const float* _v);
	void expand(const float _x, float _y, float _z);
	void expand(const BoundingBox& _bb);
	const V3f& getMin() const {return mPrivMin;};
	const V3f& getMax() const {return mPrivMax;};
	std::string toString() const;
	bool hasSharedComponent(const BoundingBox& _bb) const;
	void drawSolid() const;
	void saveToFile(fs::path bbFile);
	void saveToFile(std::string bbFile) const;
	static BoundingBox fromFile(fs::path bbFile);

	/**
	 * @brief Resets the BB to it's initial values.
	 * <ul>
	 * <li>mPrivMin = numeric_limits<float>::max()</li>
	 * <li>mPrivMax = -numeric_limits<float>::max()</li>
	 * </ul>
	 * This saves us from continued re-instantiation.
	 */
	void reset() const ;

	/**
	 *  @brief Computes center of BB.
	 *  @return   a pointer to a new V3f Reference containing the center coordinates of BB.
	 *
	 *  This is not an ordinary getter. This calculates the geometrical center of the BoundingBox
	 *  and returns a pointer to it.
	 *  Attention: the pointer is NOT kept by BB. The caller of the function has to make sure to
	 *  clean it up after use!
	 */
	void computeCenter(V3f& _center) const;

	/**
	 * @brief Performs a simple intersection-test between two Axis-ALigned BoundingBoxes.
	 * @param _b
	 * @return true if the boxes intersect.
	 */
	bool intersects(const BoundingBox& _b) const;

	/**
	 * @brief A wrapper method to call the c-function triboxoverlap.c which performs an intersection-test
	 * between this BoundingBox and the given triangle.
	 * @param _a vertex a
	 * @param _b vertex b
	 * @param _c vertex c of the triangle
	 * @return true if the triangle either intersects <b>or is completely inside</b> the
	 * BoundingBox.
	 */
	bool intersects(const V3f& _a, const V3f& _b, const V3f& _c) const;

	/**
	 * Same as the other triangle-intersection method but this time with V3f Vertices.
	 * @param _a vertex a
	 * @param _b vertex b
	 * @param _c vertex c of the triangle
	 * @return true if the triangle either intersects <b>or is completely inside</b> the
	 * BoundingBox.
	 */
	bool intersects(const V4f& _a, const V4f& _b, const V4f& _c) const;

	bool intersects9Plus(const float* _a) const;

	/**
	 * Checks if the given vertex with 3 components is inside this BB.
	 * @param _v pointer to a float array
	 * @return true if Point is inside.
	 */
	bool hasInside3(const float* _v) const;

	bool hasInside3Plus(const float *_v) const;

	/**
	 * Checks if the given vertices with 3 components are all inside this BB.
	 * @param _v pointer to a float array containing 3x3 floats
	 * @return true if all Points are inside.
	 */
	bool hasInside9(const float* _v) const;

	bool hasInside9Plus(const float *_v) const;

	/**
	 * same as hasInside3() only with single floats.
	 * @see hasInside3()
	 */
	bool hasInside(float _f1, float _f2, float _f3) const;

	/**
	 * same as hasInside9() only with single V3f references.
	 * @ see hasInside9()
	 */
	bool hasInside(const V3f& _v1, const V3f& _v2, const V3f& _v3) const;

	/**
	 * same as hasInside9() only with single V4f references.
	 * @ see hasInside9()
	 */
	bool hasInside(const V4f& _v1, const V4f& _v2, const V4f& _v3) const;

	/**
	 * same as hasInside3() only with a single V3f reference.
	 * @see hasInside3()
	 */
	bool hasInside(const V3f& _v) const;

	/**
	 * same as hasInside3() only with a single V4f reference.
	 * @see hasInside3()
	 */
	bool hasInside(const V4f& _v) const;

	/**
	 * Performs a text if the given BoundingBox is completely inside this one.
	 * @param _b the rhs-BB
	 * @return true if is completely inside.
	 */
	bool hasInside(const BoundingBox& _b) const;

	void computeEdgeSizes(V3f& _sizes) const;
	float computeDiameter() const;

	/**
	 * Assignment Operator
	 */
	BoundingBox& operator=(const BoundingBox& _bb);

	/**
	 * equality comparison operator
	 */
	bool operator==(const BoundingBox& _bb);
	bool operator!=(const BoundingBox& _bb);

	static bool hasSharedComponent(const BoundingBox& _bb1, const BoundingBox& _bb2);

private:
	// member variables
	V3f mPrivMin;
	V3f mPrivMax;
	V3f mPrivEdgeSizes;
	V3f mPrivCenter;
	TriBoxTest mTriBoxTest;

	// member methods
	void drawImmediate() const;
	void drawLineStrip() const;

};

} // end of Namespace OOCTools

#endif /* BOUNDINGBOX_H_ */
