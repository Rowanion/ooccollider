/**
 * @file declarations.h
 *
 * @file	declarations.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 *
 * @brief Conveniently place all forward declarations in one place.
 * @date Created on: 2009/03/22 17:04:31
 *
 * Declares all classes of the respective namespace.
 *
 * Please add classes in alphabetical order!
 *
 * This is a compromise between including the header of a required type into the referencing header
 * (strong dependencies between files, automatically consistent) and locally declaring each used
 * type in the referencing header (no dependencies, consistency nightmare, redundant code).
 *
 * By providing declarations in a central place, there is only one place where the forward declaration
 * of a new type needs to be added. The downside is that the introduction of a new type means that the
 * whole project needs to be recompiled, but as this should happen much more infrequently than changes
 * to header files in general, this seems like a reasonable compromise.
 */

namespace ooctools{
//	class AbstractVertex;
	class BoundingBox;
	class CgToolkit;
	class ColorTable;
	class Face;
	class Fbo;
	class FboFactory;
	class MetaGroup;
	class Model;
	class Vbo;
	class VboManager;
	template<typename T> class VertexArray;
	class V3b;
	class V3f;
	class V3ub;
	class V4f;

	struct Material;
	struct FileHeader;
	struct MtlMap;


} // end of namespace ooctools
