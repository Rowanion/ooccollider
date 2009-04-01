/*
 * Model.h
 *
 *  Created on: 30.01.2009
 *      Author: ava
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "GlToolkit.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <map>
#include <vector>

#include "BoundingBox.h"
#include "ColorTable.h"
#include "StructDefs.h"

#include "declarations.h"


namespace ooctools {

class Model
{
public:
	Model();
	virtual ~Model();
	void addGrpPtr(MetaGroup* g);
	void addVPtr(V3f* _vertex);
	void addTPtr(V3f* _texture);
	void addNPtr(V3b* _normal);
	void addFPtrToCurrent(Face *_face);
	void incVCount(int i){mPriVertexCount+=i;};
	void incVCount(){incVCount(1);};
	void incTCount(int i){mPriTextureCount+=i;};
	void incTCount(){incTCount(1);};
	void incNCount(int i){mPriNormalCount+=i;};
	void incNCount(){incNCount(1);};
	void incFCount(int i){mPriFaceCount+=i;};
	void incFCount(){incFCount(1);};
	void incGCount(int i){mPriGrpCount+=i;};
	void incGCount(){incGCount(1);};
	int getGrpCount(){return mPriGrpCount;};
	int getVertexCount(){return mPriVertexCount;};
	int getUniqueVertexCount(){return mPriUniqueVertexCount;};
	int getTextureCount(){return mPriTextureCount;};
	int getUniqueTextureCount(){return mPriUniqueTextureCount;};
	int getNormalCount(){return mPriNormalCount;};
	int getUniqueNormalCount(){return mPriUniqueNormalCount;};
	int getFaceCount(){return mPriFaceCount;};
	void setObjectSize(int i);
	MetaGroup* getGrpPtr(std::string _id);
	VertexArray<float>* getVArrayPtr(MetaGroup* _grpPtr);
	VertexArray<float>* getVArrayPtr(std::string _id);
	VertexArray<float>* getVArrayPtr(std::map<std::string, MetaGroup*>::iterator _mapIt);
	VertexArray<char>* getNArrayPtr(MetaGroup* _grpPtr);
	VertexArray<char>* getNArrayPtr(std::string _id);
	VertexArray<char>* getNArrayPtr(std::map<std::string, MetaGroup*>::iterator _mapIt);
	void setCurrentGrp(std::string);
	MetaGroup *getCurrentGrpPtr();
	void expandBB(V3f* _v);
	void expandBB(V4f* _v);
	const BoundingBox* getBB() const {return &mPriBB;};
	std::map<std::string, MetaGroup*>::iterator getGrpStart();
	std::map<std::string, MetaGroup*>::iterator getGrpEnd();
	V3f* getVPtr(size_t _id);
	V3f* getTPtr(size_t _id);
	V3b* getNPtr(size_t _id);
	void resetGrpPtr(){mPriCurrentGrpPtr = mPriGrpPtrMap.begin()->second;};
	void calculateNormals();
	void setColorTable(ColorTable& _ct);
	const ColorTable& getColorTable() const {return mPriCTable;};


private:
	int mPriGrpCount;
	int mPriMaterialCount;
	size_t mPriFaceCount;
	size_t mPriVertexCount;
	size_t mPriTextureCount;
	size_t mPriNormalCount;

	int mPriUniqueVertexCount;
	int mPriUniqueNormalCount;
	int mPriUniqueTextureCount;

	BoundingBox mPriBB;
	ColorTable mPriCTable;

	std::vector<V3f*> mPriVertexPtrList;
	std::vector<V3f*> mPriTexturePtrList;
	std::vector<V3b*> mPriNormalPtrList;
	std::map<std::string, MetaGroup*> mPriGrpPtrMap;

	MetaGroup* mPriCurrentGrpPtr;
	void createDefaultMaterial();
	void setMaterialShader(Material* mat);


};

} // end of namespace OOCTools
#endif /* MODEL_H_ */
