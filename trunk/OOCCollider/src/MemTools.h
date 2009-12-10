/**
 * @file	MemTools.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 10.12.2009
 * @brief 	MemTools class declaration.
 */


#ifndef MEMTOOLS_H_
#define MEMTOOLS_H_

class MemTools {
public:

   static MemTools* getSingleton() {
       static CGuard g;
       if (!_instance)
           _instance = new MemTools();
       return _instance;
   }
#ifndef WINDOWS
   float usedMem();
   void freeMem();
#endif
   void* memRealloc(void* _toResize, unsigned _oldsize, unsigned
_newsize);

private:
   static MemTools* _instance;

   int priPID;
   MemTools() {
   };
   MemTools(const MemTools&);

   ~MemTools() {
   };

   class CGuard {
   public:

       ~CGuard() {
           if (0 != MemTools::_instance) {
               delete MemTools::_instance;
               MemTools::_instance = 0;
           }
       }
   };
   friend class CGuard;
};



#endif  /* _MEMTOOLS_H */
