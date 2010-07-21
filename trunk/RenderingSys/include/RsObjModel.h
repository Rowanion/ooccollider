/**
 * @file	RsObjModel.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsObjModel class declaration.
 */

#ifndef RSOBJMODEL_H_
#define RSOBJMODEL_H_

#include "GL/glew.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"

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
	virtual ~RsObjModel();

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
	 * @param _materialId - The material ID which belongs to this VBO.
	 */
	void addVbo(const RsObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0, unsigned _materialId = 0);
	void addVboDebug(const RsObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);

	/**
	 * @brief sets the list of materials for this object.
	 * @param _info
	 */
	void setMaterials(const RsObjInfo* _info);

	/**
	 * @brief This method calls the draw method of each VBO in a row.
	 * @param _shader if a shader is given, then the appropriate parameters are set up as well.
	 */
	void draw(CGprogram _shader = 0);

	/**
	 * @brief This method calls the draw method of the given VBO.
	 * @param _idx Index of vbo.
	 * @param _shader if a shader is given, then the appropriate parameters are set up as well.
	 */
	void draw(unsigned _idx, CGprogram _shader = 0);
	void drawDebug();

	/**
	 * @brief returns the number of VBOs contained within this object.
	 * @return
	 */
	unsigned getVboCount();

private:
	unsigned mPriVboCount;
	RsAbstractVbo** mPriVbos;
	RsMaterial* mPriMaterials;
	unsigned mPriMaterialCount;
	unsigned* mPriGroupMaterial;

	unsigned idxCount;
	unsigned vCount;
	unsigned* iData;
	float* vData;
};

#endif /* RSOBJMODEL_H_ */
