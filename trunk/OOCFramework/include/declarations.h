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

/**
 * @namespace oocframework
 * @brief This namespace contains all files that reside in the core of the framework
 * like Interface-Definitions, the EventManager, AbstractEvents, etc.
 */
namespace oocframework{
class AbstractEventListener;
	class CCollisionProtocol;
	class ClassId;
	class EventManager;
	class TestException;
	class IEvent;
	class VboFactory;
	class Logger;
	class LowMemoryEvent;
	class Message;
	class MpiControl;
	class VirtualNode;
	class VirtualRequest;


} // end of namespace oocframework
