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

struct Tuple{
	Tuple();
	Tuple(int _lvl, float _dist);

	int lvl;     // octree-level of this node
	float dist;  // distance of node-center to eye
	bool operator<(const Tuple& rhs) const;
};

struct Triple{
	Triple();
	Triple(int _lvl, float _dist, uint64_t _id);
	void set(int _lvl, float _dist, uint64_t _id);
	void set(Triple rhs);
	void set(const Triple* rhs);

	int lvl;     // octree-level of this node
	float dist;  // distance of node-center to eye
	uint64_t id; // node-id
	bool operator<(const Triple& rhs) const;
};

struct Quadruple{
	Quadruple();
	Quadruple(int _lvl, float _dist, int destId, uint64_t _id);
	void set(int _lvl, float _dist, int destId, uint64_t _id);
	void set(Quadruple rhs);
	void set(const Quadruple* rhs);

	int lvl;     // octree-level of this node
	float dist;  // distance of node-center to eye
	int destId;  // mpi-rank of requesting node
	uint64_t id; // node-id
	bool operator<(const Quadruple& rhs) const;
};

struct Quintuple{
	Quintuple();
	Quintuple(int _lvl, float _dist, int _destId, uint64_t _id, int _isExt);
	void set(int _lvl, float _dist, int _destId, uint64_t _id, int _isExt);
	void set(Quintuple rhs);
	void set(const Quintuple* rhs);

	int lvl;     // octree-level of this node
	float dist;  // distance of node-center to eye
	int destId;  // mpi-rank of requesting node
	int priority;	 // states wheather this node is in the extended frustum or not.
	uint64_t id; // node-id

	bool operator<(const Quintuple& rhs) const;
};

//bool operator<(const Quintuple& lhs, const Quintuple& rhs);
} // end of namespace OOCTools
#endif /* STRUCTDEFS_H_ */
