/**
 * @file	TriBoxTest.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 12.04.2009
 * @brief 	Class declaration of TriBoxTest
 *
 */

#ifndef TRIBOXTEST_H
#define TRIBOXTEST_H

namespace ooctools {

/**
 * @class TriBoxTest
 * @brief Little wrapper-class for the original c-file tribox.c.
 * This makes the project a little easier for older gcc versions. And
 * it looks far more c++-stylish.
 */
class TriBoxTest
{
	public:
		/**
		 * @brief Performs an intersection-test between a plane and a box.
		 * @param normal
		 * @param d
		 * @param maxbox
		 * @return True if they intersect.
		 */
		bool planeBoxOverlap(float normal[3], float d, float maxbox[3]) const;

		/**
		 * @brief Performs an intersection-test between a triangle and a box.
		 * @param boxcenter Center coordinates of the box.
		 * @param boxhalfsize a vector containing half of the boxes edge-sizes.
		 * @param triverts The triangle itself.
		 * @return True if they intersect.
		 */
		bool triBoxOverlap(const float boxcenter[3], const float boxhalfsize[3],
				const float triverts[3][3]) const;
};

} // ooctools

#endif

