/**
 * @file	ObjModel.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	ObjModel class declaration.
 */

#ifndef OBJMODEL_H_
#define OBJMODEL_H_

#include "Vbo.h"
#include "RsStructs.h"

class ObjModel {
public:
	ObjModel();
	ObjModel(const ObjInfo* _info);

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
	void addVbo(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);
	void addVboDebug(const ObjInfo* _info, unsigned _gIdx, const unsigned* _group, const float* _vertices, const char* _normals = 0, const float* _texCoords = 0, const unsigned char* _colors = 0);
	virtual ~ObjModel();
	void addVbo(const Vbo* _vbo);

	/**
	 * @brief This method calls the draw method of each VBO in a row.
	 */
	void draw();
	void drawDebug();

	/**
	 * @brief This method computes the 3D-Texture-Corrdinates of the model-enclosing boundingbox.
	 * It adds the arrays of calculated texture-coordinates directly to the model's VBOs.
	 * @param _calcIndividual Should the boundingbox assumed around the complete model, i.e. a box
	 * enclosing all groups/VBOs or should a box calculated around each individual VBO?
	 */
	void calcWrapingTexCoords(bool _calcIndividual);

private:
	unsigned mPriVboCount;
	Vbo** mPriVbos;

	unsigned idxCount;
	unsigned vCount;
	unsigned* iData;
	float* vData;
};

#endif /* OBJMODEL_H_ */
