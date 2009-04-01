/*
 * VBOToolkit.h
 *
 *  Created on: 08.02.2009
 *      Author: ava
 */

#ifndef VBO_H_
#define VBO_H_

#include <string>
#include <vector>

#include "GlToolkit.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "VertexArray.h"

#include "declarations.h"

namespace ooctools {

class Vbo
{

friend class VboManager;

public:
	Vbo();
	virtual ~Vbo();

	void drawSingle(int vertexCount);
	void draw(int vertexCount);
	void debug();

	//getter and setter methods
	void setVData(VertexArray<float>* va);
	void setNData(VertexArray<char>* va);
	void setIndices(VertexArray<unsigned int>* _ia);
	void setCgP(CGparameter cgp);
	void setGlColor();
	void setOffline();
	void setOnline();
	void setGpuOnly();
	void useGlColor(bool color){mPriUseGlColor = color;};
	void setColor(V3f *_color);
	void setColor(float r, float g, float b);
	void setColor(V3ub *_color);

	BoundingBox *getBbPtr(){return mPriVertices->mPriBb;};
	size_t getCpuMemory();
	size_t getGpuMemory();
	// --------------------------------
	std::string toString(GLboolean b);
	void purge();


private:
	void managedDraw();

	GLuint mPriVId;
	GLuint mPriNId;
	GLuint mPriIdxId;
	bool mPriUseIndices;
	GLuint _vboUsageMode;
	VertexArray<char>* mPriNormals;
	VertexArray<float>* mPriVertices;
	VertexArray<unsigned int>* mPriIndices;
	int mPriMaxVertices, mPriMaxIndices;
	int _upperVertexBound;
	float *mPriCurrentColorF;
	unsigned char *mPriCurrentColorB;
	CGparameter mPriDiffuseParameter;
	bool mPriUseGlColor;
	bool isOffline;
	bool isGpuOnly;

	static void enableNPtr(){glEnableClientState(GL_NORMAL_ARRAY);};
	static void enableVPtr(){glEnableClientState(GL_VERTEX_ARRAY);};
	static void enableIPtr(){glEnableClientState(GL_INDEX_ARRAY);};
	static void disableNPtr(){glDisableClientState(GL_NORMAL_ARRAY);};
	static void disableVPtr(){glDisableClientState(GL_VERTEX_ARRAY);};
	static void disableIPtr(){glDisableClientState(GL_INDEX_ARRAY);};

};

} // end of namespace OOCTools

#endif /* VBO_H_ */
