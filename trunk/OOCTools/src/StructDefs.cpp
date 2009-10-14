/**
 * @file	StructDefs.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.02.2009
 * @brief 	the matching cpp-file for structDefs.h.
 *
 */


#include "StructDefs.h"
#include "declarations.h"

namespace ooctools {

Triple::Triple(){
}

Triple::Triple(int _lvl, float _dist, uint64_t _id) : lvl(_lvl), dist(_dist), id(_id){
}

bool Triple::operator<(const Triple& rhs) const {
	if (lvl == rhs.lvl){
		if (dist == rhs.dist){
			return (id < rhs.id);
		}
		else {
			return (dist < rhs.dist);
		}
	}
	else {
		return (lvl < rhs.lvl);
	}
}

Tuple::Tuple(){
}

Tuple::Tuple(int _lvl, float _dist) : lvl(_lvl), dist(_dist){
}

bool Tuple::operator<(const Tuple& rhs) const {
	if (lvl == rhs.lvl){
			return (dist < rhs.dist);
	}
	else {
		return (lvl < rhs.lvl);
	}
}

} // end of namespace OOCTools
