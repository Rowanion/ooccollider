/**
 * @file	MemTools.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 10.12.2009
 * @brief 	MemTools class declaration.
 */


#include <string.h>

#include "MemTools.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stddef.h>

MemTools* MemTools::_instance = 0;

#ifndef WINDOWS

float MemTools::usedMem() {
   this->priPID = getpid();

   char aFilename[40];

   sprintf(aFilename, "/proc/%d/statm", this->priPID);

   std::ifstream aFileStream(aFilename);
   std::string aLine = "";
   getline(aFileStream, aLine);


   int aProgSize, aPartSize, aNumPages, aPagesProg, aPagesStack,
aPagesLib, aPagesDamaged;
   sscanf(aLine.c_str(), "%d %d %d %d %d %d %d", &aProgSize,
&aPartSize, &aNumPages, &aPagesProg, &aPagesStack, &aPagesLib,
&aPagesDamaged);

   printf("Programsize    : %dKB %fMB \n", aProgSize,
aProgSize/1024.0);
   printf("MemParts       : %dKB %fMB \n", aPartSize,
aPartSize/1024.0);
   printf("# Pages        : %d\n", aNumPages);
   printf("# ProgPages    : %d\n", aPagesProg);
   printf("# StackPages   : %d\n", aPagesStack);
   printf("# LibPages     : %d\n", aPagesLib);
   printf("# DamagedPages : %d\n", aPagesDamaged);

   return ((float)aProgSize/1024.0);
}

void MemTools::freeMem() {
   sleep(1);
   std::ifstream aFileStream("/proc/meminfo");

   std::string aLine = "";

   do {
       aLine = "";
       getline(aFileStream, aLine);
       if (aLine.size()) {
           size_t aPos = aLine.find("MemFree", 0);

           if (aPos != std::string::npos) {
               std::cerr << aLine << std::endl;
               break;
           }
       }
   } while (!aFileStream.eof());
}

#endif

void* MemTools::memRealloc(void* _toResize, unsigned _oldsize, unsigned
_newsize) {
   char* aNewArray = new char[_newsize];

   if (_oldsize <= _newsize) {
       memcpy(aNewArray, _toResize, _oldsize);
   } else {
       memcpy(aNewArray, _toResize, _newsize);
   }

   delete [] (char*) _toResize;

   return (void*) aNewArray;
}
