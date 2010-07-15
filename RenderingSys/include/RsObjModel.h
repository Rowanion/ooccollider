/**
 * @file	RsObjModel.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsObjModel class declaration.
 */

#ifndef RSOBJMODEL_H_
#define RSOBJMODEL_H_

#include "RsAbstractVbo.h"
#include "RsStructs.h"

/**
 * @class RsObjModel
 * @brief This represents a 3D Mesh loaded from an OBJ-file.
 * For convenience (in sacrificing generic) each OBJ-group is stored in a VBO and gets a material assigned.
 */
class RsObjModel {
public:
	RsObjModel();
	RsObjModel(const RsObjInfo* _info);

	/**
	 * @brief This methods constructs a VBO for the model by the given data.
	 * Note, that this model has to be constructed beforehand by means of a ObjInfo pointer.
	 * @param _info - Again, the MetaInformation of the whole model. - Maybe change this at a later point.
	 * @param _gIdx - The group-index of the vbo to be created
	 * @param _group - An array of indices for the current group, from which the VBO will be created from.
	 * Mind you though - the number of components and their meaning is dictated by the bitField in the
	 * ObjInfo-Pointer.
	 * @param _vertices - The pure vertex-data-array which will be accessed via the indices in _group.
	 * @param _normals - The pure normal-data-array which will be accessed via the indices in _group - if present.
	 * @param _texCoords - The pure TexCoord-data-array which will be accessed via the indices in _group - if present.
	 * @param _colors - The pure color-data-array which will be accessed via the indices in _group - not implemented yet.
	 */
	void addVbo(const RsObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);
	void addVboDebug(const RsObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);
	virtual ~RsObjModel();
	void addVbo(const RsAbstractVbo* _vbo);

	/**
	 * @brief This method calls the draw method of each VBO in a row.
	 */
	void draw();
	void draw(unsigned _idx);
	void drawDebug();

	unsigned getVboCount();

private:
	unsigned mPriVboCount;
	RsAbstractVbo** mPriVbos;


	unsigned idxCount;
	unsigned vCount;
	unsigned* iData;
	float* vData;
};

#endif /* RSOBJMODEL_H_ */
