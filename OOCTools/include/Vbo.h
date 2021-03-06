/**
 * @file	Vbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 08.02.2009
 * @brief 	Vbo class declaration.
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
 * @class Vbo
 * @brief Representation of a Vertexbuffer-Object.
 * A Vbo (in our case) consists of a VertexArray<float> containing the vertex-data and a
 * VertexArray<char> containing the normals and (eventually) a VertexArray<unsigned int> containing
 * the indices for vertices. But the latter is still gray theory - we'll get to that in the
 * optimization-stage.
 */
class Vbo
{

friend class VboManager;

public:
	Vbo(VertexArray<unsigned int>* _ia = 0, VertexArray<char>* _na = 0, VertexArray<float>* _va = 0);
	Vbo(const Vbo& _vbo);
	virtual ~Vbo();

	void drawSingle(int vertexCount);
	void draw(int vertexCount);
	void debug();

	//getter and setter methods

	/**
	 * @brief sets the vertex data for the VBO.
	 * @param va the vertexarray.
	 * @param useGl [true] Indicated whether the method should initialize the gl-side
	 * of the function as well. Useful if you don't intent to draw the VBOs but you
	 * like to encapsulate the vertexarray, indexarray and normalarray into a convenient
	 * and easy way.
	 * @see setNData(), setIndices()
	 */
	void setVData(VertexArray<float>* va, bool useGl = true);

	/**
	 * @brief sets the normal data for the VBO.
	 * @param va the normalarray.
	 * @param useGl [true] Indicated whether the method should initialize the gl-side
	 * of the function as well. Useful if you don't intent to draw the VBOs but you
	 * like to encapsulate the vertexarray, indexarray and normalarray into a convenient
	 * and easy way.
	 * @see setVData(), setIndices()
	 */
	void setNData(VertexArray<char>* va, bool useGl = true);

	/**
	 * @brief sets the vertex data for the VBO.
	 * @param va the vertexarray.
	 * @param useGl [true] Indicated whether the method should initialize the gl-side
	 * of the function as well. Useful if you don't intent to draw the VBOs but you
	 * like to encapsulate the vertexarray, indexarray and normalarray into a convenient
	 * and easy way.
	 * @see setVData(), setNData()
	 */
	void setIndices(VertexArray<unsigned int>* _ia, bool useGl = true);
	const VertexArray<float>* getVData() const {return mPriVertices;};
	const VertexArray<char>* getNData() const {return mPriNormals;};
	const VertexArray<unsigned int>* getIndices() const {return mPriIndices;};
	void setCgP(CGparameter cgp);
//	void setGlColor();
	void setOffline();
	void setOnline();
	void setGpuOnly();
//	void useGlColor(bool color){mPriUseGlColor = color;};
//	void setColor(V3f *_color);
//	void setColor(float r, float g, float b);
//	void setColor(V3ub *_color);

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

	bool hasVertices() const;
	bool hasIndices() const;
	bool hasNormals() const;

	Vbo operator=(const Vbo& _rhs);
	bool operator==(const Vbo& _rhs);

	/**
	 * @brief removes double triangles from the VertexArrays.
	 * It will remove all duplicate triangles and their normals. It's color, ie. the
	 * 4th component as the texture coordinate will be taken into account, too. So if we would
	 * have two exact copies of a triangle with different texture coordinates, nothing gets
	 * removed. I made this method, because when objects get split up, there are a some
	 * redundant triangles which remain in both sides of the split. If somehow two VBOs
	 * re merged later on, these triangles would be doubled and pile up.
	 */
	void stripDoubleTriangles();

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
