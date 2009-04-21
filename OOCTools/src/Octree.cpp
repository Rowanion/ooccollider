/**
 * @file	Octree.cpp
 * @author	TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date 	Created on: 13.04.2009
 * @brief Class definition of Octree
 */

#include "Octree.h"

#include <string>

#include "BoundingBox.h"
#include "declarations.h"
#include "OctreeNode.h"

using namespace std;
namespace ooctools {

Octree::Octree(const BoundingBox& sBb, string _path) : OctreeNode(0, sBb, _path)
{
	// TODO Auto-generated constructor stub

}

Octree::~Octree()
{

	// TODO Auto-generated destructor stub
}




} // ooctools
