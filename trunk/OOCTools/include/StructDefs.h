/**
 * @file	StructDefs.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.02.2009
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
 * @struct C
 * @brief B
 * L
 */
struct Material
{
	float kdR; // diffuse color r
	float kdG; // diffuse color g
	float kdB; // diffuse color b
};

/**
 * @struct C
 * @brief B
 * L
 */
struct MtlMap
{
	std::string name;
	std::map<std::string, Material*> matMap;
};

} // end of namespace OOCTools
#endif /* STRUCTDEFS_H_ */
