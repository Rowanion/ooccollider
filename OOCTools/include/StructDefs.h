/**
 * @file	StructDefs.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.02.2009
 * @brief 	Conveniently place all structs in one place.
 *
 */

#ifndef STRUCTDEFS_H_
#define STRUCTDEFS_H_

#include <map>
#include <string>
#include <vector>
#include <cstddef>

#include "V3ub.h"
#include "V3f.h"
#include "BoundingBox.h"

#include "declarations.h"

namespace ooctools {

//struct Material{
//	std::string name;
//	V3f *Ka; // ambient color r|g|b
//	V3f *Kd; // diffuse color r|g|b
//	V3f *Ks; // specular color r|g|b
//	float Tr; // transparency (default 1.0) [can be either Tr or d)
//	float Ns; // shininess (default 0.0)
//	int illum; //Illumination Model; 1=flat, 2=using specular
//	std::string map_Ka; // texture map; ASCII dump of rgb-vals *puke*
//};

/**
 * @struct Material
 * @brief Very simple RedGreenBlue container for material.
 * Note that in life most material definitions aren't that simple. But because of the CAD-Model
 * we're using we only have one color for a whole object. I chose to use the definition
 * color = DiffuseColor = Material.
 */
struct Material
{
	float kdR; // diffuse color r
	float kdG; // diffuse color g
	float kdB; // diffuse color b
};

struct MtlMap
{
	std::string name;
	std::map<std::string, Material*> matMap;
};

struct V4N4{
	float v[4];
	char n[4];
};

struct StatisticalData{
	unsigned int totalObjects;
	unsigned int totalMemory;
	unsigned int totalTris;
	unsigned int totalNodes;
	unsigned int maxLevel;
	// nodes containing data
	unsigned int nodesPerLevel[17];
	unsigned int memoryPerLevel[17];
	unsigned int trisPerLevel[17];
	unsigned int objectsPerLevel[17];
	float trisPerNodePerLevel[17];
	float objectDistributionPerLevel[17];

	float objectsPerNode;
	float trisPerNode;


};

} // end of namespace OOCTools
#endif /* STRUCTDEFS_H_ */
