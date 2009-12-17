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

Quintuple::Quintuple()
{
}

Quintuple::Quintuple(int _lvl, float _dist, int _destId, uint64_t _id, int _isExt) :
	lvl(_lvl), dist(_dist), destId(_destId), priority(_isExt), id(_id)
{
}

void Quintuple::set(int _lvl, float _dist, int _destId, uint64_t _id, int _isExt)
{
	lvl = _lvl;
	dist = _dist;
	destId = _destId;
	id = _id;
	priority = _isExt;
}

void Quintuple::set(Quintuple rhs)
{
	lvl = rhs.lvl;
	dist = rhs.dist;
	destId = rhs.destId;
	id = rhs.id;
	priority = rhs.priority;
}

void Quintuple::set(const Quintuple* rhs)
{
	lvl = rhs->lvl;
	dist = rhs->dist;
	destId = rhs->destId;
	id = rhs->id;
	priority = rhs->priority;
}

bool Quintuple::operator<(const Quintuple& rhs) const
{
	if (priority == rhs.priority){
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
	else{
		return priority < rhs.priority;
	}
}

Visibility::Visibility(uint64_t _id, char _vis) : id(_id), visible(_vis)
{
}

void IVbo::debug()
{
	std::cerr << "iID " << mPriId << " - " << ((mem_t)&mPriId) - ((mem_t)this) << std::endl;
	std::cerr << "iIdxc " << mPriIndexCount << " - " << ((mem_t)&mPriIndexCount) - ((mem_t)this) << std::endl;
	std::cerr << "iVertc " << mPriVertexCount << " - " << ((uint64_t)&mPriVertexCount) - ((mem_t)this) << std::endl;
	std::cerr << "iSize " << mPriByteSize << " - " << ((mem_t)&mPriByteSize) - ((mem_t)this) << std::endl;
	std::cerr << "iIdx " << " - " << (mem_t)(indexData())-((mem_t)this) << std::endl;
	std::cerr << "iVert " << " - " << ((mem_t)vertexData())-((mem_t)this) << std::endl;
	std::cerr << "indices " << indexData()[0] << ", " << indexData()[1] << ", " << indexData()[2] << std::endl;
	std::cerr << "vertices " << vertexData()[0].v[0] << ", " << vertexData()[0].v[1] << ", " << vertexData()[0].v[2] << std::endl;
}

unsigned* IVbo::indexData()
{
	return (unsigned*)(this+1);
}

V4N4* IVbo::vertexData()
{
	return (V4N4*) (((char*)(this+1)) + (mPriIndexCount*sizeof(unsigned)));
}

uint64_t IVbo::getId()
{
	return mPriId;
}

unsigned IVbo::getIndexCount()
{
	return mPriIndexCount;
}

unsigned IVbo::getVertexCount()
{
	return mPriVertexCount;
}

unsigned IVbo::getByteSize()
{
	return mPriByteSize;
}

//bool operator<(const Quintuple& lhs, const Quintuple& rhs)
//{
//	if (lhs.destId == rhs.destId){
//		if (lhs.lvl == rhs.lvl){
//			if (lhs.dist == rhs.dist){
//				return (lhs.id < rhs.id);
//			}
//			else {
//				return (lhs.dist < rhs.dist);
//			}
//		}
//		else {
//			return (lhs.lvl < rhs.lvl);
//		}
//	}
//	else{
//		return lhs.destId < rhs.destId;
//	}
//}

} // end of namespace OOCTools
