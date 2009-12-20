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

IVbo::IVbo() : mPriIndexCount(0), mPriVertexCount(0), mPriByteSize(0),
	mPriVertexId(0), mPriIdxId(0), mPriIsOffline(true), mPriFree(false)

{

}

void IVbo::debug()
{
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

void IVbo::setOnline()
{
	std::cerr << " -------------------------------------- setting vbo online...." << std::endl;
	if (mPriIsOffline){

		// do stuff to normals
		if (mPriVertexId==0){
			std::cerr << "generating vertexBuffer4VBO...." << std::endl;
			glGenBuffers(1, &mPriVertexId);
		}
		else {
			std::cerr << "somewhow we already have a vertexBuffer4VBO.... " << mPriVertexId << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
		glBufferData(GL_ARRAY_BUFFER, mPriVertexCount*sizeof(V4N4), vertexData(), GL_STATIC_DRAW);
		glVertexPointer(4, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
		glNormalPointer(GL_BYTE, sizeof(V4N4), bufferOffset(0));
		// do stuff to indices
		if (mPriIdxId==0){
			std::cerr << "generating indexBuffer4VBO...." << std::endl;
			glGenBuffers(1, &mPriIdxId);
		}
		else {
			std::cerr << "somewhow we already have a indexBuffer4VBO.... " << mPriIdxId << std::endl;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPriIndexCount*sizeof(unsigned), indexData(), GL_STATIC_DRAW);
		mPriIsOffline = false;
	}
}

void IVbo::setOffline()
{
	if (!mPriIsOffline){
		if (mPriVertexId!=0){
			// do stuff to normals
			glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
			glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		}
		if (mPriIdxId!=0){
			// do stuff to indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		}
		mPriIsOffline=true;
	}
}

void IVbo::draw(bool dataNodeMode)
{
	if (dataNodeMode){
		glEnableClientState(GL_VERTEX_ARRAY);

		if (!mPriIsOffline){
			if (mPriVertexId!=0) {
				glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
				glVertexPointer(3, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
			}
			if (mPriIdxId!=0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			}

			glDrawElements(GL_TRIANGLES, mPriIndexCount, GL_UNSIGNED_INT, bufferOffset(0));
		}
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		if (!mPriIsOffline){
			if (mPriVertexId!=0) {
				glBindBuffer(GL_ARRAY_BUFFER, mPriVertexId);
				glVertexPointer(4, GL_FLOAT, sizeof(V4N4), bufferOffset(0));
				glNormalPointer(GL_BYTE, sizeof(V4N4), bufferOffset(4*sizeof(float)));
			}
			if (mPriIdxId!=0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPriIdxId);
			}

			glDrawElements(GL_TRIANGLES, mPriIndexCount, GL_UNSIGNED_INT, bufferOffset(0));
		}
	}
}

} // end of namespace OOCTools
