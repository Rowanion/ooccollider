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

Triple::Triple(){
}

Triple::Triple(int _lvl, float _dist, uint64_t _id) : lvl(_lvl), dist(_dist), id(_id){
}

void Triple::set(int _lvl, float _dist, uint64_t _id)
{
	lvl = _lvl;
	dist = _dist;
	id = _id;
}

void Triple::set(Triple rhs)
{
	lvl = rhs.lvl;
	dist = rhs.dist;
	id = rhs.id;
}

void Triple::set(const Triple* rhs)
{
	lvl = rhs->lvl;
	dist = rhs->dist;
	id = rhs->id;
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

Quadruple::Quadruple(){
}

Quadruple::Quadruple(int _lvl, float _dist, int _destId, uint64_t _id) : lvl(_lvl), dist(_dist), destId(_destId), id(_id){
}

void Quadruple::set(int _lvl, float _dist, int _destId, uint64_t _id)
{
	lvl = _lvl;
	dist = _dist;
	destId = _destId;
	id = _id;
}

void Quadruple::set(Quadruple rhs)
{
	lvl = rhs.lvl;
	dist = rhs.dist;
	destId = rhs.destId;
	id = rhs.id;
}

void Quadruple::set(const Quadruple* rhs)
{
	lvl = rhs->lvl;
	dist = rhs->dist;
	destId = rhs->destId;
	id = rhs->id;
}

bool Quadruple::operator<(const Quadruple& rhs) const {
	if (destId == rhs.destId){
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
	else{
		return destId < rhs.destId;
	}
}

Quintuple::Quintuple(){
}

Quintuple::Quintuple(int _lvl, float _dist, int _destId, uint64_t _id, bool _isExt) : lvl(_lvl), dist(_dist), destId(_destId), id(_id), isExt(_isExt){
}

void Quintuple::set(int _lvl, float _dist, int _destId, uint64_t _id, bool _isExt)
{
	lvl = _lvl;
	dist = _dist;
	destId = _destId;
	id = _id;
	isExt = _isExt;
}

void Quintuple::set(Quintuple rhs)
{
	lvl = rhs.lvl;
	dist = rhs.dist;
	destId = rhs.destId;
	id = rhs.id;
	isExt = rhs.isExt;
}

void Quintuple::set(const Quintuple* rhs)
{
	lvl = rhs->lvl;
	dist = rhs->dist;
	destId = rhs->destId;
	id = rhs->id;
	isExt = rhs->isExt;
}

bool Quintuple::operator<(const Quintuple& rhs) const {
	if (destId == rhs.destId){
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
	else{
		return destId < rhs.destId;
	}
}

} // end of namespace OOCTools
