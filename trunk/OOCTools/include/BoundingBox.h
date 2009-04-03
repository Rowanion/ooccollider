/**
 * @file	BoundingBox.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 22.02.2009
 *
 */


#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "V3f.h"

#include "declarations.h"

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
	virtual ~BoundingBox();
	void draw();
	void draw(float _r, float _g, float _b);
	void draw(int _r, int _g, int _b);
	void draw(float* _rgb);
	void expand(const V3f& _v);
	void expand(const V4f& _v);
	void expand(const float* _v);
	void expand(const float _x, float _y, float _z);
	void expand(const BoundingBox& _bb);
	const V3f& getMin() const {return mPrivMin;};
	const V3f& getMax() const {return mPrivMax;};
	std::string toString();
	bool hasSharedComponent(const BoundingBox& _bb) const;
	void drawSolid();
	void computeCenter(V3f& _center) const;
	bool intersects(const BoundingBox& _b) const;
	bool intersects(const V3f& _a, const V3f& _b, const V3f& _c) const;
	bool intersects(const V4f& _a, const V4f& _b, const V4f& _c) const;
	bool isInside3(const float* _v) const;
	bool isInside9(const float* _v) const;
	bool isInside(float _f1, float _f2, float _f3) const;
	bool isInside(const V3f& _v1, const V3f& _v2, const V3f& _v3) const;
	bool isInside(const V4f& _v1, const V4f& _v2, const V4f& _v3) const;
	bool isInside(const V3f& _v) const;
	bool isInside(const V4f& _v) const;
	bool isInside(const BoundingBox& _b) const;
	bool isOutside(const V3f& _v) const;
	bool isOutside(const BoundingBox& _b) const;
	bool isOutside(const V4f& _v) const;
	void computeEdgeSizes(V3f& _sizes) const;
	float computeDiameter() const;

	static bool hasSharedComponent(const BoundingBox& _bb1, const BoundingBox& _bb2);

private:
	V3f mPrivMin;
	V3f mPrivMax;
	V3f mPrivEdgeSizes;
	V3f mPrivCenter;
	void drawImmediate();
	void drawLineStrip();

};

} // end of Namespace OOCTools

#endif /* BOUNDINGBOX_H_ */
