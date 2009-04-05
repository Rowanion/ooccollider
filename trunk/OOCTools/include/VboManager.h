/**
 * @file	VboManager.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.02.2009
 *
 */

#ifndef VBOMANAGER_H_
#define VBOMANAGER_H_

#include <string>
#include <map>
#include "CgToolkit.h"

#include "ColorTable.h"
#include "BoundingBox.h"
#include "declarations.h"
#include "../../OOCTools/include/declarations.h"

namespace ooctools {

/**
 * @class C
 * @brief B
 *
 * will be a friend of VBO
 * according to its name serves as a container for multiple VBOs
 * thus enabling it to draw individual VBOs without having a gl-statechange between vbo-calls
 * additional bonus with this wrapper: we can have a global bounding box and a
 * global vertex- and face-count
 */
class VboManager
{
	typedef std::map<std::string, Vbo*>::iterator mIterator;
	typedef std::map<std::string, Vbo*>::const_iterator mcIterator;

public:
	virtual ~VboManager();
	void drawVbos();
	void drawVbo(std::string id);
	void drawNormals();
	void drawBbs(int r, int g, int b);
	void drawBbs(float r, float g, float b);
	void drawBb(std::string id, float r, float g, float b);
	void drawBb(std::string id, int r, int g, int b);
	void addVbo(std::string id, Vbo *_vbo);
	Vbo *getVbo(std::string id);
	Vbo *getVbo(mIterator it);
	void delVbo(std::string id);
	void delVbo(mIterator it);
	BoundingBox& getBb(){return mPriBb;};
	void setCgDiffParam(CGparameter _cgp){mPriCgDiffuseParam = _cgp;};
	void setUseGlColor(bool _usage){usingGlColor = _usage;};
	bool getUseGlColor(){return usingGlColor;};
	void makeVbos(Model *_model);
	void printInfo();
	ColorTable& getColorTable();
	void setColorTable(const ColorTable& _ct);

	static VboManager *getInstancePtr();

	void switchOffVBO(std::string id);
	void switchOnVBO(std::string id);

private:
	VboManager();
	void calcBB();

	size_t mPriNVertices;
	size_t mPriNFaces;
	std::map<std::string, Vbo*> *mPriVboMap;
	BoundingBox mPriBb;
	bool usingIdxPtr;
	bool usingVertexPtr;
	bool usingNormalPtr;
	bool usingGlColor;
	CGparameter mPriCgDiffuseParam;
	size_t mPriMemoryUsage;
	ColorTable mPriCt;

	static VboManager *mPriInstancePtr;
};

} // end of namespace OOCTools

#endif /* VBOMANAGER_H_ */
