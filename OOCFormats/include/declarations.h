/*! \file declarations.h
    \author Timo Wiesemann <ava@upb.de>
    \date $Date: 2009/03/22 17:04:31 $
    \brief Conveniently place all forward declarations in one place.

    Declares all classes of the respective namespace.

    Please add classes in alphabetical order!

    \b Rationale
      This is a compromise between including the header of a required type into the referencing header
      (strong dependencies between files, automatically consistent) and locally declaring each used
      type in the referencing header (no dependencies, consistency nightmare, redundant code).

      By providing declarations in a central place, there is only one place where the forward declaration
      of a new type needs to be added. The downside is that the introduction of a new type means that the
      whole project needs to be recompiled, but as this should happen much more infrequently than changes
      to header files in general, this seems like a reasonable compromise.


*/

namespace oocformats {
	class IModelLoader;
	class FileHeader;
	class FileIO;
	class ModelWriter;
	class ObjModelLoader;
	class RawModelWriter;
	class RawModelLoader;
	class OctreeHandler;
} // end of namespace oocformats
