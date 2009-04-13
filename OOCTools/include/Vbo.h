/**
 * @file	Vbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 08.02.2009
 *
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

/**
 * @class C
 * @brief B
 * L
 */
class Vbo
{

friend class VboManager;

public:
	Vbo(VertexArray<unsigned int>* _ia = 0, VertexArray<char>* _na = 0, VertexArray<float>* _va = 0);
	virtual ~Vbo();

	void drawSingle(int vertexCount);
	void draw(int vertexCount);
	void debug();

	//getter and setter methods
	void setVData(VertexArray<float>* va);
	void setNData(VertexArray<char>* va);
	void setIndices(VertexArray<unsigned int>* _ia);
	const VertexArray<float>* getVData() const {return mPriVertices;};
	const VertexArray<char>* getNData() const {return mPriNormals;};
	const VertexArray<unsigned int>* getIndices() const {return mPriIndices;};
	void setCgP(CGparameter cgp);
	void setGlColor();
	void setOffline();
	void setOnline();
	void setGpuOnly();
	void useGlColor(bool color){mPriUseGlColor = color;};
	void setColor(V3f *_color);
	void setColor(float r, float g, float b);
	void setColor(V3ub *_color);

	const BoundingBox& getBb() const {return mPriVertices->mPriBb;};
	size_t getCpuMemory();
	size_t getGpuMemory();
	// --------------------------------
	std::string toString(GLboolean b);
	void purge();

	Vbo& operator+=(const Vbo& rhs);
	Vbo operator+(const Vbo& rhs);

	/**
	 * Equality-Operator returns true if both VBO-Objects have the same VertexArray-Memory Adresses
	 * @param rhs - RightHandSide object
	 * @return boolen - see above
	 */
	bool operator==(const Vbo& rhs) const;

	/**
	 * Performs a thorough tri-box-intersection-test on this Vbo and the given BB.
	 * The results are stored inside the _inside and _outside references. Triangles
	 * which lie on an edge are duplicated in both target-objects.
	 * @param _bb the BoundingBox
	 * @param _inside will contain all the triangles and normals that lie inside the
	 * box and on an edge.
	 * @param _outside will contain all the triangles and normals that lie outside
	 * the box and on an edge.
	 */
	void split(const BoundingBox& _bb, Vbo& _inside, Vbo& _outside);

	/**
	 * adds a single triangle, consisting of 3 4-component float vertices and char normals
	 * to the Vbo.
	 * @param _verts
	 * @param _normals
	 */
	void addTriangle(const float* _verts, const char* _normals);

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
